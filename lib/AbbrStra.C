#include "AbbrStra.h"
#include <runn.h>
#include <vector>
#include <fstream>
#include <iostream>


WordData::WordData(const char *wrdnam, const char *stpnam, 
                   const char *lfsnam) :
  wrdset(wrdnam), stp(stpnam), lfs(lfsnam)
{
  wrdset.set_path_name("Ab3P");
  wrdset.gopen_ctable_map();
  stp.set_path_name("Ab3P");
  stp.gopen_htable_map();
  lfs.set_path_name("Ab3P");
  lfs.gopen_htable_map();
}

WordData::~WordData()
{
  wrdset.gclose_ctable_map();
  stp.gclose_htable_map();
  lfs.gclose_htable_map();
}


AbbrStra::AbbrStra()
{
  npairs = tpairs = nsfs = nmatchs = amatchs = 0;
}


AbbrStra::~AbbrStra()
{
}


void AbbrStra::token(const char *str, char lst[1000][1000])
{
   long i,j=0,k=0;
   long n=strlen(str)-1;

   while(isblank(str[n])) n--; 

   while(str[j]){
      while(isblank(str[j]))j++;
      i=j;
      while((str[j])&&(!isblank(str[j])))j++;
      strncpy(lst[k],str+i,j-i);
      lst[k][j-i]='\0';
      if(str[j]){
	 k++; 
         j++;
      }
   }
   if((j-1)>n) k--; //added by Sohn (Jan-17-08): "ab cd " -> 2 tokens
   ntk=k+1; //# tokens, ntk is data member
}


long AbbrStra::tokenize(const char *str, char lst[1000][1000])
{
   long i,j=0,k=0;
   long n=strlen(str)-1;

   while(isblank(str[n])) n--; 

   while(str[j]){
      while(isblank(str[j]))j++;
      i=j;
      while((str[j])&&(!isblank(str[j])))j++;
      strncpy(lst[k],str+i,j-i);
      lst[k][j-i]='\0';
      if(str[j]){
         k++; 
         j++;
      }
   }
   if((j-1)>n) k--; //added by Sohn (Jan-17-08): "ab cd " -> 2 tokens
   return k+1; //# tokens
}


long AbbrStra::num_token(const char *str)
{
   long i,j=0,k=0;
   long n=strlen(str)-1;

   while(isblank(str[n])) n--; 

   while(str[j]){
      while(isblank(str[j]))j++;
      i=j;
      while((str[j])&&(!isblank(str[j])))j++;
      if(str[j]){
	 k++;
         j++;
      }
   }
   if((j-1)>n) k--; //added by Sohn (Jan-17-08): "ab cd " -> 2 tokens
   return k+1; //# tokens
}


// fch is 1st char of str token from backward
long AbbrStra::first_ch(const char *str, char *fch, long num)
{
  long i, j, numtk;
  char tk[1000][1000];

  numtk = tokenize(str,tk);
  if(num>numtk) return 0;

  for(i=0; i<num; i++) 
    fch[i] = tk[numtk-num+i][0];
  
  return 1;
}

long AbbrStra::is_upperal(const char *str)
{
  for(long i=strlen(str)-1; i>=0; i--)
    if(!isupper(str[i]) || !isalpha(str[i]))
      return 0;
  return 1;
}

long AbbrStra::is_alpha(const char *str)
{
  for(long i=strlen(str)-1; i>=0; i--)
    if(!isalpha(str[i]))
      return 0;
  return 1;
}


// str2 will lower-case of str1
void AbbrStra::str_tolower(const char *str1, char *str2)
{
  long i=0;

  while(str1[i]) {
    str2[i] = tolower(str1[i]);
    i++;
  }
  str2[i] = '\0';
}

