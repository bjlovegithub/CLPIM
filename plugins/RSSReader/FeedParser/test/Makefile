COMMON_HEAD_FILE = ../../../../common/
XERCESC_HEAD_FILE = ../../
PARSER_HEAD_FILE = ../
INCLUDE = -I$(COMMON_HEAD_FILE) -I$(XERCESC_HEAD_FILE) -I$(PARSER_HEAD_FILE)

XERCESC_LIB = ../../lib/
COMMON_LIB = ../../../../lib/
LIBS = -L$(XERCESC_LIB) -L$(COMMON_LIB) -lxerces-c -lclpim

CFLAG = -g

All: rdf_test rdf_parser_ut atom_parser_ut rss_parser_ut

rdf_test: rdf_rss_test.cpp
	g++ $(INCLUDE) $(LIBS) $^ -o $@

rdf_parser_ut: rdf_parser_test.cpp ../feed_parser.cpp ../xercesc_factory.cpp ../rdf_parser.cpp
	g++ $(CFLAG) $(INCLUDE) $^ -o $@ $(LIBS)

atom_parser_ut: atom_parser_test.cpp ../feed_parser.cpp ../xercesc_factory.cpp ../atom_parser.cpp
	g++ $(CFLAG) $(INCLUDE) $^ -o $@ $(LIBS)

rss_parser_ut: rss_parser_test.cpp ../feed_parser.cpp ../xercesc_factory.cpp ../rss_parser.cpp
	g++ $(CFLAG) $(INCLUDE) $^ -o $@ $(LIBS)
