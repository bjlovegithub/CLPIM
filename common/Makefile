FLAGS = $(CXXFLAGS) -fPIC -c
LIB_FILES = $(wildcard *.cpp)
LIBOFILES = $(LIB_FILES:%.cpp=%.o)

ALL: libclpim_common.a libshared_ptr.a libnet.a
	cp libclpim_common.a $(CLPIM_LIB)

libshared_ptr.a:
	$(MAKE) -C shared_ptr

libnet.a:
	$(MAKE) -C net

libclpim_common.a: $(LIB_FILES)
	$(CC) $(FLAGS) $(LIB_FILES)
	ar cr $@ $(LIBOFILES)

.depend: $(LIB_FILES)
	@$(CC) -MM $(LIB_FILES) > $@
sinclude .depend

clean:
	rm -f *.o
	rm -f lib*.a
	$(MAKE) -C net clean
	$(MAKE) -C test clean

tests:
	$(MAKE) -C net tests
	$(MAKE) -C test