//copy num tokens from back of str1 to str2 
long AbbrStra::get_str(const char *str1, char *str2, long num)
{
  char ch, tk[1000][1000];
  long i, j, numtk;

  if(num<0) { cout<<"num<0\n"; exit(1); }
  numtk = tokenize(str1,tk);
  if(numtk<num) return 0;

  strcpy(str2,tk[numtk-num]);
  for(i=1; i<num; i++) {
    strcat(str2," ");
    strcat(str2,tk[numtk-num+i]);
  }

  return 1;
}

bool AbbrStra::isupper_str(const char *str)
{
  long i, len=strlen(str);

  for(i=0; i<len; i++) 
    if(isalpha(str[i]) && !isupper(str[i])) 
      return false;

  return true;
}

bool AbbrStra::is_onealpha(const char *str)
{
  long i, j=0, len=strlen(str);

  for(i=0; i<len; i++)
    if(isalpha(str[i])) j++;

  if(j==1) return true;
  else return false;
}

long AbbrStra::count_upperstr(const char *str)
{
  long i, j, k, numtk;
  char tk[1000][1000];

  numtk = tokenize(str,tk);

  j = 0;
  for(i=numtk-1; i>=0; i--) {
    if(isupper(tk[i][0])) j++;
    else return j;
  }

  return j;
}

void AbbrStra::get_alpha(const char *str1, char *str2)
{
  long i = 0, j = 0;
  long len = strlen(str1);

  while(i<len) {
    if(isalpha(str1[i])) {
      str2[j] = str1[i];
      j++;
    }
    i++;
  } 
  str2[j] = '\0';
}


bool AbbrStra::lf_ok(const char *shrtf, const char *longf)
{
  long i;
  long paren=0, sbrac=0;
  string s, l;

  //false for one parenthesis or square bracket
  for(i=strlen(longf)-1; i>=0; i--) { 
    if(longf[i]=='(') paren++;
    if(longf[i]==')') paren--;
    if(longf[i]=='[') sbrac++;
    if(longf[i]==']') sbrac--;
  }
  if(paren!=0 || sbrac!=0) return false;

  s.assign(shrtf);
  l.assign(longf);

  for(i=0; i<s.length(); i++) s[i]=tolower(s[i]);
  for(i=0; i<l.length(); i++) l[i]=tolower(l[i]);

  //false if LF words contain SF  
  if( (" "+l+" ").find(" "+s+" ")!=string::npos ) return false;    

  return true;
}


//first=true: allow 1-ahpha, 0 don't allow
long AbbrStra::search_backward(long sloc, long tinx, long tloc, const char *abbr, bool first)
{
  long sfloc=sloc, tkinx=tinx, tkloc=tloc; 

   while(sfloc>=0) {
      loop1: while((tkloc>=0)&&(tok[tkinx][tkloc]!=abbr[sfloc])) tkloc--;
      if(tkloc<0) {  
	tkinx--; 
        if(tkinx<0) return 0; //moved to here (Sep-14-07)
	tkloc=strlen(tok[tkinx])-1; 
      }
      else { 
	if(sfloc==0) {
	  if(tkloc!=0) {
	    if(!first) { tkloc--; goto loop1; }
	    else if(isalnum(tok[tkinx][tkloc-1])) { tkloc--; goto loop1; }
	  }
	}
	mod[sfloc][0]=tkinx; 
	mod[sfloc][1]=tkloc;
	sfloc--; tkloc--;
      }
   }

   return 1;
}

long AbbrStra::search_backward_adv(const char *abbr, bool flag)
{
   long i;
   long lna=strlen(abbr);

   i=0;
   while(i<lna){
      if(search_backward(i,mod[i][0],mod[i][1]-1,abbr,flag)) return 1;
      i++;
   }
   return 0;
}

void AbbrStra::extract_lf(long begin, long end)
{
  strcpy(lf,tok[begin]);
  for(long i=begin+1; i<=end; i++) { 
    strcat(lf," "); 
    strcat(lf,tok[i]); 
  }
}


void AbbrStra::extract_lf(long begin, long end, const char *str)
{
  lf_begin_token = begin;
  token(str,tok);
  strcpy(lf,tok[begin]);
  for(long i=begin+1; i<=end; i++) { 
    strcat(lf," "); 
    strcat(lf,tok[i]); 
  }
}

