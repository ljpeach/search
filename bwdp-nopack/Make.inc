bwdp-nopack:\
	bwdp-nopack/10bwdp_solver\
    bwdp-nopack/20bwdp_solver\
    bwdp-nopack/50bwdp_solver\
    bwdp-nopack/100bwdp_solver\
    bwdp-nopack/10bw_solver\
    bwdp-nopack/20bw_solver\
    bwdp-nopack/50bw_solver\
    bwdp-nopack/100bw_solver\


CLEAN+=\
    bwdp-nopack/10bwdp_solver\
    bwdp-nopack/10bwdp_solver\
    bwdp-nopack/20bwdp_solver\
    bwdp-nopack/50bwdp_solver\
    bwdp-nopack/100bwdp_solver\
    bwdp-nopack/10bw_solver\
    bwdp-nopack/20bw_solver\
    bwdp-nopack/50bw_solver\
    bwdp-nopack/100bw_solver\

EVERYTHING+=bwdp-nopack

BWNPRC=\
	bwdp-nopack/blocksworld.cc\

CLEAN+=$(BWNPRC:.cc=.o)

bwdp-nopack/%bwdp_solver: bwdp-nopack/main.d
bwdp-nopack/%bwdp_solver:\
	$(BWNPRC)\
	bwdp-nopack/main.cc\
	utils/utils.a\
	structs/structs.a\
	search/search.a
	@echo $@
	@$(CXX) $(BWNPRC)\
		bwdp-nopack/main.cc\
		utils/utils.a\
		structs/structs.a\
		search/search.a\
		$(LDFLAGS) -DNBLOCKS=$* -DDEEP -o $@

bwdp-nopack/%bw_solver: bwdp-nopack/main.d
bwdp-nopack/%bw_solver:\
	$(BWNPRC)\
	bwdp-nopack/main.cc\
	utils/utils.a\
	structs/structs.a\
	search/search.a
	@echo $@
	@$(CXX) $(BWNPRC)\
		bwdp-nopack/main.cc\
		utils/utils.a\
		structs/structs.a\
		search/search.a\
		$(LDFLAGS) -DNBLOCKS=$* -o $@

include bwdp-nopack/main.d
