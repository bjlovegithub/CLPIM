LIBS += -L$(CLPIM_LIB) -lclpim -ldl
INCLUDE += -I$(CLPIM_COMMON) -I$(CLPIM_INCLUDE)
CXXFLAGS += $(INCLUDE)

clpim: CLPIM.o
	$(CC) $(CXXFLAGS) $(INCLUDE) CLPIM.o -o $@ $(LIBS)

.depend: CLPIM.cpp
	@$(CC) $(INCLUDE) -MM CLPIM.cpp > $@

sinclude .depend

clean:
	rm -f *.o clpim .depend