//---
bool AbbrStra::exist_skipword(long nsf)
{
  long i=0, j=0, k;

  while(i<nsf) {
    if(i==(nsf-1)) k=ntk-mod[i][0]-1;
    else k=mod[i+1][0]-mod[i][0]-1;
    if(k>0) j+=k;
    i++;
  }

  if(j>0) return true;
  else return false;
}


bool AbbrStra::exist_n_skipwords(long nsf, long n)
{
  long i=0, j, k;
  bool flag=false;

  //k: # skip words
  while(i<nsf) {
    if(i==(nsf-1)) k=ntk-mod[i][0]-1;
    else k=mod[i+1][0]-mod[i][0]-1;
    if(k>n) return false;
    if(k==n) flag=true;
    i++;
  }

  if(flag) return true;
  else return false;
}

//exists n consecutive skip stopwords between tokens
bool AbbrStra::exist_n_stopwords(long nsf, long n)
{
  long i=0, j, k;
  bool flag=false;

  while(i<nsf) {
    if(i==(nsf-1)) k=ntk-mod[i][0]-1;
    else k=mod[i+1][0]-mod[i][0]-1;
    if(k>n) return false;
    if(k==n) flag=true;
    if(k>0) { //skip word exists
      while(k) {
	if(!wData->stp.find(tok[mod[i][0]+k])) return false;
	k--;
      }
    }
    i++;
  }

  if(flag) return true;
  else return false;
}


bool AbbrStra::stopword_ok(long nsf, long nsw)
{
  long i=0, j, k;

  while(i<nsf) {
    if(i==(nsf-1)) k=ntk-mod[i][0]-1;
    else k=mod[i+1][0]-mod[i][0]-1;
    if(k>nsw) return false;
    if(k>0) { //skip word exists
      while(k) {
	if(!wData->stp.find(tok[mod[i][0]+k])) return false;
	k--;
      }
    }
    i++;
  }

  return true;
}

bool AbbrStra::skip_stop_ok(long nsf, long nsw, long n)
{
  long i=0, j, k, nstp;

  while(i<nsf) {
    if(i==(nsf-1)) k=ntk-mod[i][0]-1;
    else k=mod[i+1][0]-mod[i][0]-1;
    if(k>nsw) return false;
    //if(k>0) { //skip word exists
    if(k>(nsw-n)) { 
      nstp=0; //# skiped stopword between tokens 
      while(k) {
	if(wData->stp.find(tok[mod[i][0]+k])) nstp++;
	k--;
      }
      if(nstp<n) return false;
    }
    i++;
  }

  return true;
}


bool AbbrStra::skip_stop_ok2(long nsf, long nsw, long n)
{
  long i=0, j, k, nstp;

  while(i<nsf) {
    if(i==(nsf-1)) k=ntk-mod[i][0]-1;
    else k=mod[i+1][0]-mod[i][0]-1;
    if((k>0)&&(k!=nsw)) return false;
    if(k>0) { //skip word exists
      nstp=0; //# skiped stopword between tokens 
      while(k) {
	if(wData->stp.find(tok[mod[i][0]+k])) nstp++;
	k--;
      }
      if(nstp<n) return false;
    }

    i++;
  }

  return true;
}


bool AbbrStra::skipword_ok(long nsf, long nsw)
{
  long i=0, j, k;

  while(i<nsf) {
    if(i==(nsf-1)) k=ntk-mod[i][0]-1;
    else k=mod[i+1][0]-mod[i][0]-1;
    if(k>nsw) return false;
    i++;
  }

  return true;
}


bool AbbrStra::is_subword(long nsf)
{
  long i=0;
  char word[1000];

  while(i<nsf) {
    if(mod[i][1]!=0) {
      strcpy(word,tok[mod[i][0]]+mod[i][1]);
      if(wData->wrdset.count(word)==0) return false;
    }
    i++;
  }

  return true;
}


