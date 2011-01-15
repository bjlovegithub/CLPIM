LIB = -L$(CLPIM_LIB) -lclpim
INCLUDE = -I$(CLPIM_COMMON) -I$(CLPIM_INCLUDE)

test: file_fetcher_test

file_fetcher_test: file_fetcher_test.cpp $(CLPIM_LIB)
	$(CC) $(INCLUDE) $(CXXFLAG) file_fetcher_test.cpp -o $@ $(LIB)

clean:
	rm -f file_fetcher_test