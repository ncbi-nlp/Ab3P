#ifndef ABBRSTRA_H
#define ABBRSTRA_H

#include <vector>
#include <string>
#include <Hash.h>

using namespace std;
using namespace iret;


class WordData {
public:
  WordData(const char *wrdname="wrdset3", const char *stpname="stop", 
           const char *lfsname="Lf1chSf");
  
  ~WordData();

  Chash wrdset; //sigle word in MEDLINE 
  Hash stp; //stopword 
  Hash lfs; //lfs (1-ch sf) for FirstLet match cases >=2
};


class AbbrStra {
public:
  AbbrStra();
  ~AbbrStra();
  void token(const char *str, char lst[1000][1000]); // tokennize & set ntk 
  long tokenize(const char *str, char lst[1000][1000]); //tokennize & return # tokens
  long num_token(const char *str); //return # tokens
  long first_ch(const char *str, char *fch, long num);
  long is_upperal(const char *str);
  long is_alpha(const char *str);
  void str_tolower(const char *str1, char *str2);
  long get_str(const char *str1, char *str2, long num);
  bool isupper_str(const char *str);
  bool is_onealpha(const char *str);
  long count_upperstr(const char *str);
    //return # upper-case 1st letter of consecutive tokens (backward)
  void get_alpha(const char *str1, char *str2);
    //set str2 with only alphabet of str1
  bool lf_ok(const char *shrtf, const char *longf);
   
  virtual bool set_condition(const char *sf);
    //must set nonAlphaSF=true if want to use SF containing non-alphabet
  virtual long strategy(const char *sf, const char *str) = 0;
    //sf & str will be lower-cased (OCt-25-2007)
  long search_backward(long sloc, long tnum, long tloc, const char *sf, bool first); 
     //search backward to find match starting from sf[sloc] 
     //Returns 1 if matches. sf[0] must match with begin word
  long search_backward_adv(const char *sf, bool first); 
     //Searches for next model setting. Returns 1 if finds one.
  void extract_lf(long begin, long end); 
     //save strings from begin to end of tok to lf
  void extract_lf(long begin, long end, const char *str); 
     //save strings from begin to end of str's tok to lf

  //---after set mod check conditions
  //nsf:# ch in sf, nsw:# allowed skipword, general:true allow 1st ch match after non-alnum 
  bool exist_skipword(long nsf);
     //true if at least one skip word exists
  bool exist_n_skipwords(long nsf, long n);
     //true if exist n consecutive skip words between tokens but cannot be more than n
  bool exist_n_stopwords(long nsf, long n);
     //true if exist n consecutive skip stopwords between tokens but cannot be more than n
  bool stopword_ok(long nsf, long nsw);
     //true if at most (can be 0) nsw skip stopword in row exists
  bool skip_stop_ok(long nsf, long nsw, long n);
     //true if at most (can be 0) nsw skip word, which include at least n stopwords, in row exists
  bool skip_stop_ok2(long nsf, long nsw, long n);
     //true if nsw skip word, which include at least n stopwords, in row exists
  bool skipword_ok(long nsf, long nsw);
     //true if at most (can be 0) nsw skip word in row exists
  bool is_subword(long nsf);
     //true if matching string is begin of a tok or a word in wrdlist
  bool is_BeginWrdMatch(long nsf, bool general);
     //true if begining ch of a word match 
     //if general is true, allow match after non-alnum (eg, 1-alpha)
  bool is_WithinWrdMatch(long nsf, bool general);
     //true if within word match
     //if general is true, 1-Alpha: 'A' is not within word match
  bool is_FirstLetMatch(long nsf, bool general);
     //true if each ch of sf match with 1st ch of word
     //(true: Alpha anyword Beta (AB))
     //if general=true, true: 1-Alpha Beta, Alpha-Beta
  bool is_FirstLetMatch2(long nsf, bool general);
     //at least one 1-Alpha
  bool is_FirstLetSMatch(const char *sf, bool general);
     //true if first letter match & 's' match with last ch of lf
  bool is_ContLetMatch(long nsf);
     //true if two or more consecutive letter match
  //---

  char *pch; //sf applied to a strategy
  char *ps, *pl; //sf, potential lf
  char sf[100], text[10000]; //sf & potential lf used in a strategy
  char lf[10000]; //lf found by a strategy 
  char tok[1000][1000]; //token of potential lf 
                        //lower after strategy, original after extract_lf(b,e,str)
  long ntk; //# tokens
  int lf_begin_token;         // first token in LF
  long mod[100][2]; //match locations of tok with a given sf
     //mod[sf_inx][0]=tok inx, mod[sf_inx][1]=match loc in tok[mod[sf_inx][0]] 

  //for each n_ch-SF
  long npairs; //selected pairs for this strategy 
  long tpairs; //total pairs
  long nsfs; //# selected unique sfs for this strategy 
  long nmatchs; //# matchs (success strategy & given sf == real sf)
  long amatchs; //# accumulated matchs up to this strategy
  long setCondition;  //SF condition
  long greaterEqNsf; //if 1 select SF  |SF|>=nsf

  WordData *wData;
};


/*
alpha beta gamma (ABG)
 */
class FirstLet : public AbbrStra {
public:
  virtual bool set_condition(const char *str1, const char *str2, char *str);
  virtual long strategy(const char *sf, const char *str); 
};


