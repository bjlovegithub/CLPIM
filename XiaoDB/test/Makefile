LIBS = $(LIB_PATH) -lXiaoDB -lclpim
LIB_FILES = $(wildcard *.cpp)
LIBOFILES = $(LIB_FILES:%.cpp=%.o)

ALL: BlockSizeReadTest BPlusTreeTest TreeNodeCacheManagerTest \
     KeyTest

BlockSizeReadTest: BlockSizeReadTest.cpp
	$(CC) $(INCLUDE) $(CXXFLAGS) $^ -o $@

BPlusTreeTest: BPlusTreeTest.cpp
	$(CC) $(INCLUDE) $(CXXFLAGS) $^ -o $@ $(LIBS)

TreeNodeCacheManagerTest: TreeNodeCacheManagerTest.cpp
	$(CC) $(INCLUDE) $(CXXFLAGS) $^ -o $@ $(LIBS)

KeyTest: KeyTest.cpp
	$(CC) $(INCLUDE) $(CXXFLAGS) $^ -o $@ $(LIBS)


.depend: $(LIB_FILES)
	@$(CC) -MM $(INCLUDE) $(LIB_FILES) > $@
sinclude .depend

clean:
	rm -f *.o
	rm -f BlockSizeReadTest BPlusTreeTest
	rm -f .depend
