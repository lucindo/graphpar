ifeq ($(findstring k,$(MAKEFLAGS)),k)
  KEEP_GOING = -
endif

TARGETS := all realclean cleanbin clean 
$(TARGETS):
	$(KEEP_GOING)@cd util/tinyxml && $(MAKE) $(@)
	$(KEEP_GOING)@cd util && $(MAKE) $(@)
	$(KEEP_GOING)@cd graph && $(MAKE) $(@)
	$(KEEP_GOING)@cd ptree && $(MAKE) $(@)
	$(KEEP_GOING)@cd gen && $(MAKE) $(@)
	$(KEEP_GOING)@cd approx && $(MAKE) $(@)
	$(KEEP_GOING)@cd heuristic && $(MAKE) $(@)
	$(KEEP_GOING)@cd treetest && $(MAKE) $(@)
	$(KEEP_GOING)@cd treepar && $(MAKE) $(@)
	$(KEEP_GOING)@cd heurtest && $(MAKE) $(@)
	$(KEEP_GOING)@cd approxtest && $(MAKE) $(@)
	$(KEEP_GOING)@cd ytest && $(MAKE) $(@)
	$(KEEP_GOING)@cd benchmark && $(MAKE) $(@)
	$(KEEP_GOING)@cd ip && $(MAKE) $(@)
	$(KEEP_GOING)@cd approxheuristcmp && $(MAKE) $(@)
	$(KEEP_GOING)@cd approxbench && $(MAKE) $(@)
	$(KEEP_GOING)@cd heurcmp && $(MAKE) $(@)
	$(KEEP_GOING)@cd graphpar && $(MAKE) $(@)

backup tar: realclean distclean
	tar zcvf backup_`date +"%s"`.tar.gz *

distclean:
	rm -rf lib/* bin/*
