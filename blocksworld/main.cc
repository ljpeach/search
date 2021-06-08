#include "blocksworld.hpp"
#include "../search/main.hpp"
#include <cstdio>

int main(int argc, const char *argv[]) {
	dfheader(stdout);
	Blocksworld d(stdin);
    /*
    Blocksworld::State test = d.initialstate();
    d.dumpstate(stdout, test);
    test.moveblock(Blocksworld::Oper(2,4), d);
    d.dumpstate(stdout, test);
    test.moveblock(Blocksworld::Oper(2,10), d);
    d.dumpstate(stdout, test);
    */
    search<Blocksworld>(d, argc, argv);
	dffooter(stdout);
	return 0;
}
