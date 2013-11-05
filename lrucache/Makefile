#include	./makefile.ctlvsvr

# my inc/lib

INC1 =-I./  $(INC) 
LIB1 = -L./ $(LIB) 

OBJ= tool_test_lrucache.o lru_cache.o
BUILD= tool_test_lrucache

$(BUILD):$(OBJ)
	@echo 
	@echo "Compiling $< ==> $@..."
	$(CXX) $(INC1) $(C_FLAGS)  -Wall -g -o $(BUILD) $(OBJ)   $(LIB1)


%.o:%.cpp 
	@echo 
	@echo "Compiling $< ==> $@..."
	$(CXX) -c $(INC1) $(C_FLAGS) -g -Wall  $< -o $@ 

check:
	$(PATH_QZONEPLATFORM_PRJ)/makeinclude/make_check
	

all: $(BUILD) 

clean:
	rm -f $(BUILD) $(OBJ) 
