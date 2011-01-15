INCLUDE = -I$(CLPIM_COMMON) -I$(CLPIM_INCLUDE)
LIB = $(CLPIM_LIB) -lclpim -lpthread 

test: Test

Test: shared_ptr_test.cpp
	$(CC) $(INCLUDE) $(CXXFLAGS) shared_ptr_test.cpp -o $@ $(LIB)

Boost_Test: boost_shared_ptr_test.cpp
	g++ -I/home/xianggen.wang/Apsara/include/ext $(INCLUDE) -lpthread -Wall boost_shared_ptr_test.cpp -o $@