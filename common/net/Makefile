LIB_FILES = $(wildcard *.cpp)
LIB_FILES_OBJ = $(LIB_FILES:%.cpp=%.o)
INCLUDE = -I$(CLPIM_COMMON) -I$(CLPIM_INCLUDE)
CXXFLAGS += $(INCLUDE)

libnet.a: $(LIB_FILES) test
	$(CC) $(INCLUDE) $(CXXFLAGS_SHARED_LIB) $(LIB_FILES)
	ar cr $@ $(LIB_FILES_OBJ)
	cp $@ $(CLPIM_LIB)

.depend: $(LIB_FILES)
	@$(CC) $(INCLUDE) -MM $(LIB_FILES) > $@
sinclude .depend

clean:
	rm -f *.o .depend lib*.a

tests:
	$(MAKE) -C test