CC=g++
CPPFLAGS=-std=c++0x -c -pthread -DHAVE_CONFIG_H -Iinclude `wx-config --cppflags` -finput-charset=cp936 -fexec-charset=utf8
LDFLAGS=-pthread -lsnmp++ `pkg-config --libs xerces-c` `wx-config --libs` -ldes
SOURCES=$(wildcard src/*.cpp)
OBJS:=$(patsubst %.cpp,%.o,$(SOURCES))

MySnmp:$(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@.out
	cd src;rm -f *.d;

-include $(SOURCES:.cpp=.d)

%.d:%.cpp
	set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< >$@.$$$$;\
	sed 's,\($*\).o[ :]*,\1.o $@:,' < $@.$$$$ >$@;\
	rm -f $@.$$$$

.PHONY:clean
clean:
	rm -f *.o *.d *.out;cd src;rm -f *.o *.d *.out
