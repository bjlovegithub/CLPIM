LIB_FILES = $(wildcard *.cpp)
LIBOFILES = $(LIB_FILES:%.cpp=%.o)

libFeedParser.a: $(LIB_FILES)
	$(CC) $(CXXFLAGS_SHARED_LIB) $(INCLUDE) $(LIB_FILES)
	ar cr $@ $(LIBOFILES)
	cp $@ ../

.depend: $(LIB_FILES)
	@$(CC) $(INCLUDE) -MM $(LIB_FILES) > $@
sinclude .depend

clean:
	rm -f *.o

tests:
	$(MAKE) -C test