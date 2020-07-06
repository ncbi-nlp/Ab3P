NCBITEXTLIB=../../NCBITextLib
SRC_DIR=./
TRASHFILES = *.o *~ *.bak core *.a
LIB_INC=-I. -I$(NCBITEXTLIB)/include
#.KEEP_STATE:

OS=-g

libAb3P.a: MPtok.o AbbrStra.o AbbrvE.o Ab3P.o
	ar rus $@ $?

Ab3P.o: AbbrvE.h
AbbrvE.o: AbbrvE.h

%.o: $(SRC_DIR)/%.C
	g++ -c $(OS) $< -o $@ $(LIB_INC)

clean:
	rm -f $(TRASHFILES)
