#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>

#define MAXBLOCKS 65536
#if NBLOCKS > MAXBLOCKS
#error Too many blocks for unsigned short typed blocks.
#endif

extern "C" unsigned long hashbytes(unsigned char[], unsigned int);

class Blocksworld{
public:
    typedef unsigned short Block;
	typedef unsigned int Cost;
    enum{Nblocks = NBLOCKS};
	// The type of an operator which can be
	// applied to a state.  This is usually just an
	// integer but it may be some more complex
	// class.  Searches assume that operator==
	// is defined on the Oper class.

    struct Oper{
        Block from;
        Block to;

        Oper() : from(0), to(0){}
        Oper(Block b1, Block b2){
            from = b1;
            to = b2;
        }

        bool operator==(const Oper &o) const {
			return from == o.from && to == o.to;
        }
    };

    static const Oper Nop;//a "do nothing" action

	Blocksworld(FILE*);

	struct State {
        bool eq(const Blocksworld*, const State &o) const {
			for (unsigned int i = 0; i < Nblocks; i++) {
				if (below[i] != o.below[i])
					return false;
			}
			return true;
		}

		unsigned long hash(const Blocksworld*) const {
            return hashbytes((unsigned char *) below,
						Nblocks * sizeof(Block));
		}

        bool operator==(const State &o) const {
			for(int i = 0; i<Nblocks; i++){
                if(o.below[i] != below[i]) return false;
            }
            return true;
		}

        void moveblock(Oper move, const Blocksworld &domain)
        {
            Block pickUp = move.from;
            Block putOn = move.to;
            Block block = pickUp;
            //If at any point in the stack, the block below doesn't match what is in the goal, remove 1 from h.
            while(block!=0)
            {
                if(below[block-1] != domain.goal[block-1]){
                    h--;
                    break;
                }
                block = below[block-1];
            }
            //remove block from top of old stack (set block below to have nothing on top)
            if(below[pickUp-1] != 0) above[below[pickUp-1]-1] = 0;
            if(putOn != 0) {
                above[putOn-1] = pickUp;
                below[pickUp-1] = putOn;
                //see if block's new location is correct (add 1 to h if not)
                if(below[pickUp-1]!=domain.goal[pickUp-1]){
                     h++;
                 }
                //otherwise move down stack, if any are out of place, add 1 to h.
                else{
                    block = below[pickUp-1];
                    while(block!=0)
                    {
                        if(below[block-1] != domain.goal[block-1]){
                            h++;
                            break;
                        }
                        block = below[block-1];
                    }
                }
            }
            else{
                below[pickUp-1] = 0;
                if(domain.goal[pickUp-1]!=0) h++;
            }
            d = h;
        }

    private:
        friend class Blocksworld;

        Block above[Nblocks];
        Block below[Nblocks];
        Cost h;
        Cost d;




    };

	// Memory-intensive algs such as A* which store
	// PackedStates instead of States.  Each time operations
	// are needed, the state is unpacked and operated
	// upon.
	//
	// If your state is as packed as it will get then you
	// can simply 'typedef State PackedState'
    typedef State PackedState;
    /*
	struct PackedState {
		Block *packedDown;

		// Functions for putting a packed state
		// into a hash table.
		bool operator==(const PackedState &o) const {
			for(int i = 0; i<nblocks; i++){
                if(o[i] != *packedDown[i]) return False;
            }
            return True;
		}
	};
    */

	// Get the initial state.
	State initialstate();

	// Get the heuristic.
	Cost h(const State &s) const {
		return s.h;
	}

	// Get a distance estimate.
	Cost d(const State &s) const {
		return s.d;
	}

	// Is the given state a goal state?
	bool isgoal(const State &s) const {
		return s.h  == 0;
	}

