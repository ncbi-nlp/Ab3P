NCBITEXTLIB=../NCBITextLib
LIBPATH=./lib
OS=-g
# OS=-O


.KEEP_STATE:

%:%.o
	g++ $(OS) -o $@ $< -L$(LIBPATH) -lAb3P -L$(NCBITEXTLIB)/lib -lText

%.o: %.C
	g++ -c -fpic $(OS) -I$(LIBPATH) -I$(NCBITEXTLIB)/include $< -o $@

all:
	make programs
	make data

programs:
	make library
	make make_wordSet
	make make_wordCountHash
	make identify_abbr
	make wrapper.o

library:
	cd lib; make

data:
	./make_wordSet WordData/stop stop
	./make_wordSet WordData/Lf1chSf Lf1chSf
	./make_wordCountHash WordData/SingTermFreq.dat


test:
	./identify_abbr MED1250_unlabeled | diff identify_abbr-out -

test2:
	./identify_abbr test2.in | diff test2.out -

clean:
	cd lib; make clean
	rm -f WordData/cshset_* WordData/hshset_*
	rm -f identify_abbr make_wordCountHash make_wordSet *.o *~

make_wordSet:		make_wordSet.o
make_wordCountHash:	make_wordCountHash.o
identify_abbr:		identify_abbr.o lib/libAb3P.a
identify_abbr.o:	lib/Ab3P.h
identify_abbr_loc:	identify_abbr.o lib/libAb3P.a
identify_abbr_loc.o:	lib/Ab3P.h
wrapper.o:	lib/Ab3P.h
