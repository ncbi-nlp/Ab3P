# Abbreviation Plus Pseudo-Precision (Ab3P) #

Ab3P is an abbreviation definition detector. A set of rules recognizes
simple patterns such as Alpha Beta (AB) as well as more involved
cases. The precision of each rule is estimated by applying to
randomized data (psuedo-precision). The algorithm is described in the paper:

Abbreviation definition identification based on automatic precision estimates.
Sohn S, Comeau DC, Kim W, Wilbur WJ.
BMC Bioinformatics. 2008 Sep 25;9:402.
PubMed ID: 18817555

The text of the paper can be read at
http://www.ncbi.nlm.nih.gov/pmc/articles/PMC2576267/

## Public Domain Notice ##

This work is a "United States Government Work" under the terms of the
United States Copyright Act. It was written as part of the authors'
official duties as a United States Government employee and thus cannot
be copyrighted within the United States. The data is freely available
to the public for use. The National Library of Medicine and the U.S.
Government have not placed any restriction on its use or reproduction.

Although all reasonable efforts have been taken to ensure the accuracy
and reliability of the data and its source code, the NLM and the
U.S. Government do not and cannot warrant the performance or results
that may be obtained by using it. The NLM and the U.S. Government
disclaim all warranties, express or implied, including warranties of
performance, merchantability or fitness for any particular purpose.

## Setup ##

This abbreviation identification package is provided as a C++ library.
The sample program can be used to identify abbreviations in plain
text.

1. Install the NCBITextLib library (<https://github.com/ncbi-nlp/NCBITextLib>)

2. Define the NCBITEXTLIB variable in the file Makefile, in the main
directory and the Library directory, to point to where NCBITextLib is located.

3. The command `make` builds the library, builds the program, and creates
the data files.

4.  The command `make test` runs the abbreviation
identification program on our gold standard and compares your
results to our results.  They should be identical.

### Setting up on macOS:

Some extra work is necessary to get Ab3P to work on macOS. This is because apple makes `g++` an alias for `clang++` and although `clang` is meant as a drop-in replacement for `gcc`, in this case the compilation fails (because of a problem in NCBITextLib). Even if the problem is resolved (see pull request on that library), the resulting executable does not work as intended.

Instead, instal gcc (with homebrew - https://brew.sh/, simply do `brew install gcc`) and then modify the makefiles, replacing "g++" by the path to the actual g++ binary. For example:

```makefile
%:%.o
	/usr/local/Cellar/gcc/8.3.0/bin/g++-8   $(OS) -o $@ $< -L$(LIBPATH) -lAb3P -L$(NCBITEXTLIB)/lib -lText
```

Compilation and execution should then work as expected.


## Usage ##

To identify SF-LF pairs from free text use the command  
  `./identify_abbr text_file_name`  
It processes the input one line at a time and prints out:

    input line
      sf|lf|P-precision

To run this program in a directory other than where it was originally
built, include a file called `path_Ab3P` in the directory that gives
the path to the WordData directory. Include the final slash (/).

The program `identify_abbr.C` also provides an example of using the Ab3P
library in a program.  Documentation on the routines can be found in
`Library/Ab3P.h`. 

Information Ab3P needs to work appears in the `WordData` directory. This
includes information about words and abbreviation strategies and their
estimated precision in `Ab3P_prec.dat`. Stop words are in the file
`stop`.  Long forms for 1-character short forms are in the file
`Lf1chSf`.  Defined words, for subword matching, are in
`SingTermFreq.dat`.

Before this information can be used by the program, it needs to be in
a form the program can efficiently access.  The command `make data`
creates these files.

We provide 1250 random MEDLINE records annotated manually
(`MED1250_labeled`) and the original unlabeled version
(`MED1250_unlabeled`). The format of the labeled data is:

    PubMed ID
    Title
    Abstract
      sf|lf (skip two space in the beginning)

Double slashes indicate comments. Certain comments have a particular
meaning as follows:

    //*  sf|lf
      skip two spaces after //*) for synonyms.   Synonyms are
      identified but not included in the gold standard.

    //!syn
      If there is no matching character for technical terms, including
      element symbols, for example MeHg|methyl mercury.

    //!out
      If the long form does not appear earlier in the same sentence.

    //!ord
      If the matching characters are not in the same order.  For example,
      Y73SV|Sarcoma Virus Y73

    //!num
      If a number matches a word.  For example, 2D|two dimension

    //!nch
      If we find a long form, but some characters do not match.  For
      example, Bet|Bel-1 plus Bel-2

    //!cnj
      Conjunction complicates long form determination.  For example, alpha
      beta (ABG) and coal delta (DDG) gamma.

    //
      any other comments (not starting with '!')

## Previous, non-GitHub versions ##

v1.5   Easier to run identify_abbr from a directory other than where built.

v1.1   API clarified

v1      Original public release
