CXX = g++
CXXFLAGS = -O2 -g -fno-strict-aliasing -Wall -march=x86-64

INC= -I./
LIB= -lpthread

SRC_DIR = .
SRCS =	$(SRC_DIR)/circle_queue.cpp \
		$(SRC_DIR)/parallel_client.cpp \
		$(SRC_DIR)/parallel_api_multi_thread.cpp \
		$(SRC_DIR)/main.cpp \

OBJ_DIR = .
OBJS =  $(OBJ_DIR)/circle_queue.o \
		$(OBJ_DIR)/parallel_client.o \
		$(OBJ_DIR)/parallel_api_multi_thread.o \
		$(OBJ_DIR)/main.o \
		
TARGET = mptest

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $? $(LIB)
	
OBJS: $(SRCS)
	$(CXX) $(CXXFLAGS) $(INC) -c $? -o $@

clean:
	-rm -f $(OBJS) $(TARGET)