bool AbbrStra::is_BeginWrdMatch(long nsf, bool general)
{
  long i=0, j;
  bool *bwm = new bool [ntk]; //BeginWrdMatch of a given tok

  for(j=0; j<ntk; j++) bwm[j] = false;

  while(i<nsf) {
    if(mod[i][1]==0)
      bwm[mod[i][0]] = true;
    else if( general && (!isalnum(tok[mod[i][0]][mod[i][1]-1])) ) 
      bwm[mod[i][0]] = true; 
    i++;
  }

  for(j=0; j<nsf; j++) 
    if(!bwm[mod[j][0]]) { 
      delete [] bwm; 
      return false;
    }

  delete [] bwm;

  return true;
}


bool AbbrStra::is_WithinWrdMatch(long nsf, bool general)
{
  long i=0, wwm=0;

  while(i<nsf) {
    if(!general) { 
      if(mod[i][1]>0) wwm++; 
    }
    else {
      if(mod[i][1]>0 && isalnum(tok[mod[i][0]][mod[i][1]-1])) wwm++;
    }
    i++;
  }

  if(wwm>0) return true;
  else return false;
}


bool AbbrStra::is_FirstLetMatch(long nsf, bool general)
{
  long i=0, flm=0, flm2=0;

  while(i<nsf) {
    if(mod[i][1]==0) flm++;
    else if( general && (!isalnum(tok[mod[i][0]][mod[i][1]-1])) ) { 
      flm++; flm2++;
    }
    i++;
  }

  if(flm==nsf) return true;
  else return false;
}


bool AbbrStra::is_FirstLetMatch2(long nsf, bool general)
{
  long i=0, flm=0, flm2=0;

  while(i<nsf) {
    if(mod[i][1]==0) flm++;
    else if( general && (!isalnum(tok[mod[i][0]][mod[i][1]-1])) ) { 
      flm++; flm2++;
    }
    i++;
  }
  
  if( (flm==nsf) && (flm2>=1) ) return true;
  else return false;
}


bool AbbrStra::is_FirstLetSMatch(const char *abbr, bool general)
{
  long i=0, j=strlen(abbr)-1, flm=0, lsm=0;
 
  while(i<j) {
    if(mod[i][1]==0) flm++;
    else if( general && (!isalnum(tok[mod[i][0]][mod[i][1]-1])) ) flm++;
    i++;
  }

  if( (tok[mod[j][0]][mod[j][1]]=='s') &&
      (mod[j][1]==(strlen(tok[mod[j][0]])-1)) && 
      mod[j][0]==mod[j-1][0] ) lsm++;

  if((flm==j) && (lsm==1)) return true;
  else return false;
}


bool AbbrStra::is_ContLetMatch(long nsf)
{
  long i=0, cl=1;

  while(i<(nsf-1)) {
    if( mod[i][0]==mod[i+1][0] &&
	(mod[i][1]+1)==mod[i+1][1] ) cl++;
    i++;
  }

  if(cl>=2) return true;
  else return false;
}
//----