	// Operators implements an vector of the applicable
	// operators for a given state.
	struct Operators {
		Operators(const Blocksworld&, const State& s){
            int stacks = 0;
            int tabled = 0;
            for(int i = 0; i< Nblocks; i++){
                if(s.above[i] == 0 && s.below[i] != 0) stacks++;
                else if(s.above[i] == 0) tabled++;
            }
            n = tabled * (stacks + tabled -1) + stacks * (stacks + tabled);
            int * tops = new int[stacks+tabled];
            int pos = 0;
            for(int i = 0; i< Nblocks; i++){
                if(s.above[i] == 0) {
                    tops[pos] = i;
                    pos++;
                }
            }
            pos = 0;
            Oper * temp = new Oper[n];
            mvs = temp;
            for(int pickUp = 0; pickUp < stacks+tabled; pickUp++){
                for(int putOn = 0; putOn < stacks+tabled; putOn++){
                    if(pickUp!=putOn) mvs[pos] = Oper(tops[pickUp]+1, tops[putOn]+1);
                    else if(s.below[tops[pickUp]]==0) continue;
                    else mvs[pos] = Oper(tops[pickUp]+1, 0);
                    pos++;
                }
            }
            delete [] tops;

        }

		// size returns the number of applicable operatiors.
		unsigned int size() const {
            return n;
		}

		// operator[] returns a specific operator.
		Oper operator[] (unsigned int i) const {
			return mvs[i];
		}

    private:
        Oper *mvs;
        int n;
	};

	struct Edge {
		Cost cost;
		Oper revop;
		Cost revcost;

		// The state field may or may not be a reference.
		// The reference variant is used in domains that
		// do in-place modification and the non-reference
		// variant is used in domains that do out-of-place
		// modification.
		//State state;
		State &state;
        Blocksworld &domain;
		// Applys the operator to thet state.  Some domains
		// may modify the input state in this constructor.
		// Because of this, a search algorithm may not
		// use the state passed to this constructor until
		// after the Edge's destructor has been called!
		Edge(Blocksworld& d, State &s, Oper move) :
                cost(1), revop(Oper(move.from, s.below[move.from-1])),
                revcost(1), state(s), domain(d){
            state.moveblock(move, domain);
        }

		// The destructor is expected to undo any changes
		// that it may have made to the input state in
		// the constructor.  If a domain uses out-of-place
		// modification then the destructor may not be
		// required.
		~Edge(void) {
            state.moveblock(revop, domain);
        }
	};

	// Pack the state into the destination packed state.
	// If PackedState is the same type as State then this
	// should at least copy.
	void pack(PackedState &dst, State &src) const {
		dst = src;
	}

	// Unpack the state and return a reference to the
	// resulting unpacked state.  If PackedState and
	// State are the same type then the packed state
	// can just be immediately returned and used
	// so that there is no need to copy.
	State &unpack(State &buf, PackedState &pkd) const {
		return pkd;
	}

	// Print the state.
	void dumpstate(FILE *out, const State &s) const {
        Block printarray[Nblocks][Nblocks];
        for(int i=0; i<Nblocks; i++){
            for(int j=0; j<Nblocks; j++) printarray[i][j]=0;
        }
        int stack = 0;
        int height = 0;
        int maxheight = 0;
        Block current;
        unsigned int spaces = std::to_string(Nblocks).length();
		for(int i=0; i<Nblocks; i++){
            if(s.below[i]==0){
                current = i+1;
                while(current!=0){
                    printarray[stack][height] = current;
                    current = s.above[current-1];
                    height++;
                }
                if(height>maxheight) maxheight = height;
                height = 0;
                stack++;
            }
        }

        for(int j=maxheight; j>=0; j--){
            for(int i=0; i<stack; i++){
                unsigned int fill = spaces - std::to_string(printarray[i][j]).length();
                for(unsigned int k=0; k< fill; k++) fprintf(out, " ");
                if(printarray[i][j]==0) fprintf(out, "  ");
                else fprintf(out, "%u ", printarray[i][j]);
            }
            fprintf(out, "\n");
        }
        fprintf(out, "h: %d\n", s.h);
	}

	Cost pathcost(const std::vector<State>&, const std::vector<Oper>&);
private:
    Block init[Nblocks];
    Block goal[Nblocks];

    Cost noop(Block above[], Block below[]) {
        Cost oop = 0;
        for(int i=0; i<Nblocks; i++){
            if(below[i] == 0){
                bool incorrect = false;
                Block current = i+1;
                while(current != 0){
                    if(!incorrect && below[current-1] != goal[current-1]) incorrect = true;
                    if(incorrect) oop++;
                    current = above[current-1];
                }
            }
        }
        return oop;
    }
};
