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
        maxnodes = -1;
        maxdisc = -1;
        for (int i = 0; i < argc; i++) {
			if (i < argc - 1 && strcmp(argv[i], "-width") == 0)
				width = atoi(argv[++i]);
            if (i < argc - 1 && strcmp(argv[i], "-disc") == 0)
				maxdisc = atoi(argv[++i]);
            if (i < argc - 1 && strcmp(argv[i], "-hashsize") == 0)
				maxnodes = atoi(argv[++i]);
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
        candidate = NULL;

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

    Node* bulbprobe(D &d, unsigned int depth, int disc){
        Node * pathNode;
        int ind;
        //pseudo: <SLICE, value, index> := nextSlice(depth, 0, h(.), B)
        expand(d, depth, 0, ind);
        //pseudo: if value>=0, then return value. (return path length if deadend/solution found)
        if(candidate!=NULL) return candidate;
        else if(ind == -1) return NULL;
        //pseudo: if discrepancies=0 then:
        if(disc == 0){
            //pseudo: if slice = empty set, then return inf
            if(open[depth+1].empty()) return NULL;
            //pseudo: pathLength = BULBprobe(depth+1, 0, --)
            pathNode = bulbprobe(d, depth+1, 0);
            //pseudo: remove SLICE from hash table
            if(!pathNode) clearBucket(d, depth+1);
            //pseudo: return pathLength
            return pathNode;
        }
        else{
            //pseudo: if slice is an empty set, then remove SLICE from the hash table
            if(!open[depth+1].empty()) clearBucket(d, depth+1);
            while (true) {
                //pseudo: <SLICE, value, index> := nextSlice(depth, index, --)
                expand(d, depth, 0, ind);
                //pseudo: if value >=0,
                if(candidate!=NULL)
                    return candidate;
                else if(ind == -1) break;
                //pseudo: if slice is empty then continue
                if(open[depth+1].empty()) continue;
                //pseudo: pathLength := bulbprobe(depth+1, discrepancies-1, --)
                pathNode = bulbprobe(d, depth+1, disc-1);
                //pseudo: remove slice from hash table
                if(!pathNode) clearBucket(d, depth+1);
                //pseudo: if pathlength<inf, return pathLength (if solution)
                if(pathNode != NULL) return pathNode;
            }
            //pseudo: slice... etc. nxt slice(depth, 0, --)
            expand(d, depth, 0, ind);
            //pseudo: if value>=0 return value
            if(candidate!=NULL) return candidate;
            else if(ind == -1) return NULL;
            //pseudo: if slice is empty, return inf
            if(open[depth+1].empty()) return NULL;
            //pseudo: path length = BP(depth+1, disc, --)
            pathNode = bulbprobe(d, depth+1, disc);
            if(!pathNode) clearBucket(d, depth+1);
            return pathNode;
        }
    }

    void clearBucket(D &d, unsigned int depth){
        Node * remNode;
        while(!open[depth].empty()) {
            remNode = open[depth].back();
            open[depth].pop_back();
            closed.remove(remNode->state, remNode->state.hash(&d));
            nodes->destruct(remNode);
        }
        open[depth].clear();
    }

    void expand(D &d, unsigned int depth, int index, int &ind) {
        //generateNewSuccessors
        std::vector<Node*> succs;
        Node* child;
        int i, j;
        for(Node* n : open[depth]){
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
            candidate = NULL;
            ind = -1;
            return;
        }
        State buf, &state = d.unpack(buf, succs[0]->state);
        if(d.isgoal(state)) {
            //clear unused succs
            for(i=1; i<(int)succs.size(); i++){
                nodes->destruct(succs[i]);
            }
            candidate = succs[0];
            ind = -1;
            return;
        }
        i = index;
        while(i < (int)succs.size() && i - index < width){
            if(SearchAlgorithm<D>::limit() || closed.getFill()>=maxnodes){
                if(i!=0) clearBucket(d, depth+1);
                //clear unused succs
                for(j=i; j<(int)succs.size(); j++){
                    nodes->destruct(succs[j]);
                }
                candidate = NULL;
                ind = -1;
                return;
            }
            unsigned long hash = succs[i]->state.hash(&d);
            Node* dup = closed.find(succs[i]->state, hash);
            if(dup == NULL || succs[i]->h < dup->h){
                intoOpen(depth+1, succs[i]);
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
        candidate = NULL;
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
        Node* dup = closed.find(kid->state, hash);
        if(dup == NULL || kid->h < dup->h) return kid;
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
    unsigned int maxnodes;
    Node* candidate;
    std::vector<std::vector<Node*>> open;
    ClosedList<Node, Node, D> closed;
    Pool<Node> *nodes;
};
