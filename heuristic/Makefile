ifeq ($(findstring k,$(MAKEFLAGS)),k)
  KEEP_GOING = -
endif

TARGETS := all realclean cleanbin clean 
$(TARGETS):
	$(KEEP_GOING)@cd approximation && $(MAKE) $(@)
	$(KEEP_GOING)@cd spanningtrees && $(MAKE) $(@)

