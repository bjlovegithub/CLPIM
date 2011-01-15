PWD = $(shell pwd)
XERCESC_INCLUDE = $(PWD)
INCLUDE = -I$(CLPIM_INCLUDE) -I$(CLPIM_COMMON) -I$(XERCESC_INCLUDE)
XERCESC_LIB = $(PWD)/lib
LIBS += -L$(XERCESC_LIB) -L$(CLPIM_LIB) -lxerces-c -lcurses -lclpim

LIB_FILES = $(wildcard *.cpp)
LIBOFILES = $(LIB_FILES:%.cpp=%.o)

ALL: libRSSReader.so

libRSSReader.so: $(LIB_FILES) libFeedParser.a
	$(CC) $(CXXFLAGS_SHARED_LIB) $(INCLUDE) $(LIB_FILES)
	$(CC) -shared -o $@ *.o $(LIBS) -lFeedParser
	cp $@ $(PLUGIN_DIR)

libFeedParser.a:
	$(MAKE) -C $(PWD)/FeedParser

.depend: $(LIB_FILES)
	@$(CC) $(INCLUDE) -MM $(LIB_FILES) > $@
sinclude .depend

tests:
	$(MAKE) -C test

clean:
	rm -f *.o