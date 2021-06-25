bwdp-nopack:\
	bwdp-nopack/10bwnpdp_solver\
    bwdp-nopack/20bwnpdp_solver\
    bwdp-nopack/50bwnpdp_solver\
    bwdp-nopack/100bwnpdp_solver\
    bwdp-nopack/10bwnp_solver\
    bwdp-nopack/20bwnp_solver\
    bwdp-nopack/50bwnp_solver\
    bwdp-nopack/100bwnp_solver\


CLEAN+=\
    bwdp-nopack/10bwnpdp_solver\
    bwdp-nopack/10bwnpdp_solver\
    bwdp-nopack/20bwnpdp_solver\
    bwdp-nopack/50bwnpdp_solver\
    bwdp-nopack/100bwnpdp_solver\
    bwdp-nopack/10bwnp_solver\
    bwdp-nopack/20bwnp_solver\
    bwdp-nopack/50bwnp_solver\
    bwdp-nopack/100bwnp_solver\

EVERYTHING+=bwdp-nopack

BWNPRC=\
	bwdp-nopack/blocksworld.cc\

CLEAN+=$(BWNPRC:.cc=.o)

bwdp-nopack/%bwnpdp_solver: bwdp-nopack/main.d
bwdp-nopack/%bwnpdp_solver:\
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

bwdp-nopack/%bwnp_solver: bwdp-nopack/main.d
bwdp-nopack/%bwnp_solver:\
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