//---1st ch must be alnum & at least one alphabet for all
//str1: sf
bool AbbrStra::set_condition(const char *str1)
{
  int n=0, m=0, o=0;

  switch(setCondition) {
    case 1: //all alphabet SFs
      for(long i=strlen(str1)-1; i>=0; i--)
	if(!isalpha(str1[i]))
	  return false;
      return true;
      break;
    case 2: //at least one non-alphabet
      if(!isalnum(str1[0])) return false;
      for(long i=strlen(str1)-1; i>=0; i--) {
	if(isalpha(str1[i])) n++;
	else m++;
      }
      if( (n>0) && (m>0) ) return true;
      else return false;
      break;
    case 3: //only alnum & at least one num 
      for(long i=strlen(str1)-1; i>=0; i--) {
	if(!isalnum(str1[i])) return false;
	if(isalpha(str1[i])) n++;
	if(isdigit(str1[i])) m++;
      }
      if( (n>0) && (m>0) ) return true;
      else return false;
      break;
    case 4: //only alpha and non-alnum & at least one non-alnum 
      if(!isalpha(str1[0])) return false;
      for(long i=strlen(str1)-1; i>=0; i--) {
	if(isdigit(str1[i])) return false;
	if(!isalnum(str1[i])) n++;
      }
      if(n>0) return true;
      else return false;
      break;
    case 5: //at least one non-alnum 
      if(!isalnum(str1[0])) return false;
      for(long i=strlen(str1)-1; i>0; i--) {
	if(!isalnum(str1[i])) return true;
      }
      return false;
      break;
    case 6: //at least one num and non-alnum
      if(!isalnum(str1[0])) return false;
      for(long i=strlen(str1)-1; i>=0; i--) {
	if(isalpha(str1[i])) n++;
	if(isdigit(str1[i])) m++;
	if(!isalnum(str1[i])) o++;
      }
      if( (n>0) && (m>0) && (o>0) ) return true;
      else return false;
      break;
    case 7: //1+2 (SH algorithm)
      if(!isalnum(str1[0])) return false;
      for(long i=strlen(str1)-1; i>=0; i--)
	if(isalpha(str1[i])) return true;
      return false;
      break;
    default:
      cout << "Not defined set condition\n"; 
      exit(1);
  }
}

//---
//same as FirstLet::set_condition
//but requires extra set conditions 
bool FirstLetOneChSF::set_condition(const char *shrtf, const char *longf, char *str)
{
  long i=0, len=strlen(shrtf), numtk;
  char tk[1000][1000];

  //sf conditions: all alphabet
  while(i<len && isalpha(shrtf[i])) i++;
  if(i!=len) return false; 

  //lf conditions: #tok>=|SF|, 1st ch of words must be alphabet
  numtk = tokenize(longf,tk);
  if(len>numtk) return false;

  for(i=0; i<len; i++) 
    str[i] = tk[numtk-len+i][0];
  str[i] = '\0';

  if(!is_alpha(str)) return false;

  return true;
}


long FirstLetOneChSF::strategy(const char *sf_, const char *str_) {
  long lna,lnt,flag;
  bool genFL=false; //1:allow 1-ahpha for 1ch of SF match, 0:don't
  char phr[10000], phrl[10000];    

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  get_str(str_,phr,1); //phr: 1st token of str from back
  str_tolower(phr,phrl);  
  //conditions
  if(is_onealpha(phr)) return 0; //last token includes 1 alphabet
  if(isupper_str(phr)) return 0; //last token is all upper-case alphabet
  if(wData->stp.find(phrl)) return 0; //last token is stopword
  if(!wData->lfs.find(phrl)) return 0; //lfs (1-ch sf) for FirstLet match cases < 2

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!skipword_ok(lna,0)) continue; 
    if(!is_FirstLetMatch(lna,genFL)) continue; //not allow 1-alpha

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}
//---

bool FirstLet::set_condition(const char *shrtf, const char *longf, char *str)
{
  long i=0, len=strlen(shrtf), numtk;
  char tk[1000][1000];

  //sf conditions
  while(i<len && isalpha(shrtf[i])) i++;
  if(i!=len) return false; 

  //lf conditions
  numtk = tokenize(longf,tk);
  if(len>numtk) return false;

  for(i=0; i<len; i++) 
    str[i] = tk[numtk-len+i][0];
  str[i] = '\0';

  if(!is_alpha(str)) return false;

  return true;
}


long FirstLet::strategy(const char *sf_, const char *str_) {
  long lna,lnt,flag;
  bool genFL=false; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!skipword_ok(lna,0)) continue; 
    if(!is_FirstLetMatch(lna,genFL)) continue; //not allow 1-alpha

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long FirstLetGen::strategy(const char *sf_, const char *str_)
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!skipword_ok(lna,0)) continue; 
    if(!is_FirstLetMatch2(lna,genFL)) continue; //at least 1-alpha

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long FirstLetGen2::strategy(const char *sf_, const char *str_)
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!skipword_ok(lna,0)) continue; 
    if(!is_FirstLetMatch(lna,genFL)) continue; 

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


