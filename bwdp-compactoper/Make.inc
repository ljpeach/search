bwdp-compactoper:\
	bwdp-compactoper/10bwdp_solver\
    bwdp-compactoper/20bwdp_solver\
    bwdp-compactoper/50bwdp_solver\
    bwdp-compactoper/100bwdp_solver\
    bwdp-compactoper/10bw_solver\
    bwdp-compactoper/20bw_solver\
    bwdp-compactoper/50bw_solver\
    bwdp-compactoper/100bw_solver\


CLEAN+=\
    bwdp-compactoper/10bwdp_solver\
    bwdp-compactoper/10bwdp_solver\
    bwdp-compactoper/20bwdp_solver\
    bwdp-compactoper/50bwdp_solver\
    bwdp-compactoper/100bwdp_solver\
    bwdp-compactoper/10bw_solver\
    bwdp-compactoper/20bw_solver\
    bwdp-compactoper/50bw_solver\
    bwdp-compactoper/100bw_solver\

EVERYTHING+=bwdp-compactoper

BWRC=\
	bwdp-compactoper/blocksworld.cc\

CLEAN+=$(BWRC:.cc=.o)

bwdp-compactoper/%bwdp_solver: bwdp-compactoper/main.d
bwdp-compactoper/%bwdp_solver:\
	$(BWRC)\
	bwdp-compactoper/main.cc\
	utils/utils.a\
	structs/structs.a\
	search/search.a
	@echo $@
	@$(CXX) $(BWRC)\
		bwdp-compactoper/main.cc\
		utils/utils.a\
		structs/structs.a\
		search/search.a\
		$(LDFLAGS) -DNBLOCKS=$* -DDEEP -o $@

bwdp-compactoper/%bw_solver: bwdp-compactoper/main.d
bwdp-compactoper/%bw_solver:\
	$(BWRC)\
	bwdp-compactoper/main.cc\
	utils/utils.a\
	structs/structs.a\
	search/search.a
	@echo $@
	@$(CXX) $(BWRC)\
		bwdp-compactoper/main.cc\
		utils/utils.a\
		structs/structs.a\
		search/search.a\
		$(LDFLAGS) -DNBLOCKS=$* -o $@

include bwdp-compactoper/main.d
