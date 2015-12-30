#BIN = partition
LIB = Util

CPPFLAGS += -fPIC -g3 
SOFLAGS  += $(CPPFLAGS) -fPIC -shared -Wl,-install_name,lib$(LIB).so

cpps :=  $(wildcard *.cpp)
objects := $(foreach file, $(cpps), $(basename $(file)).o)

ifdef BIN
all: $(BIN)
else
all: $(LIB)
endif

$(BIN): $(objects)
	$(CXX) $(CPPFLAGS) -o $(BIN) $(objects) -lpthread

$(LIB): $(objects)
	$(CXX) $(SOFLAGS) -o ../lib/lib$(LIB).so $(objects) -lpthread 
	$(AR)  rcs ../lib/lib$(LIB).a $(objects)

$(objects): %.o: %.cpp %.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

.PHONY: clean cleanbin realclean 
realclean: cleanbin clean

cleanbin:
	-rm -f $(BIN) $(LIB)
clean:
	-rm -f *.*~ $(objects) *.so
