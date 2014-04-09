CC=g++
CPPFLAGS=-std=c++0x -c -pthread -DHAVE_CONFIG_H -Iinclude `wx-config --cppflags` -finput-charset=cp936 -fexec-charset=utf8
LDFLAGS=-pthread -lsnmp++ `pkg-config --libs xerces-c` `wx-config --libs` -ldes
SOURCES=$(wildcard src/*.cpp)
OBJS:=$(patsubst %.cpp,%.o,$(SOURCES))

MySnmp:$(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

-include $(SOURCES:.cpp=.d)

%.d:%.cpp
	set -e;rm -f $@;\
	echo '$(@F)' > $@.tmp;\
	tmpname=`cat $@.tmp`;\
	tmpname=$${tmpname%%.*};\
	$(CC) -MM $(CPPFLAGS) $< >$@.tmp;\
	sed "s,\($${tmpname}\).o:,$(@D)/\1.o $@:," < $@.tmp >$@;\
	rm -f $@.tmp;

.PHONY:clean
clean:
	rm -f *.o *.out *.d;cd src;rm -f *.o *.out *.d