bool FirstLetGenS::set_condition(const char *str)
{
  if(str[strlen(str)-1]!='s') return false;

  for(long i=strlen(str)-2; i>=0; i--) {
    if(!isupper(str[i])) return false;
    if(!isalpha(str[i])) return false; //necessary?
  }

  return true;
}


long FirstLetGenS::strategy(const char *sf_, const char *str_)
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  if(!set_condition(sf_)) return 0;

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!skipword_ok(lna,0)) continue; 
    if(!is_FirstLetSMatch(sf,genFL)) continue;

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long FirstLetGenStp::strategy(const char *sf_, const char *str_)
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!exist_skipword(lna)) continue; 
    if(!stopword_ok(lna,1)) continue; 
    if(!is_FirstLetMatch(lna,genFL)) continue; 

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long FirstLetGenStp2::strategy(const char *sf_, const char *str_)
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!exist_n_stopwords(lna,2)) continue; 
    if(!is_FirstLetMatch(lna,genFL)) continue; 

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long FirstLetGenSkp::strategy(const char *sf_, const char *str_)
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!exist_skipword(lna)) continue; 
    if(!skipword_ok(lna,1)) continue; 
    if(!is_FirstLetMatch(lna,genFL)) continue; 

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long WithinWrdWrd::strategy(const char *sf_, const char *str_) 
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!skipword_ok(lna,0)) continue; 
    if(!is_subword(lna)) continue; 
    if(!is_WithinWrdMatch(lna,genFL)) continue;

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long WithinWrdFWrd::strategy(const char *sf_, const char *str_) 
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!skipword_ok(lna,0)) continue; 
    if(!is_subword(lna)) continue; 
    if(!is_BeginWrdMatch(lna,genFL)) continue; 
    if(!is_WithinWrdMatch(lna,genFL)) continue;

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long WithinWrdFWrdSkp::strategy(const char *sf_, const char *str_) 
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!exist_skipword(lna)) continue; 
    if(!skipword_ok(lna,1)) continue; 
    if(!is_subword(lna)) continue; 
    if(!is_BeginWrdMatch(lna,genFL)) continue; 
    if(!is_WithinWrdMatch(lna,genFL)) continue;
    
    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long WithinWrdLet::strategy(const char *sf_, const char *str_)
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!skipword_ok(lna,0)) continue; 
    if(!is_WithinWrdMatch(lna,genFL)) continue;

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long WithinWrdFLet::strategy(const char *sf_, const char *str_) 
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!skipword_ok(lna,0)) continue; 
    if(!is_BeginWrdMatch(lna,genFL)) continue; 
    if(!is_WithinWrdMatch(lna,genFL)) continue;

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long WithinWrdFLetSkp::strategy(const char *sf_, const char *str_) 
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!exist_skipword(lna)) continue; 
    if(!skipword_ok(lna,1)) continue; 
    if(!is_BeginWrdMatch(lna,genFL)) continue; 
    if(!is_WithinWrdMatch(lna,genFL)) continue;

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long ContLet::strategy(const char *sf_, const char *str_) 
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!skipword_ok(lna,0)) continue; 
    if(!is_BeginWrdMatch(lna,genFL)) continue; 
    if(!is_ContLetMatch(lna)) continue; 

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long ContLetSkp::strategy(const char *sf_, const char *str_) 
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!exist_skipword(lna)) continue; 
    if(!skipword_ok(lna,1)) continue; 
    if(!is_BeginWrdMatch(lna,genFL)) continue; 
    if(!is_ContLetMatch(lna)) continue; 

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}


