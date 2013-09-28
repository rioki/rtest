
PACKAGE     = rtest
VERSION     = 0.1.0
prefix     ?= /usr/local
CXX        ?= g++
CXXFLAGS   += -g -Wall -std=c++0x 
LDFLAGS    += 

headers     := $(wildcard *.h)
srcs        := $(wildcard *.cpp)
extra_dist  := Makefile README.md

.PHONY: all clean install uninstall

all: librtest.a

librtest.a: rtest.o
	ar rcs $@ $^
	
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -MD -c $< -o $@

clean:
	rm -rf *.a *.o
	
install: librtest.a
	mkdir -p $(prefix)/include
	cp rtest.h $(prefix)/include
	mkdir -p $(prefix)/lib
	cp librtest.a $(prefix)/lib	

uninstall:
	rm $(prefix)/include/rtest.h
	rm $(prefix)/lib/librtest.a

dist:
	mkdir $(PACKAGE)-$(VERSION)
	cp $(headers) $(PACKAGE)-$(VERSION)
	cp $(srcs) $(PACKAGE)-$(VERSION)
	cp $(extra_dist) $(PACKAGE)-$(VERSION)
	tar -czvf $(PACKAGE)-$(VERSION).tar.gz $(PACKAGE)-$(VERSION)
	rm -rf $(PACKAGE)-$(VERSION)
	
	
	
ifneq "$(MAKECMDGOALS)" "clean"
-include $(patsubst %.cpp, obj/%.d, $(srcs))
endif
