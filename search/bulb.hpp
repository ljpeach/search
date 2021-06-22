#include "search.hpp"
#include "../utils/pool.hpp"
#include <tuple>
#include <unordered_map>
#include <vector>

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

    Bulb(int argc, const char *argv[]) :
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

    ~Bulb(){
        delete nodes;
    }

    void search(D &d, typename D::State &s0) {
		this->start();
		closed.init(d);

		Node *n0 = init(d, s0);
		closed.add(n0);
		open.push(n0);

        int discrepancies = 0;

		while (!SearchAlgorithm<D>::limit()) {
			Node *n = bulbprobe(0, discrepancies);

			State buf, &state = d.unpack(buf, n->state);
			if (d.isgoal(state)) {
				solpath<D, Node>(d, n, this->res);
				break;
			}

            discrepancies++;
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
		dfpair(stdout, "open list type", "%s", open.kind());
		dfpair(stdout, "node size", "%u", sizeof(Node));
	}

private:

    Node* bulbprobe(Cost g, int disc){
        std::vector<Node*> slice;
        Cost val;
        int ind;
        expand(g, 0, &slice, &val, &ind);
        if(val >= 0 || val == -2) {
            if (val > -1) return slice[0];
            else return NULL;
        }
        if(disc == 0){
            if(slice == NULL) return NULL;
            Node* pathNode = bulbprobe(g+1, 0);
            clearBucket(g);
            return pathNode;
        }
        else{
            if(slice != NULL) clearBucket(g);
            while (true) {
                expand(g, 0, &slice, &val, &ind)
                if(val >= 0 || val == -2) {
                    if (val > -1) return slice[0];
                    break;
                }
                if(slice.empty()) continue;
                Node* pathNode = bulbprobe(g+1,disc);
                clearBucket(g);
                if(pathNode != NULL) return pathNode;
            }
            expand(g, 0, &slice, &val, &ind);
            if(val >= 0 || val == -2) {
                if (val > -1) return slice[0];
                else return NULL;
            }
            if(slice == NULL) return NULL;
            pathNode = bulbprobe(g+1, disc);
            return pathNode;
        }
    }

    void clearBucket(Cost g){
        Node * remNode;
        while(!open[g].empty()) {
            remNode = open[g].pop_back();
            closed.remove(remNode.state, kid->state.hash(&d));
            nodes.destruct(remNode);
        }
        open[g].clear();
    }

    void expand(D &d, Cost g, int index, std::vector<Node*> &slice, Cost &val, int &ind) {
        slice.clear();
        std::vector<Node*> succs;
        Node* child;
        int i;
        for(Node* n : open[g]){
            State buf, &state = d.unpack(buf, n->state);
            typename D::Operators ops(d, state);
            SearchAlgorithm<D>::res.expd++;
            for(i = 0; i < ops.size(); i++) {
                child = considerkid(d, n, state, ops[i]);
                if(child!=NULL) succs.push_back(child);
            }
        }
        std::sort(succs.cbegin, succs.cend, Node::pred());

        if(succs.empty() || index == succs.size()){
            delete succs;
            val = -2;
            ind = -1;
            return;
        }
        buf, &state = d.unpack(buf, succs[0]->state);
        if(d.isgoal(state)) {
            slice.push_back(succs[0]);
            delete succs;
            val = g+1;
            ind = -1;
            return;
        }
        slice.clear();
        i = index;
        while(i < valid && i - index < width){
            if(SearchAlgorithm<D>::limit()){
                for(int j = index; j < i; j++){
                    closed.remove(open[n->g][j], open[n->g][j]->state.hash(&d));
                    nodes.destruct(open[n->g][j]);
                    open[n->g].pop_back();
                }
                slice.clear();
                val = -2;
                ind = -1;
                return;
            }
            if(closed.find(kid->state, hash)==NULL){
                slice.push_back(succs[i]);
                open[n->g].push_back(succs[i]);
                closed.add(succs[i]->state, succs[i]->state.hash(&d));
            }
            i++;
        }
        val = -1;
        ind = i;
        return;
    }

    Node* considerkid(D &d, Node *parent, State &state, Oper op) {
        Node* kid = nodes->construct();
		assert (kid);
		typename D::Edge e(d, state, op);
		kid->g = parent->g + e.cost;
		d.pack(kid->state, e.state);

		kid->h = d.h(e.state);
		kid->parent = parent;
		kid->op = op;
		kid->pop = e.revop;

		State buf, &state = d.unpack(buf, kid->state);
        unsigned long hash = kid->state.hash(&d);
        if(closed.find(kid->state, hash)!=NULL)
		    return kid;
        nodes.destruct(kid);
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

    int maxsize;
    int width;
    int maxdisc;
    std::vector<vector<Node*>> open;
    ClosedList<Node, Node, D> closed;
    Pool<Node> *nodes;
};
