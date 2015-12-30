#BIN = htest
LIB = GraphPartition

CPPFLAGS += -fPIC -DDEBUG -g3 -I../../util -I../../ptree -I../../graph -I../../gen -L../../lib 
LDFLAGS  += -lUtil -lTreePartition -lGraph -lGraphGen
SOFLAGS  += $(CPPFLAGS) -fPIC -shared 

cpps :=  $(wildcard *.cpp)
objects := $(foreach file, $(cpps), $(basename $(file)).o)

ifdef BIN
all: $(BIN)
else
all: $(LIB)
endif

$(BIN): $(objects)
	$(CXX) $(CPPFLAGS) -o ../../bin/$(BIN) $(objects) $(LDFLAGS)

$(LIB): $(objects)
	$(CXX) $(SOFLAGS) -o ../../lib/lib$(LIB).so $(objects) $(LDFLAGS)
	$(AR) rcs ../../lib/lib$(LIB).a $(objects) 

$(objects): %.o: %.cpp %.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

.PHONY: clean cleanbin realclean 
realclean: cleanbin clean

cleanbin:
	-rm -f $(BIN) $(LIB)
clean:
	-rm -f *.*~ $(objects) *.so
