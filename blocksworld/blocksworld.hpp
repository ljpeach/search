#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <vector>

int MAXBLOCKS = 65536;

#if NBLOCKS >= MAXBLOCKS
#error Too many blocks for unsigned short typed blocks.
#endif

class Blocksworld{
public:
    typedef unsigned short Block;
	typedef unsigned int Cost;

	// The type of an operator which can be
	// applied to a state.  This is usually just an
	// integer but it may be some more complex
	// class.  Searches assume that operator==
	// is defined on the Oper class.
    static const Oper Nop = Oper(NULL, NULL);//a "do nothing" action

	Blocksworld(FILE*);

    struct Oper{
        typedef Block from;
        typedef Block to;

        Oper(Block b1, Block b2){
            from = b1;
            to = b2;
        }
    };

	struct State {

    private:
        friend class Blocksworld;

        Block above[nblocks];
        Block below[nblocks];
        Cost h;
        Cost d;

        void moveblock(Oper move, const Blocksworld &d.)
        {
            Block pickUp = move.from;
            Block putOn = move.to;
            if(below[pickUp]!=d.goal[pickUp]) h--;
            else
            {
                int block = below[pickUp];
                while(block!=NULL)
                {
                    if(below[block] != d.goal[block]){
                        h--;
                        break;
                    }
                    block = below[block];
                }
            }
            if(below[pickUp] != NULL) above[below[pickUp]] = NULL;
            if(putOn != 0) {
                if(below[pickUp]!=d.goal[putOn]) h++;
                else
                {
                    int block = below[putOn];
                    while(block!=NULL)
                    {
                        if(below[block] != d.goal[block]){
                            h++;
                            break;
                        }
                        block = below[block];
                    }
                }
                above[putOn - 1] = pickUp;
                below[pickUp] = putOn-1;
            }
            else below[pickUp] = NULL;
            d = h;
        }

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
	unsigned long hash(const PackedState&) const {
		return -1;
	}

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
            for(int i = 0; i< nblocks; i++){
                if(s.above[i] == NULL && s.below != NULL) stacks++;
                else if(s.above[i] == NULL) tabled++;
            }
            n = tabled * (stacks + tabled -1) + stacks * (stacks + tabled)
            int * tops= new Oper[stacks+tabled]
            int pos = 0
            for(int i = 0; i< nblocks; i++){
                if(s.above[i] == NULL) {
                    tops[pos] = i;
                    pos++;
                }
            }
            pos = 0;
            *mvs = new Oper[n]
            for(Block pickUp : tops){
                Block upOper = pickup;
                if (s.below[pickUp] != NULL) {
                    *mvs[pos] = upOper;
                    pos++;
                }
                for(int putOn : tops){
                    *mvs[pos] = Oper(upOper, putOn+1);
                    pos++;
                }
            }
            del tops;
        }
        //42949 67296
        //1234500000
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
		State &state;
		// State &state

		// Applys the operator to thet state.  Some domains
		// may modify the input state in this constructor.
		// Because of this, a search algorithm may not
		// use the state passed to this constructor until
		// after the Edge's destructor has been called!
		Edge(const Blocksworld&, const State& s, Oper move) {
            state = s;
            cost = 1;
            Block inHand = move.from;
            Block oldBottom = below[inHand];
            if (oldBottom = NULL) oldBottom = 0;
            else oldBottom++;
            revop = Oper(inHand, oldBottom);
            revcost = 1;
            state.moveblock(move);
        }

		// The destructor is expected to undo any changes
		// that it may have made to the input state in
		// the constructor.  If a domain uses out-of-place
		// modification then the destructor may not be
		// required.
		~Edge(void) {
            state.moveblock(revop);
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
		fatal("Unimplemented");
	}

	Cost pathcost(const std::vector<State>&, const std::vector<Oper>&);
private:
    Block init[nblocks];
    Block goal[nblocks];

    static Cost noop(Block below[], Block above[]) {
        Cost oop = 0;
        for(Block block : blocks)
        {
            if(below[block] == NULL)
            {
                bool incorrect = false;
                Block current = block;
                while(current != NULL){
                    if(!incorrect && below[current] != goal[current]) incorrect = true;
                    if(incorrect) oop++;
                    current = above[current];
                }
            }
        }
        return oop;
    }
};
