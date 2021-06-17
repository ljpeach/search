// © 2013 the Search Authors under the MIT license. See AUTHORS for the list of authors.

#include "mdist.hpp"
#include "../search/main.hpp"
#include <cstdio>

static SearchAlgorithm<TilesMdist> *get(int, const char *[]);

int main(int argc, const char *argv[]) {
	dfheader(stdout);

	FILE *lvl = stdin;
	const char *lvlpath = "";
	const char *cost = "unit";
	for (int i = 0; i < argc; i++) {
		if (i < argc - 1 && strcmp(argv[i], "-lvl") == 0)
			lvlpath = argv[++i];
		if(i < argc - 1 && strcmp(argv[i], "-cost") == 0)
			cost = argv[++i];
	}

	if (lvlpath[0] != '\0') {
		lvl = fopen(lvlpath, "r");
		if (!lvl)
			fatalx(errno, "Failed to open %s for reading", lvlpath);
	}

	dfpair(stdout, "cost", "%s", cost);
	TilesMdist d(lvl, cost);

	if (lvlpath[0] != '\0') {
	  dfpair(stdout, "level", "%s", lvlpath);
	  fclose(lvl);
	}
		
	searchGet<TilesMdist>(get, d, argc, argv);
	
	dffooter(stdout);
	return 0;
}

static SearchAlgorithm<TilesMdist> *get(int argc, const char *argv[]) {
	return getsearch<TilesMdist>(argc, argv);
}