class FirstLetOneChSF : public AbbrStra {
public:
  virtual bool set_condition(const char *str1, const char *str2, char *str);
  virtual long strategy(const char *sf, const char *str); 
};


/*
- sf ch matchs with 1st ch or ch right after non-alphanum of lf 
  but at least one match right after non-alphanum 
  (eg, success: 1-alpha 2-beta (AB), alpha-beta(AB), 
       fail: alpha beta(AB))
*/
class FirstLetGen : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str); 
};


/*
- sf ch matchs with 1st ch or ch right after non-alphanum of lf 
  (eg, success: 1-alpha 2-beta (AB), alpha-beta(AB), 
                alpha beta(AB))
*/
class FirstLetGen2 : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str); 
};


/*
For sf consisting of capital letters & lower-case 's'
- First letter & 's' in the last token of lf
(success: Alpha Betas (ABs), 1-Alpha Betas (ABs), 
          1-Alpha-Betas (ABs), Alpha BetaS (ABs)
 fail: Alpha Beta xxs (ABs) )
*/
class FirstLetGenS : public AbbrStra {
public:
  virtual bool set_condition(const char *sf); //sf must be an original sf
                                              //true if sf is like ABCs
  virtual long strategy(const char *sf, const char *str);
};


/*
- sf ch matches with 1st ch or ch right after non-alphanum of lf
- allowing one skip stopword between tokens (no more than one in row)
  at least one skip stopword in total 
  (eg, success: alpha and beta (AB), 1-alpha and beta (AB)
       fail: alpha beta (AB), alpha word beta (AB))
*/
class FirstLetGenStp : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str); 
};


/*
- same as FirstLetGenStp except for 2 skip stopwords
  & at least one two consecutive skip stopwords
 */
class FirstLetGenStp2 : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str); 
};


/*
- same as FirstLetGenStp except using skip any word instead of stopword
 */
class FirstLetGenSkp : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str); 
};


/*
- a matching sub-string must be word 
   (eg, success: AlphaBeta (AB), Beta is word 
                 x-AlphaBeta (AB) )
- at least one within word match
  (eg,fail: Alpha Beta Word (ABW), Alpha x-Beta x-Word (ABW) 
      success: AlphaBeta Word (ABW), x-AlphaBeta inWord (ABW))
 */
class WithinWrdWrd : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str);
};


/*
- WithinWrdWrd w/ Begin Word Match
  (success: AlphaBeta x-Word (ABW)
   fail: AlphaBeta inWord (ABW) )
 */
class WithinWrdFWrd : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str);
};


/*
- WithinWrdFWrd w/ allowing one skip word between tokens (no more than one in row)
  at least one skip word in total
  (success: AlphaBeta zzz x-Word zzz (ABW)
   fail: AlphaBeta x-Word (ABW), AlphaBeta zzz yyy x-Word (ABW))
*/
class WithinWrdFWrdSkp : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str);
};


/*
- at least one within word match
  ( success: Alpha InXyy (AX), x-Alpha InXyy (AX))
    fail: Alpha Xyy (AX), Alpha 1-Xyy (AX))
*/
class WithinWrdLet : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str);
};


/*
- WithinWrdLet w/ Begin Word Match
  (fail: Alpha InXyy (AX), x-Alpha InXyy (AX)
   success: AlphaXyy Word (AXW), x-AlphaXyy 1-Word (AXW))
*/
class WithinWrdFLet : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str);
};


/*
- WithinWrdFLet w/ allowing one skip word between tokens (no more than one in row)
  at least one skip word in total
  (success: AlphaXyy zzz Word zzz (AXW)
   fail: AlphaXyy Word (AXW), AlphaXyy zzz yyy Word (AXW))
*/
class WithinWrdFLetSkp : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str);
};


/* 
- any two consecutive letter matching  w/ begin word match  
eg) ABxxx (AB), 1-ABxxx (AB), ABxxx Cxxx (ABC), Axxx BCxxx (ABC)
    prolactin (PRL), succinylcholine (SCh)
*/
class ContLet : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str);
};


/*
- ContLet w/ allowing one skip word between tokens (no more than one in row) 
  at least one skip word in total 
*/
class ContLetSkp : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str);
};


/*
-  match can occur anywhere 
-  allow one skip word between tokens (no more than one in row)
   (success: Alpha yXyy (AX), Alpha yXyy word (AX)
             1-Alpha yXyy word (AX)) 
*/
class AnyLet : public AbbrStra {
public:
  virtual long strategy(const char *sf, const char *str);
};


class StratUtil {
public:
  AbbrStra *strat_factory(string name);
  vector<string> get_strats(string s);
    //get the strategy sequence for a given #-ch SF group
  void push_back_strat(string sgp, string strat);
  bool group_sf(const char *sf,  string &grp);
    //check if sf is ok and assign a group
  bool group_sf(const char *sf,  const char *lf, string &grp);
    //add the contion |lf|>|sf|
  void remove_nonAlnum(const char *str1, char *str2);
    //remove non-alnum in str1 and save it to str2
  long exist_upperal(const char *str); //return 1 if exists upper char, 0 ow
  long num_token(const char *str); //return # tokens

  vector<string> Al1, Al2, Al3, Al4, Al5;
  vector<string> Num2, Num3, Num4, Num5;
  vector<string> Spec2, Spec3, Spec4, Spec5;
};


#endif


