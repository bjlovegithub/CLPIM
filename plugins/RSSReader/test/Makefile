LIBS = $(LIBS) -lxerces-c

All: xerces_test Console

xerces_test: xerces_example.cpp
	g++ $(INCLUDE) $(CXXFLAGS) $< -o $@ $(LIBS)

Console: console_test.cpp
	g++ console_test.cpp -lcurses -o $@