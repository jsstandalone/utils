CXX = g++
CXXFLAGS = -O2 -g -fno-strict-aliasing -Wall -fPIC -std=c++0x
AR = ar rs

INC= -I.
LIB=

SRC_DIR = .
SRCS =	$(SRC_DIR)/circle_queue.cpp \

OBJ_DIR = .
OBJS =  $(OBJ_DIR)/circle_queue.o \

TARGET = libcircle_queue.a
all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $@ $?

$(OBJS): $(SRCS)
	$(CXX) -o $@ $(CXXFLAGS) -c $< $(INC)

clean:
	-rm -f $(OBJS) $(TARGET)