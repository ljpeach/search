#include "blocksworld.hpp"
#include "../utils/utils.hpp"
#include <cstdio>
#include <cerrno>

Blocksworld::Blocksworld(FILE *in) {
	unsigned int Ncakes;
	if (fscanf(in, "%u", &Nblocks) != 1)
		fatalx(errno, "Failed to read the number of blocks");
/*
	if (nblocks != Nblocks)
		fatal("Number of blocks instance/compiler option mismatch");
*/
    fscanf(in);
	for (unsigned int i = 0; i < Nblocks; i++) {
		if (fscanf(in, " %d", goal+i) != 1)
			fatalx(errno, "Failed to read block number %d", i);
	}
}

Blocksworld::State Blocksworld::initialstate() {
	State s;

	for (unsigned int i = 0; i < Nblocks; i++){
        s.lower[i] = init[i];
        if(s.lower[i]!=NULL) s.upper[s.lower[i]] = i;
    }
    for(unsigned int i = 0; i<Nblocks; i++){
        if(s.lower[i]==0) s.lower[i] = NULL;
        else s.lower[i]--;
        if(s.upper[i]==0) s.upper[i] = NULL;
        else s.upper[i]--;
    }
	s.h = noop(s.upper, s.lower, goal);

	return s;
}

Blocksworld::Cost Blocksworld::pathcost(const std::vector<State> &path, const std::vector<Oper> &ops) {
	State state = initialstate();
	Cost cost(0);
	for (int i = ops.size() - 1; i >= 0; i--) {
		State copy(state);
		Edge e(*this, copy, ops[i]);
		assert (e.state.eq(this, path[i]));
		state = e.state;
		cost += e.cost;
	}
	assert (isgoal(state));
	return cost;
}
