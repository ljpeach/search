bwdp:\
	bwdp/10bwdp_solver\
    bwdp/20bwdp_solver\
    bwdp/50bwdp_solver\
    bwdp/100bwdp_solver\
    bwdp/10bw_solver\
    bwdp/20bw_solver\
    bwdp/50bw_solver\
    bwdp/100bw_solver\


CLEAN+=\
    bwdp/10bwdp_solver\
    bwdp/10bwdp_solver\
    bwdp/20bwdp_solver\
    bwdp/50bwdp_solver\
    bwdp/100bwdp_solver\
    bwdp/10bw_solver\
    bwdp/20bw_solver\
    bwdp/50bw_solver\
    bwdp/100bw_solver\

EVERYTHING+=bwdp

BWRC=\
	bwdp/blocksworld.cc\

CLEAN+=$(BWRC:.cc=.o)

bwdp/%bwdp_solver: bwdp/main.d
bwdp/%bwdp_solver:\
	$(BWRC)\
	bwdp/main.cc\
	utils/utils.a\
	structs/structs.a\
	search/search.a
	@echo $@
	@$(CXX) $(BWRC)\
		bwdp/main.cc\
		utils/utils.a\
		structs/structs.a\
		search/search.a\
		$(LDFLAGS) -DNBLOCKS=$* -DDEEP -o $@

bwdp/%bw_solver: bwdp/main.d
bwdp/%bw_solver:\
	$(BWRC)\
	bwdp/main.cc\
	utils/utils.a\
	structs/structs.a\
	search/search.a
	@echo $@
	@$(CXX) $(BWRC)\
		bwdp/main.cc\
		utils/utils.a\
		structs/structs.a\
		search/search.a\
		$(LDFLAGS) -DNBLOCKS=$* -o $@

include bwdp/main.d
