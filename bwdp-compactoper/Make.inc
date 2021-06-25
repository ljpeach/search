bwdp-compactoper:\
    bwdp-compactoper/10bwco_solver\
    bwdp-compactoper/20bwco_solver\
    bwdp-compactoper/50bwco_solver\
    bwdp-compactoper/100bwco_solver\
    bwdp-compactoper/10bwcodp_solver\
    bwdp-compactoper/20bwcodp_solver\
    bwdp-compactoper/50bwcodp_solver\
    bwdp-compactoper/100bwcodp_solver\


CLEAN+=\
    bwdp-compactoper/10bwco_solver\
    bwdp-compactoper/20bwco_solver\
    bwdp-compactoper/50bwco_solver\
    bwdp-compactoper/100bwco_solver\
    bwdp-compactoper/10bwcodp_solver\
    bwdp-compactoper/20bwcodp_solver\
    bwdp-compactoper/50bwcodp_solver\
    bwdp-compactoper/100bwcodp_solver\

EVERYTHING+=bwdp-compactoper

BWCORC=\
	bwdp-compactoper/blocksworld.cc\

CLEAN+=$(BWCORC:.cc=.o)

bwdp-compactoper/%bwcodp_solver: bwdp-compactoper/main.d
bwdp-compactoper/%bwcodp_solver:\
	$(BWCORC)\
	bwdp-compactoper/main.cc\
	utils/utils.a\
	structs/structs.a\
	search/search.a
	@echo $@
	@$(CXX) $(BWCORC)\
		bwdp-compactoper/main.cc\
		utils/utils.a\
		structs/structs.a\
		search/search.a\
		$(LDFLAGS) -DNBLOCKS=$* -DDEEP -o $@

bwdp-compactoper/%bwco_solver: bwdp-compactoper/main.d
bwdp-compactoper/%bwco_solver:\
	$(BWCORC)\
	bwdp-compactoper/main.cc\
	utils/utils.a\
	structs/structs.a\
	search/search.a
	@echo $@
	@$(CXX) $(BWCORC)\
		bwdp-compactoper/main.cc\
		utils/utils.a\
		structs/structs.a\
		search/search.a\
		$(LDFLAGS) -DNBLOCKS=$* -o $@

include bwdp-compactoper/main.d
