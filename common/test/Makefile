LIBS += -liconv
SRC_FILES = FileUtilTest.cpp UnicodeTest.cpp HashTableTest.cpp

ALL: UnicodeTest FileUtilTest HashTableTest TimeFuncTest HashTablePerfTest \
	 UtilityTest

UnicodeTest: UnicodeTest.cpp
	$(CC) $(INCLUDE) $(CXXFLAGS) $^ -o $@ $(LIBS)

FileUtilTest: FileUtilTest.cpp
	$(CC) $(INCLUDE) $(CXXFLAGS) $^ -o $@ $(LIBS)

HashTableTest: HashTableTest.cpp
	$(CC) $(INCLUDE) $(CXXFLAGS) $^ -o $@ $(LIBS)

TimeFuncTest: TimeFuncTest.cpp
	$(CC) $(INCLUDE) $(CXXFLAGS) $^ -o $@ $(LIBS)

HashTablePerfTest: HashTablePerfTest.cpp
	$(CC) $(INCLUDE) $(CXXFLAGS) $^ -o $@ $(LIBS)

UtilityTest: UtilityTest.cpp
	$(CC) $(INCLUDE) $(CXXFLAGS) $^ -o $@ $(LIBS)

.depend: $(SRC_FILES)
	$(CC) -MM $(SRC_FILES) $(INCLUDE) > $@
sinclude .depend	

clean:
	rm -f UnicodeTest FileUtilTest HashTableTest TimerFuncTest
	rm -f .depend