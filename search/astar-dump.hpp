#include "../search/search.hpp"
#include "../utils/pool.hpp"

// Astar_dump is like A* except that
// it dumps information about nodes
// on the solution path to the datafile.
// This information can be useful for
// learning heuristics from off-line
// training instances.
template <class D> struct Astar_dump : public SearchAlgorithm<D> {

	typedef typename D::State State;
	typedef typename D::PackedState PackedState;
	typedef typename D::Cost Cost;
	typedef typename D::Oper Oper;
	
	struct Node : SearchNode<D> {
		Cost f, d, inedge;
		unsigned int depth;
		Node *parentnode;
	
		static bool pred(Node *a, Node *b) {
			if (a->f == b->f)
				return a->g > b->g;
			return a->f < b->f;
		}

		static Cost prio(Node *n) { return n->f; }

		static Cost tieprio(Node *n) { return n->g; }
	};

	Astar_dump(int argc, const char *argv[]) :
		SearchAlgorithm<D>(argc, argv), closed(30000001) {
		nodes = new Pool<Node>();
	}

	~Astar_dump() {
		delete nodes;
	}

	void search(D &d, typename D::State &s0) {
		this->start();
		closed.init(d);

		Node *n0 = init(d, s0);
		closed.add(n0);
		open.push(n0);

		while (!open.empty() && !SearchAlgorithm<D>::limit()) {
			Node *n = open.pop();
			State buf, &state = d.unpack(buf, n->packed);

			if (d.isgoal(state)) {
				SearchAlgorithm<D>::res.goal(d, n);
				dumpgoal(n);
				break;
			}

			expand(d, n, state);
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

	void dumpgoal(Node *goal) const {
		dfrowhdr(stdout, "path", 6, "g", "h", "D", "d", "h*", "d*");

		double hstar = 0;
		double dstar = 0;
		for (Node *n = goal; n; n = n->parentnode) {
			dfrow(stdout, "path", "gggggg",
				(double) n->g,
				(double) n->f - n->g,
				(double) n->depth,
				(double) n->d,
				hstar,
				dstar);
			hstar += (double) n->inedge;
			dstar++;
		}
	}

	virtual void output(FILE *out) {
		SearchAlgorithm<D>::output(out);
		closed.prstats(stdout, "closed ");
		dfpair(stdout, "open list type", "%s", open.kind());
		dfpair(stdout, "node size", "%u", sizeof(Node));
	}

private:

	void expand(D &d, Node *n, State &state) {
		SearchAlgorithm<D>::res.expd++;

		typename D::Operators ops(d, state);
		for (unsigned int i = 0; i < ops.size(); i++) {
			if (ops[i] == n->pop)
				continue;
			SearchAlgorithm<D>::res.gend++;
			considerkid(d, n, state, ops[i]);
		}
	}

	void considerkid(D &d, Node *parent, State &state, Oper op) {
		Node *kid = nodes->construct();
		assert (kid);
		typename D::Edge e(d, state, op);
		kid->depth = parent->depth + 1;
		kid->g = parent->g + e.cost;
		kid->inedge = e.cost;
		d.pack(kid->packed, e.state);

		unsigned long hash = d.hash(kid->packed);
		Node *dup = static_cast<Node*>(closed.find(kid->packed, hash));
		if (dup) {
			this->res.dups++;
			if (kid->g >= dup->g) {
				nodes->destruct(kid);
				return;
			}
			bool isopen = open.mem(dup);
			if (isopen)
				open.pre_update(dup);
			dup->depth = kid->depth;
 			dup->inedge = kid->inedge;
			dup->f = dup->f - dup->g + kid->g;
			dup->parentnode = parent;
			dup->update(kid->g, parent, op, e.revop);
			if (isopen) {
				open.post_update(dup);
			} else {
				this->res.reopnd++;
				open.push(dup);
			}
			nodes->destruct(kid);
		} else {
			kid->f = kid->g + d.h(e.state);
			kid->d = d.d(e.state);
			kid->parentnode = parent;
			kid->update(kid->g, parent, op, e.revop);
			closed.add(kid, hash);
			open.push(kid);
		}
	}

	Node *init(D &d, State &s0) {
		Node *n0 = nodes->construct();
		d.pack(n0->packed, s0);
		n0->g = Cost(0);
		n0->inedge = Cost(0);
		n0->f = d.h(s0);
		n0->d = d.d(s0);
		n0->depth = 0;
		n0->pop = n0->op = D::Nop;
		n0->parent = NULL;
		n0->parentnode = NULL;
		return n0;
	}

	OpenList<Node, Node, Cost> open;
 	ClosedList<SearchNode<D>, SearchNode<D>, D> closed;
	Pool<Node> *nodes;
};
