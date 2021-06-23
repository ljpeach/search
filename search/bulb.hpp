#include "search.hpp"
#include "../utils/pool.hpp"
#include <tuple>
#include <vector>
//#include <unordered_map>


template <class D> struct BulbSearch : public SearchAlgorithm<D> {

	typedef typename D::State State;
	typedef typename D::PackedState PackedState;
	typedef typename D::Cost Cost;
	typedef typename D::Oper Oper;

    struct Node {
        ClosedEntry<Node, D> closedent;
		int openind;
		Node *parent;
		PackedState state;
		Oper op, pop;
		Cost h, g;

        Node() : openind(-1) {
		}

		static ClosedEntry<Node, D> &closedentry(Node *n) {
			return n->closedent;
		}

		static PackedState &key(Node *n) {
			return n->state;
		}

		static void setind(Node *n, int i) {
			n->openind = i;
		}

		static int getind(const Node *n) {
			return n->openind;
		}

		static bool pred(Node *a, Node *b) {
			return a->h < b->h;
		}

		static Cost prio(Node *n) {
			return n->h;
		}

		static Cost tieprio(Node *n) {
			return n->g;
		}

    };

    BulbSearch(int argc, const char *argv[]) :
		SearchAlgorithm<D>(argc, argv), closed(30000001) {
        for (int i = 0; i < argc; i++) {
			if (i < argc - 1 && strcmp(argv[i], "-width") == 0)
				width = atoi(argv[++i]);
            if (i < argc - 1 && strcmp(argv[i], "-disc") == 0)
				maxdisc = atoi(argv[++i]);
		}

		if (width < 1)
			fatal("Must specify a >0 beam width using -width");

		nodes = new Pool<Node>();
	}

    ~BulbSearch(){
        delete nodes;
    }

    void search(D &d, typename D::State &s0) {
		this->start();
		closed.init(d);
		Node *n0 = init(d, s0);
		closed.add(n0);
		intoOpen(0, n0);
        int discrepancies = 0;

		while (!SearchAlgorithm<D>::limit()) {
			Node *n = bulbprobe(d, 0, discrepancies);
            if(n){
    			State buf, &state = d.unpack(buf, n->state);
    			if (d.isgoal(state)) {
    				solpath<D, Node>(d, n, this->res);
    				break;
    			}
            }
            discrepancies++;
            if(discrepancies == maxdisc) break;
		}
		this->finish();
	}

    virtual void reset() {
		SearchAlgorithm<D>::reset();
		open.clear();
		closed.clear();
		delete nodes;
		nodes = new Pool<Node>();
	}

    virtual void output(FILE *out) {
		SearchAlgorithm<D>::output(out);
		closed.prstats(stdout, "closed ");
		dfpair(stdout, "open list type", "%s", "Node Pointer Vector Vector");
		dfpair(stdout, "node size", "%u", sizeof(Node));
	}

private:

    Node* bulbprobe(D &d, Cost g, int disc){
        Node * pathNode;
        std::vector<Node*> slice;
        Node * candidate = NULL;
        int ind;
        //pseudo: <SLICE, value, index> := nextSlice(depth, 0, h(.), B)
        expand(d, g, 0, slice, candidate, ind);
        //pseudo: if value>=0, then return value. (return path length if deadend/solution found)
        if(candidate!=NULL) return slice[0];
        else if(ind == -1) return NULL;
        //pseudo: if discrepancies=0 then:
        if(disc == 0){
            //pseudo: if slice = empty set, then return inf
            if(slice.empty()) return NULL;
            //pseudo: pathLength = BULBprobe(depth+1, 0, --)
            pathNode = bulbprobe(d, g+1, 0);
            //pseudo: remove SLICE from hash table
            clearBucket(d, g+1, slice.size());
            //pseudo: return pathLength
            return pathNode;
        }
        else{
            //pseudo: if slice is an empty set, then remove SLICE from the hash table
            if(!slice.empty()) clearBucket(d, g+1, slice.size());
            while (true) {
                //pseudo: <SLICE, value, index> := nextSlice(depth, index, --)
                expand(d, g, 0, slice, candidate, ind);
                //pseudo: if value >=0,
                if(candidate!=NULL)
                    return slice[0];
                else if(ind == -1) break;
                //pseudo: if slice is empty then continue
                if(slice.empty()) continue;
                //pseudo: pathLength := bulbprobe(depth+1, discrepancies-1, --)
                pathNode = bulbprobe(d, g+1, disc-1);
                //pseudo: remove slice from hash table
                clearBucket(d, g+1, slice.size());
                //pseudo: if pathlength<inf, return pathLength (if solution)
                if(pathNode != NULL) return pathNode;
            }
            //pseudo: slice... etc. nxt slice(depth, 0, --)
            expand(d, g, 0, slice, candidate, ind);
            //pseudo: if value>=0 return value
            if(candidate!=NULL) return slice[0];
            else if(ind == -1) return NULL;
            //pseudo: if slice is empty, return inf
            if(slice.empty()) return NULL;
            //pseudo: path length = BP(depth+1, disc, --)
            pathNode = bulbprobe(d, g+1, disc);
            return pathNode;
        }
    }

    void clearBucket(D &d, Cost g, unsigned int slicesize){
        Node * remNode;
        unsigned int removed = 0;
        while(!open[g].empty() && !(removed == slicesize)) {
            remNode = open[g].back();
            open[g].pop_back();
            closed.remove(remNode->state, remNode->state.hash(&d));
            nodes->destruct(remNode);
            removed++;
        }
        open[g].clear();
    }

    void expand(D &d, Cost g, int index, std::vector<Node*> &slice, Node* &cand, int &ind) {
        //generateNewSuccessors
        std::vector<Node*> succs;
        Node* child;
        int i, j;
        for(Node* n : open[g]){
            State buf, &state = d.unpack(buf, n->state);
            typename D::Operators ops(d, state);
            SearchAlgorithm<D>::res.expd++;
            for(i = 0; i < (int)ops.size(); i++) {
                child = considerkid(d, n, state, ops[i]);
                if(child!=NULL) succs.push_back(child);
            }
        }
        std::sort(succs.begin(), succs.end(), Node::pred);

        //nextSlice
        if(succs.empty() || index == (int)succs.size()){
            //clear unused succs
            for(i=0; i<(int)succs.size(); i++){
                nodes->destruct(succs[i]);
            }
            cand = NULL;
            ind = -1;
            return;
        }
        State buf, &state = d.unpack(buf, succs[0]->state);
        if(d.isgoal(state)) {
            slice.push_back(succs[0]);
            //clear unused succs
            for(i=1; i<(int)succs.size(); i++){
                nodes->destruct(succs[i]);
            }
            cand = succs[0];
            ind = -1;
            return;
        }
        slice.clear();
        i = index;
        while(i < (int)succs.size() && i - index < width){
            if(SearchAlgorithm<D>::limit()){
                for(j = i; j > index; j--){
                    closed.remove(open[g+1][j]->state, open[g][j]->state.hash(&d));
                    nodes->destruct(open[g+1][j]);
                    open[g+1].pop_back();
                }
                //clear unused succs
                for(j=0; j<(int)succs.size(); j++){
                    nodes->destruct(succs[j]);
                }
                slice.clear();
                cand = NULL;
                ind = -1;
                return;
            }
            unsigned long hash = succs[i]->state.hash(&d);
            if(closed.find(succs[i]->state, hash)==NULL){
                slice.push_back(succs[i]);
                intoOpen(g+1, succs[i]);
                closed.add(succs[i], hash);
            }
            i++;
        }
        //clear unused succs (before slice)
        for(int j=0; j<index; j++){
            nodes->destruct(succs[j]);
        }
        //clear unused succs (after slice)
        for(int j=i; j<(int)succs.size(); j++){
            nodes->destruct(succs[j]);
        }
        cand = NULL;
        ind = i;
        return;
    }

    Node* considerkid(D &d, Node *parent, State &state, Oper op) {
        SearchAlgorithm<D>::res.gend++;
        Node* kid = nodes->construct();
		assert (kid);
		typename D::Edge e(d, state, op);
		kid->g = parent->g + e.cost;
		d.pack(kid->state, e.state);

		kid->h = d.h(e.state);
		kid->parent = parent;
		kid->op = op;
		kid->pop = e.revop;

        unsigned long hash = kid->state.hash(&d);
        if(closed.find(kid->state, hash)==NULL)
		    return kid;
        nodes->destruct(kid);
        return NULL;
    }

    Node *init(D &d, State &s0) {
		Node *n0 = nodes->construct();
		d.pack(n0->state, s0);
		n0->g = Cost(0);
		n0->h = d.h(s0);
		n0->pop = n0->op = D::Nop;
		n0->parent = NULL;
		return n0;
	}

    void intoOpen(unsigned int depth, Node* node){
        if(open.size() <= depth){
            open.resize(depth+1, std::vector<Node*>(0));
        }
        open[depth].push_back(node);
    }

    int width;
    int maxdisc;
    int maxnodes;
    std::vector<std::vector<Node*>> open;
    ClosedList<Node, Node, D> closed;
    Pool<Node> *nodes;
};