long AnyLet::strategy(const char *sf_, const char *str_) 
{
  long lna,lnt,flag;
  bool genFL=true; //1:allow 1-ahpha for 1ch of SF match, 0:don't

  str_tolower(sf_,sf);
  str_tolower(str_,text);

  token(text,tok);
  lna = strlen(sf);
  lnt = strlen(tok[ntk-1]);

  flag = search_backward(lna-1,ntk-1,lnt-1,sf,genFL);
  if(!flag) return 0;

  do {
    if(!skipword_ok(lna,1)) continue; 

    extract_lf(mod[0][0],ntk-1,str_);
    return 1;
  } while(search_backward_adv(sf,genFL));

  return 0;
}



//-----
AbbrStra * StratUtil::strat_factory(string name)
{
  if(name=="FirstLetOneChSF") return new FirstLetOneChSF;
  else if(name=="FirstLet") return new FirstLet;
  else if(name=="FirstLetGen") return new FirstLetGen;
  else if(name=="FirstLetGen2") return new FirstLetGen2;
  else if(name=="FirstLetGenS") return new FirstLetGenS;
  else if(name=="FirstLetGenStp") return new FirstLetGenStp;
  else if(name=="FirstLetGenStp2") return new FirstLetGenStp2;
  else if(name=="FirstLetGenSkp") return new FirstLetGenSkp;
  else if(name=="WithinWrdWrd") return new WithinWrdWrd;
  else if(name=="WithinWrdFWrd") return new WithinWrdFWrd;
  else if(name=="WithinWrdFWrdSkp") return new WithinWrdFWrdSkp;
  else if(name=="WithinWrdLet") return new WithinWrdLet;
  else if(name=="WithinWrdFLet") return new WithinWrdFLet;
  else if(name=="WithinWrdFLetSkp") return new WithinWrdFLetSkp;
  else if(name=="ContLet") return new ContLet;
  else if(name=="ContLetSkp") return new ContLetSkp;
  else if(name=="AnyLet") return new AnyLet;
  else { cout << "Fail strat_factory\n"; exit(1); }
}


//check if sf is ok and assign a group
//if sf length > 5, use 5!!
//grp will be Al+#ChInSF, Num+#ChInSF, or Spec+#ChInSF
bool StratUtil::group_sf(const char *sf,  string &grp)
{
  long i, j, len=strlen(sf);
  long al=0, num=0, nonalnum=0;
  long paren=0, sbrac=0;

  grp = "";                      // if failure, no group

  if(!isalnum(sf[0])) return false; //1sf ch must alnum
  for(i=0; i<len; i++) {
    if(isalpha(sf[i])) al++;
    else if(isdigit(sf[i])) num++;
    else nonalnum++;
  }
  if(al<1) return false; //at least one alphabet

  //false for one parenthesis or square bracket
  for(i=len-1; i>=0; i--) { 
    if(sf[i]=='(') paren++;
    if(sf[i]==')') paren--;
    if(sf[i]=='[') sbrac++;
    if(sf[i]==']') sbrac--;
  }
  if(paren!=0 || sbrac!=0) return false;

  if(al==len) grp.assign("Al");
  else if(num>0) grp.assign("Num");
  else if(nonalnum>0) grp.assign("Spec");
  else { cout << "No sf group\n"; exit(1); }

  //append sf length
  len = len>5 ? 5 : len;

    switch(len) {
      case 1:
        grp.append("1");
	break;
      case 2:
        grp.append("2");
	break;
      case 3:
        grp.append("3");
	break;
      case 4:
        grp.append("4");
	break;
      case 5:
        grp.append("5");
	break;
      default:
	cout << "Not defined #-ch SF" << endl;
	exit(1);
    }

  return true;
}

