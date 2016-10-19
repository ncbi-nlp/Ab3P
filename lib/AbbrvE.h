#ifndef ABBRVE_H
#define ABBRVE_H
#include <fstream>
#include <iostream>
#include <runn.h>
#include <MPtok.h>
#include <vector>
using namespace std;
namespace iret {

  typedef vector<string> strings;


/***
    Like a character pointer, except it also tracks an offset
*/

class offset_char_p {
public:
 offset_char_p(void) : text(""), offset(0) { }
  /*
  ~offset_char_p() {
    if ( text ) {
      //      cout << (void*) text << ' ' << text << '\n';
      delete [] text;
    }
  }
  */
  /*
  offset_char_p( const offset_char_p & in ) {
    text = 0;
    if ( in.text ) {
      set_text( in.text, strlen(in.text) );
    }
    offset = in.offset;
  }
  */

  void set_text( char * in_text, int in_len ) {

    text = string( in_text, in_len );
  }

  /*
  void set_text( char * in_text, int in_len ) {
    delete [] text;             // if something already present
    text = new char[in_len+1];
    std::copy( in_text, in_text + in_len, text  );
    text[in_len] = '\0';
  }
  */
  operator const char*() const { return text.c_str(); }

  int offset;
  // private: 
  //  char * text;
  string text;
};


//** potential abbreviation

 class Pot_Abbr {
 public:
 Pot_Abbr()
   : abbs(0), abbl(0), nt(0) {}
   char * abbs;
   char * abbl;
   int nt;
   vector<offset_char_p> abbs_tokens;
   vector<offset_char_p> abbl_tokens;
 };


class Find_Seq {
public:

  Find_Seq( void  );

  // flag the SFs whether part of sequence or not
  void flag_seq( int numa, char* abbs[] );
  void flag_seq( const vector<Pot_Abbr> & abb );

  // true if good SF, false if part of sequence
  bool rate( int i ) const { my_rate[i]; }

private:
  void find_seq( const vector<string> & seq );
  void create_seq( void );

  // const works with c++0x
  /* const */ strings seq_i;
  /* const */ strings seq_I;
  /* const */ strings seq_a;
  /* const */ strings seq_A;
  
  vector<bool> my_rate;
  int my_numa;
  char ** my_abbs;              // really char *[], but that doesn't work
  
};





class AbbrvE {
   public:
      AbbrvE(long ta=10000 ); //Sets space for extracted
         //potential abbreviations to ta
     ~AbbrvE(void);
     //      void Extract(char *pch); //Extracts possible long-short form
         //pairs, but does not attempt to find the relationship
     void Extract2(const char *pch,
                   int sentence_offset); //extened version (Jan-9-2008)
      void Extract2_ch(const char *pch,
                       const char *openCh, const char *closeCh );
      // helper for Extract2
      bool Test(const char *str); //Tests a single token and returns true
         //if the token should be a possible first token of a short form
      void Rate(void); //Sets ratings for the proposed pairs. Effort to 
         //remove (a), (b), etc., sequence markers
      void token(const char *str); //Produces a list of tokens in order of
         //of occurrence in the string.
      void token2(const char *str, int sentence_offset);
      //extended version (Jan-9-2008)
      void cleara(void); //Clear the abbl & abbs memory of strings

      //Application functions
      void Proc(char *pch); //Accepts a natural language statement and
         //processes to final results stored in tta, abbs, and abbl
         //Need to call cleara function after each use of this function

      // find the offsets for each sentence in the text
        void find_sentence_offsets( const char * text,
                                    vector<int> & offsets );

      // Internal routines:
      // setup data for Test method
      void setup_Test( void );
      bool prefix_match( const char *str ); // does str begins with a prefix?

      //Data
      long tta; //Total possible abbreviations extracted
         //default 10k
      //      long numa; //number of abbreviations in current extract
      //      char **abbl; //Long form space, hold up to 10 tokens
      //      char **abbs; //Short form space, hold up to 10 tokens
      Find_Seq seq;             // identify sequences to ignore
      //      int  *nt; //Number of tokens within parentheses
      vector<Pot_Abbr> abb;
      vector<offset_char_p> lst; //Holds the tokens

      static const int cnam_size=100000;
      char cnam[cnam_size]; //Work space
      int cnam_offset[cnam_size]; // where cnam character came from
      MPtok *pMt; //Pointer at tokenizer class. Used to segment text
         //in Proc function.

      // Test data
      set<string> match;        // bad SF to match exactly
      vector<string> prefix;    // bad SF to match prefix
};
}
#endif
