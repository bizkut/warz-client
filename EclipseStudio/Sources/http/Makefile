
CFLAGS=-g
LIBS=-lstdc++
PROG=$(shell basename `pwd`)
SOURCES=$(wildcard *.cpp)
OBJECTS=$(patsubst %.cpp,obj/%.o,$(SOURCES))
DEPS=$(patsubst %.o,%.d,$(OBJECTS))

$(PROG):	obj $(OBJECTS)
	cc -g -o $@ $(OBJECTS) $(LIBS)

clean:
	rm -rf obj $(PROG)

obj:
	mkdir -p obj

obj/%.o:	%.cpp
	cc -c -o $@ $< -MMD $(CFLAGS)

-include $(DEPS)