//add the condition |lf|>|sf|
bool StratUtil::group_sf(const char *sf,  const char *lf, string &grp)
{
  long i, j, len=strlen(sf);
  long al=0, num=0, nonalnum=0;
  long paren=0, sbrac=0;

  if(strlen(lf)<len) return false; //|lf|>|sf|
  if(!isalnum(sf[0])) return false; //1sf ch must alnum
  for(i=0; i<len; i++) {
    if(isalpha(sf[i])) al++;
    else if(isdigit(sf[i])) num++;
    else nonalnum++;
  }
  if(al<1) return false; //at least one alphabet
  if(al>10) return false; //|alpha sf| is at most 10
  if(num_token(sf)>2) return false; //added Feb-21-08

  //false for one parenthesis or square bracket
  for(i=len-1; i>=0; i--) { 
    if(sf[i]=='(') paren++;
    if(sf[i]==')') paren--;
    if(sf[i]=='[') sbrac++;
    if(sf[i]==']') sbrac--;
  }
  if(paren!=0 || sbrac!=0) return false;

  if(al==len) grp.assign("Al");
  else if(num>0) grp.assign("Num");
  else if(nonalnum>0) grp.assign("Spec");
  else { cout << "No sf group\n"; exit(1); }

  //append sf length
  len = len>5 ? 5 : len;

    switch(len) {
      case 1:
        grp.append("1");
	break;
      case 2:
        grp.append("2");
	break;
      case 3:
        grp.append("3");
	break;
      case 4:
        grp.append("4");
	break;
      case 5:
        grp.append("5");
	break;
      default:
	cout << "Not defined #-ch SF" << endl;
	exit(1);
    }

  return true;
}


//remove non-alnum in str1 and save it to str2
void StratUtil::remove_nonAlnum(const char *str1, char *str2)
{
  long i=0, j=0;

  while(str1[i]) {
    if(isalnum(str1[i])) {
      str2[j] = str1[i];
      j++;
    }
    i++;
  } 
  str2[j] = '\0';
}


vector<string> StratUtil::get_strats(string s)
{
  if(s=="Al1") return Al1;
  else if(s=="Al2") return Al2;
  else if(s=="Al3") return Al3;
  else if(s=="Al4") return Al4;
  else if(s=="Al5") return Al5;
  else if(s=="Num2") return Num2;
  else if(s=="Num3") return Num3;
  else if(s=="Num4") return Num4;
  else if(s=="Num5") return Num5;
  else if(s=="Spec2") return Spec2;
  else if(s=="Spec3") return Spec3;
  else if(s=="Spec4") return Spec4;
  else if(s=="Spec5") return Spec5;
  else { cout << "Incorrect name\n"; exit(1); }
}


void StratUtil::push_back_strat(string sgp, string strat)
{
  if(sgp=="Al1") Al1.push_back(strat);
  else if(sgp=="Al2") Al2.push_back(strat);
  else if(sgp=="Al3") Al3.push_back(strat);
  else if(sgp=="Al4") Al4.push_back(strat);
  else if(sgp=="Al5") Al5.push_back(strat);
  else if(sgp=="Num2") Num2.push_back(strat);
  else if(sgp=="Num3") Num3.push_back(strat);
  else if(sgp=="Num4") Num4.push_back(strat);
  else if(sgp=="Num5") Num5.push_back(strat);
  else if(sgp=="Spec2") Spec2.push_back(strat);
  else if(sgp=="Spec3") Spec3.push_back(strat);
  else if(sgp=="Spec4") Spec4.push_back(strat);
  else if(sgp=="Spec5") Spec5.push_back(strat);
}


long StratUtil::exist_upperal(const char *str)
{
  long i, len=strlen(str);

  for(i=0; i<len; i++)
    if(isupper(str[i]))
      return 1;
  return 0;
}

long StratUtil::num_token(const char *str)
{
   long i,j=0,k=0;
   long n=strlen(str)-1;

   while(isblank(str[n])) n--; 

   while(str[j]){
      while(isblank(str[j]))j++;
      i=j;
      while((str[j])&&(!isblank(str[j])))j++;
      if(str[j]){
	 k++;
         j++;
      }
   }
   if((j-1)>n) k--; //added by Sohn (Jan-17-08): "ab cd " -> 2 tokens
   return k+1; //# tokens
}
//-----
