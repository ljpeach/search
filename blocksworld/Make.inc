blocksworld:\
	blocksworld/9blocksworld_solver\
	blocksworld/14blocksworld_solver

CLEAN+=\
	blocksworld/9blocksworld_solver\
	blocksworld/14blocksworld_solver

EVERYTHING+=blocksworld

BLOCKSWORLDRC=\
	blocksworld/blocksworld.cc\

CLEAN+=$(BLOCKSWORLDRC:.cc=.o)

blocksworld/%blocksworld_solver: blocksworld/main.d
blocksworld/%blocksworld_solver:\
	$(BLOCKSWORLDRC)\
	blocksworld/main.cc\
	utils/utils.a\
	structs/structs.a\
	search/search.a
	@echo $@
	@$(CXX) $(BLOCKSWORLDRC)\
		blocksworld/main.cc\
		utils/utils.a\
		structs/structs.a\
		search/search.a\
		$(LDFLAGS) -DNBLOCKS=$*  -o $@

include blocksworld/main.d
