BIN = ytest
#LIB = GraphPartition

CPPFLAGS += -fPIC -DDEBUG -g3 -I../util -I../util/tinyxml -I../graph -I../gen -I../heuristic/approximation -I../ptree -L../lib 
LDFLAGS  += -lUtil -lGraph -lGraphGen -lGraphApproxHeuristics -lGraphApproxPartition -lTreePartition -lTinyXML
SOFLAGS  += $(CPPFLAGS) -fPIC -shared 

cpps :=  $(wildcard *.cpp)
objects := $(foreach file, $(cpps), $(basename $(file)).o)

ifdef BIN
all: $(BIN)
else
all: $(LIB)
endif

$(BIN): $(objects)
	$(CXX) $(CPPFLAGS) -o ../bin/$(BIN) $(objects) $(LDFLAGS)

$(LIB): $(objects)
	$(CXX) $(SOFLAGS) -o lib$(LIB).so $(objects) 

$(objects): %.o: %.cpp %.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

.PHONY: clean cleanbin realclean 
realclean: cleanbin clean

cleanbin:
	-rm -f $(BIN) $(LIB)
clean:
	-rm -f *.*~ $(objects) *.so
