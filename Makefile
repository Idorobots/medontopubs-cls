CC=g++
LDLIBS=
CXXFLAGS=-Wall -Wextra -pedantic
GDB=-ggdb3
VPATH=src
AR=ar
RM=rm -f

LIB=libcls.a
LIBOBJS=util.o classifier.o

TARGET=cls
OBJS=main.o $(LIBOBJS)

all: $(LIB) $(TARGET)

$(LIB) : $(LIBOBJS)
	$(AR) rcu $@ $<

$(TARGET) : $(OBJS)
	$(CC) $^ $(LDLIBS) -o $@

%.o : %.cpp
	$(CC) $(CXXFLAGS) $(GDB) $< -c

.PHONY: clean

clean:
	$(RM) $(LIB)
	$(RM) $(LIBOBJS)
	$(RM) $(TARGET)
	$(RM) $(OBJS)
