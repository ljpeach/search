#include "blocksworld.hpp"
#include "../utils/utils.hpp"
#include <cstdio>
#include <cerrno>
#include <iostream>

const Blocksworld::Oper Blocksworld::Nop;

Blocksworld::Blocksworld(FILE *in) {
	unsigned int Nblocks;
	if (fscanf(in, "%u\n", &Nblocks) != 1)
		fatalx(errno, "Failed to read the number of blocks");
    /*
	if (nblocks != Nblocks)
		fatal("Number of blocks instance/compiler option mismatch");
    */
    fscanf(in, "What each block is on:\n");
    for (unsigned int i = 0; i < Nblocks; i++) {
        if (fscanf(in, "%hu\n", &init[i]) != 1)
            fatalx(errno, "Failed to read basic block number %d", i);
    }

    fscanf(in, "Goal:\n");
	for (unsigned int i = 0; i < Nblocks; i++) {
        if (fscanf(in, "%hu\n", &goal[i]) != 1)
            fatalx(errno, "Failed to read basic block number %d", i);
	}

}

Blocksworld::State Blocksworld::initialstate() {
	State s;
    for(int i=0; i<Nblocks; i++) s.above[i] = 0;
	for (Block i = 0; i < Nblocks; i++){
        s.below[i] = init[i];
        if(s.below[i]!=0) s.above[s.below[i]-1] = i+1;
    }
	s.h = noop(s.above, s.below);
    s.d = s.h;
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
    printf("this is the assert\n");
	assert (isgoal(state));
	return cost;
}
