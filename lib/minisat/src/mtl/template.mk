##
##  Template makefile for Standard, Profile, Debug, Release, and Release-static versions
##
##    eg: "make rs" for a statically linked release version.
##        "make d"  for a debug version (no optimizations).
##        "make"    for the standard version (optimized, but with debug information and assertions active)

MROOT     := ../..

PWD        = $(shell pwd)
EXEC      := $(MROOT)/bin/minisat
LIB       := $(MROOT)/bin/libminisat

CSRCS      = $(wildcard $(PWD)/*.cc) 
DSRCS      = $(foreach dir, $(DEPDIR), $(filter-out $(MROOT)/$(dir)/Main.cc, $(wildcard $(MROOT)/$(dir)/*.cc)))
CHDRS      = $(wildcard $(PWD)/*.h)
COBJS      = $(CSRCS:.cc=.o) $(DSRCS:.cc=.o)

PCOBJS     = $(addsuffix p,  $(COBJS))
DCOBJS     = $(addsuffix d,  $(COBJS))
RCOBJS     = $(addsuffix r,  $(COBJS))


CXX       ?= g++
CFLAGS    ?= -Wall -Wno-parentheses
LFLAGS    ?= -Wall

COPTIMIZE ?= -O3

CFLAGS    += -I../../.. -D __STDC_LIMIT_MACROS -D __STDC_FORMAT_MACROS
LFLAGS    += -lz

.PHONY : s p d r rs clean 

s:	$(EXEC)
p:	$(EXEC)_profile
d:	$(EXEC)_dbg
r:	$(EXEC)_rel
rs:	$(EXEC)_static

libs:	$(LIB)_standard.a
libp:	$(LIB)_profile.a
libd:	$(LIB)_dbg.a
libr:	$(LIB)_rel.a

## Compile options
%.o:			CFLAGS +=$(COPTIMIZE) -g -D DEBUG
%.op:			CFLAGS +=$(COPTIMIZE) -pg -g -D NDEBUG
%.od:			CFLAGS +=-O0 -g -D DEBUG
%.or:			CFLAGS +=$(COPTIMIZE) -g -D NDEBUG

## Link options
$(EXEC):		LFLAGS += -g
$(EXEC)_profile:	LFLAGS += -g -pg
$(EXEC)_dbg:		LFLAGS += -g
#$(EXEC)_rel:		LFLAGS += ...
$(EXEC)_static:		LFLAGS += --static

## Dependencies
$(EXEC):		$(COBJS)
$(EXEC)_profile:	$(PCOBJS)
$(EXEC)_dbg:		$(DCOBJS)
$(EXEC)_rel:		$(RCOBJS)
$(EXEC)_static:		$(RCOBJS)

$(LIB)_standard.a:	$(filter-out */Main.o,  $(COBJS))
$(LIB)_profile.a:	$(filter-out */Main.op, $(PCOBJS))
$(LIB)_dbg.a:		$(filter-out */Main.od, $(DCOBJS))
$(LIB)_rel.a:		$(filter-out */Main.or, $(RCOBJS))


## Build rule
%.o %.op %.od %.or:	%.cc
	@echo Compiling: $(subst $(MROOT)/,,$@)
	@$(CXX) $(CFLAGS) -c -o $@ $<

## Linking rules (standard/profile/debug/release)
$(EXEC) $(EXEC)_profile $(EXEC)_dbg $(EXEC)_rel $(EXEC)_static:
	@echo Linking: "$@ ( $(foreach f,$^,$(subst $(MROOT)/,,$f)) )"
	@$(CXX) $^ $(LFLAGS) -o $@

## Library rules (standard/profile/debug/release)
$(LIB)_standard.a $(LIB)_profile.a $(LIB)_rel.a $(LIB)_dbg.a:
	@echo Making library: "$@ ( $(foreach f,$^,$(subst $(MROOT)/,,$f)) )"
	@$(AR) -rcsv $@ $^

## Library Soft Link rule:
#libs libp libd libr:
#	@echo "Making Soft Link: $^ -> $(LIB).a"
#	@ln -sf $^ $(LIB).a

## Clean rule
clean:
	@rm -f $(EXEC) $(EXEC)_profile $(EXEC)_dbg $(EXEC)_rel $(EXEC)_static \
	  $(COBJS) $(PCOBJS) $(DCOBJS) $(RCOBJS) *.core depend.mk 

## Make dependencies
depend.mk: $(CSRCS) $(CHDRS)
	@echo Making dependencies
	@$(CXX) $(CFLAGS) -I$(MROOT) \
	   $(CSRCS) -MM | sed 's|\(.*\):|$(PWD)/\1 $(PWD)/\1r $(PWD)/\1d $(PWD)/\1p:|' > depend.mk
	@for dir in $(DEPDIR); do \
	      if [ -r $(MROOT)/$${dir}/depend.mk ]; then \
		  echo Depends on: $${dir}; \
		  cat $(MROOT)/$${dir}/depend.mk >> depend.mk; \
	      fi; \
	  done

-include $(MROOT)/mtl/config.mk
-include depend.mk
