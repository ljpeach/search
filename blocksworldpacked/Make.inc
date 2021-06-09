blocksworldpacked:\
	blocksworldpacked/10blocksworldpacked_solver\
    blocksworldpacked/20blocksworldpacked_solver\
    blocksworldpacked/50blocksworldpacked_solver\
    blocksworldpacked/100blocksworldpacked_solver\

CLEAN+=\
    blocksworldpacked/10blocksworldpacked_solver

EVERYTHING+=blocksworldpacked

BLOCKSWORLDPACKEDRC=\
	blocksworldpacked/blocksworld.cc\

CLEAN+=$(BLOCKSWORLDPACKEDRC:.cc=.o)

blocksworldpacked/%blocksworldpacked_solver: blocksworldpacked/main.d
blocksworldpacked/%blocksworldpacked_solver:\
	$(BLOCKSWORLDPACKEDRC)\
	blocksworldpacked/main.cc\
	utils/utils.a\
	structs/structs.a\
	search/search.a
	@echo $@
	@$(CXX) $(BLOCKSWORLDPACKEDRC)\
		blocksworldpacked/main.cc\
		utils/utils.a\
		structs/structs.a\
		search/search.a\
		$(LDFLAGS) -DNBLOCKS=$*  -o $@

include blocksworldpacked/main.d
