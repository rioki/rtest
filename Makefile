
PACKAGE     = rtest
VERSION     = 0.1.0
prefix     ?= /usr/local
CXX        ?= g++
CXXFLAGS   += -g -Wall -std=c++0x 
LDFLAGS    += 

headers     := rtest.h
srcs        := rtest.cpp
tests       := check.cpp
extra_dist  := Makefile README.md

ifeq ($(OS),Windows_NT)
  EXEEXT=.exe
else
  EXEEXT=
endif

.PHONY: all clean install uninstall

all: librtest.a

librtest.a: rtest.o
	ar rcs $@ $^
	
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -MD -c $< -o $@

check: rtest-check
	./rtest-check
	
rtest-check: check.o librtest.a	
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@
	
	
clean:
	rm -rf librtest.a rtest-check$(EXEEXT) *.o *.d 
	
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
