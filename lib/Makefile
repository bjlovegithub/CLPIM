CLPIM_LIB_OJB = shared_counter.o
CLPIM_COMMON_LIB = libclpim_common.a
CLPIM_NET_LIB =  libnet.a

libclpim.a: $(CLPIM_LIB_SRC) $(CLPIM_COMMON_LIB)
	ar x $(CLPIM_COMMON_LIB)
	ar x $(CLPIM_NET_LIB)
	ar cr $@ *.o

clean:
	rm -f libclpim.a $(CLPIM_LIB_SRC)
	rm -f *.o
	rm -f *.a
	rm -f *.so