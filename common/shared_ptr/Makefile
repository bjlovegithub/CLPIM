shared_counter.o : shared_counter.cpp
	$(CC) -fPIC -c $(CXXFLAGS) $^ -o $@
	mv $@ $(CLPIM_LIB)

test:
	$(CC) -C test