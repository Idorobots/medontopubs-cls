CC=g++
LDLIBS= -lcls
CXXFLAGS=-Wall -Wextra -pedantic
GDB=-ggdb3
VPATH=src
AR=ar
RM=rm -f

LIBOBJS=classifier.o
OBJS=main.o

TARGET=cls
LIB=libcls

all: $(LIB) $(TARGET)

$(LIB) : $(LIBOBJS)
	$(AR) rcu $@.a $<

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
