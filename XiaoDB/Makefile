#In BPlusTree, it needs version info. All version info is putted here.
VERSION_INFO = -DBTREE_VERSION=\"0.0.1\"
CXXFLAGS += $(VERSION_INFO)
CXXFLAGS_SHARED_LIB += $(VERSION_INFO)
LIB_FILES = $(wildcard *.cpp)
LIBOFILES = $(LIB_FILES:%.cpp=%.o)

ALL: libXiaoDB.a
	cp libXiaoDB.a $(CLPIM_LIB)

libXiaoDB.a: $(LIB_FILES)
	$(CC) $(CXXFLAGS_SHARED_LIB) $(INCLUDE) $(LIB_FILES)
	ar cr $@ $(LIBOFILES)

.depend: $(LIB_FILES)
	$(CC) -MM $(INCLUDE) $(LIB_FILES) > $@
sinclude .depend

clean:
	rm -f *.o
	rm -f lib*.a
	rm -f .depend
	$(MAKE) -C test clean

tests:
	$(MAKE) -C test
