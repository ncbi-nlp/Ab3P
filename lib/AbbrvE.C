#include "AbbrvE.h"
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <cstring>

namespace iret {

  Find_Seq::Find_Seq( void  )
  /* initializers work in C++0x
     :
     seq_i ( { "i", "ii", "iii", "iv", "v", "vi" } ),
     seq_I ( { "I", "II", "III", "IV", "V", "VI" } ),
     seq_a ( { "a", "b", "c", "d", "e", "f" } ),
     seq_A ( { "A", "B", "C", "D", "E", "F" } )
  */
  {
    seq_i.push_back("i");
    seq_i.push_back("ii");
    seq_i.push_back("iii");
    seq_i.push_back("iv");
    seq_i.push_back("v");
    seq_i.push_back("vi");
    
    seq_I.push_back("I");
    seq_I.push_back("II");
    seq_I.push_back("III");
    seq_I.push_back("IV");
    seq_I.push_back("V");
    seq_I.push_back("VI");
    
    seq_a.push_back("a");
    seq_a.push_back("b");
    seq_a.push_back("c");
    seq_a.push_back("d");
    seq_a.push_back("e");
    seq_a.push_back("f");
    
    seq_A.push_back("A");
    seq_A.push_back("B");
    seq_A.push_back("C");
    seq_A.push_back("D");
    seq_A.push_back("E");
    seq_A.push_back("F");
    
  }

  void 
  Find_Seq::flag_seq( int numa, char* abbs[] ) {

    my_numa = numa;
    my_abbs = abbs;
    
    my_rate.resize(numa);
    for ( int i = 0; i < numa; ++i )
      my_rate[i] = true;

    find_seq(seq_i);
    find_seq(seq_I);
    find_seq(seq_a);
    find_seq(seq_A);

    create_seq();
  }

  void Find_Seq::flag_seq( const vector<Pot_Abbr> & abb ) {
    char ** abbs = new char*[ abb.size() ];
    for ( int i = 0; i < abb.size(); ++i )
      abbs[i] = abb[i].abbs;
    flag_seq( abb.size(), abbs );
    delete [] abbs;
  }    


  void
  Find_Seq::find_seq( const vector<string> & seq ) {
    
    for ( int i_abbr = 0; i_abbr < my_numa-1; ++i_abbr ) {
      // need to see at least 2 in sequence
      
      if ( seq[0] == my_abbs[i_abbr] ) {
        int i_seq = 1;
        while ( i_seq < seq.size()  and
                i_seq + i_abbr < my_numa  and
                seq[i_seq] == my_abbs[i_abbr + i_seq ] )
          ++i_seq;
      
        if ( i_seq > 1 )
          for ( int i = 0; i < i_seq; ++i )
            my_rate[i_abbr+i] = false;
      }
      
    }
  }
  
  void
  Find_Seq::create_seq( void ) {
    
    for ( int i_abbr = 0; i_abbr < my_numa; ++i_abbr ) {
      
      size_t len = std::strlen( my_abbs[i_abbr] );
      
      if ( my_abbs[i_abbr][len-1] == '1' ) {
        // create sequence and test
        
        string prefix( my_abbs[i_abbr], len-1 );
        size_t seq_len = my_numa - i_abbr; // max possible length
        vector<string> seq;
        // sequence starts with 1
        for ( int i= 1; i <= seq_len; ++i ) {
          std::ostringstream stream(prefix,std::ios::app);
          stream << i;
          seq.push_back( stream.str() );
        }

        //        cout << seq << '\n';
        find_seq(seq);
      }
    }
  }
  

  
  AbbrvE::AbbrvE(long ta ){
    tta=ta;
    //    abbl=new char*[tta];
    //    abbs=new char*[tta];
    //    nt=new int[tta];
    //    numa=0;
    pMt=new MPtok;
    setup_Test();
  }

  AbbrvE::~AbbrvE(){
    cleara();
    delete pMt;
  }

#if 0
  
void AbbrvE::Extract(char*pch){
   long i,j,k,u,flag;
   int ix;

   if ( strlen(pch) <= 0 )	// no text to look at
     return;

   token(pch);

   i=j=k=0;
   flag=0;
   while(i< lst.size() ){
      if(!strcmp("(",lst[i])){
         if(flag)k=j+1;
         if((i>k)&&(strcmp(")",lst[i-1]))){
            j=i;
            flag=1;
         }
      }
      if(!strcmp(")",lst[i])){
         if(!flag){j=k=i+1;}
         else {
            if(((j>k)&&(i<j+12))&&(i>j+1)){
               if(k<j-10)k=j-10;
               strcpy(cnam,lst[k]);
               for(u=k+1;u<j;u++){
                  strcat(cnam," ");
                  strcat(cnam,lst[u]);
               }
               ix=strlen(cnam);
               abbl[numa]=new char[ix+1];
               strcpy(abbl[numa],cnam);

               strcpy(cnam,lst[j+1]);
               for(u=j+2;u<i;u++){
                  strcat(cnam," ");
                  strcat(cnam,lst[u]);
               }
               nt[numa]=i-j-1;
               ix=strlen(cnam);
               abbs[numa]=new char[ix+1];
               strcpy(abbs[numa],cnam);
               if(Test(abbs[numa]))numa++;
               else{ //if test done earlier would not need to allocate memory
                     //until known to be needed
                 delete [] abbs[numa];
                 delete [] abbl[numa];
               }
               flag=0;
            }
            else {
               flag=0;
               k=i+1;
            }
         }
      }
      i++;
   }
}

#endif

//modified  Jan-9-2008
//extract SF in [], parse until ';' or ',' in () or []
  void AbbrvE::Extract2(const char*pch, int sentence_offset ){
 
   if ( strlen(pch) <= 0 )	// no text to look at
     return;

   token2( pch, sentence_offset); // alpha beta (AB) -> alpha beta ( AB )
 
   Extract2_ch( pch, "(", ")" );
   Extract2_ch( pch, "[", "]" );

}


void AbbrvE::Extract2_ch( const char*pch,
                          const char * openCh, const char * closeCh ) {
   long i,j,k,u,ii,flag;
   int ix;

   i=j=k=0;
   flag=0;
   
   while(i< lst.size() ){
     Pot_Abbr pot_abbr;
      if(!strcmp(openCh,lst[i])){
         if(flag)k=j+1; //increment after seeing both '(' and ')'
         if((i>k)&&(strcmp(closeCh,lst[i-1]))){
            j=i; //index of '(' 
            flag=1;
         }
      }
      if(!strcmp(closeCh,lst[i])){
         if(!flag){j=k=i+1;} //next token
         else {
            if(((j>k)&&(i<j+12))&&(i>j+1)){
               if(k<j-10)k=j-10;
               strcpy(cnam,lst[k]);
               pot_abbr.abbl_tokens.push_back( lst[k] );
               for(u=k+1;u<j;u++){
                  strcat(cnam," ");
                  strcat(cnam,lst[u]);
                  pot_abbr.abbl_tokens.push_back( lst[u] );
               }
               ix=strlen(cnam);
               pot_abbr.abbl=new char[ix+1];
               strcpy(pot_abbr.abbl,cnam);

               strcpy(cnam,lst[j+1]);
               pot_abbr.abbs_tokens.push_back( lst[j+1] );
               for(u=j+2;u<i;u++){
                  strcat(cnam," ");
                  strcat(cnam,lst[u]);
                  pot_abbr.abbs_tokens.push_back( lst[u] );
               }
               pot_abbr.nt=i-j-1; //# abbr tokens
               ix=strlen(cnam);
               int orig_ix = ix;
	       
	       //---- parse until ';' or ',' 
	       ii=0;
	       while(ii<ix) {
		 if( ((cnam[ii]==';')&&(cnam[ii+1]==' ')) || 
                     ((cnam[ii]==',')&&(cnam[ii+1]==' ')) ) {
                    ix=ii;
		    cnam[ii]='\0';
		    break;
		  }
		  ii++;
	       }
               if ( ix != orig_ix ) {
                 // remove spaces from end of cnam
                 while ( cnam[ix-1] == ' ' )
                   cnam[--ix] = '\0';
                   
                 // remove extra tokens from abbs_tokens
                 vector<offset_char_p> & tokens = pot_abbr.abbs_tokens;
                 int sum_token_len = 0;
                 for ( int it = 0; it < tokens.size(); ++it ) {
                   sum_token_len += tokens[it].text.size();
                   if ( sum_token_len >= ix ) {
                     tokens.erase( tokens.begin() + it + 1, tokens.end() );
                     int extra_char = sum_token_len - ix;
                     if ( extra_char > 0 )
                       tokens[it].text.erase( tokens[it].text.size() - extra_char );
                     break;
                   }
                   sum_token_len += 1; // spaces between tokens
                 }
               }
	       //----
	       
               pot_abbr.abbs=new char[ix+1];
               strcpy(pot_abbr.abbs,cnam);
               if ( Test(pot_abbr.abbs) ) abb.push_back(pot_abbr);
               else{ //if test done earlier would not need to allocate memory
                     //until known to be needed
                 delete [] pot_abbr.abbs;
                 delete [] pot_abbr.abbl;
             }
               flag=0;
            }
            else {
               flag=0;
               k=i+1;
            }
         }
      }
      i++;
   }
  }



void AbbrvE::token(const char *pch){
   long i=1,j=0,k=0;
   long u=1,flag=0;
   char c,*str=cnam;
   int size=strlen(pch);
   if(size>cnam_size) {
     cerr<<"Scratch space "<<cnam_size<<", needed "<<size<<endl;
     exit(1);
   }
   lst.clear();                 // ready space for tokens
   cnam[0]=pch[0];
   while(c=pch[i]){
      switch(c){
         case '(': if(isblank(str[u-1])){
                      str[u++]=pch[i++];
                      if(!isblank(pch[i])){
                         str[u++]=' ';
                      }
                      flag=1;
                   }
                   else str[u++]=pch[i++];
                   break;
         case ')': if(flag){
                      if(!isblank(str[u-1])){
                         str[u++]=' ';
                         str[u++]=pch[i++];
                      }
                      if(!isblank(pch[i]))str[u++]=' ';
                      flag=0;
                   }
                   else str[u++]=pch[i++];
                   break;
         default: str[u++]=pch[i++];
      }
   }
   while((u>0)&&(isblank(str[u-1])))u--;
   str[u]='\0';    
  
   while(str[j]){
      while(isblank(str[j]))j++;
      i=j;
      while((str[j])&&(!isblank(str[j])))j++;

      offset_char_p offset_char;
      lst.push_back( offset_char );
      lst.back().set_text( &str[i], j-i );
      lst.back().offset = i;

      if(str[j]){
         k++;
         j++;
      }
   }
   assert( lst.size() == k+1 );
}


//both () & [] Jan-9-2008
//(G(1)) -> ( G(1) ) Jan-28-2008
  void AbbrvE::token2(const char *pch, int sentence_offset){

   long i=1,j=0,k=0;
   long u=1;
   vector<bool> openChFlag1,openChFlag2;
   long cflag;
   long ii, jj, kk, sz;
   char c,*str=cnam;
   lst.clear();                 // ready space for tokens
   cnam[0]=pch[0];
   cnam_offset[0] = 0;
   while(c=pch[i]){
      switch(c){
        case '(':   	   
                   //--- (h)alpha -> (h)alpha, (h)-alpha -> ( h ) -alpha
                   ii=kk=i;
		   cflag=0; 
		   while(pch[ii] && !isblank(pch[ii])) { //pch[ii] can be '\0'
		     if(pch[ii]=='(') cflag -= 1;
		     else if(pch[ii]==')') { cflag += 1; kk=ii; }
		     ii++; 
		   }

		   if(!cflag && isalnum(pch[kk+1])) { //if alnum right after ')'
		     while(i<ii) {
                       cnam_offset[u] = i;
                       str[u++]=pch[i++];
                     }
		     break;
		   }
		   //---
		   
	           if(isblank(str[u-1])){
                      cnam_offset[u] = i;
                      str[u++]=pch[i++];
                      if(!isblank(pch[i])){
                         cnam_offset[u] = cnam_offset[u-1];
                         str[u++]=' ';
                      }
                      openChFlag1.push_back(true);
                   }
                   else {
                      cnam_offset[u] = i;
		      str[u++]=pch[i++];
                      openChFlag1.push_back(false);
		   }

                   break;

        case ')':  sz = openChFlag1.size();
	           if(sz>0 && openChFlag1[sz-1]){ //modified Jan-28-08
                      if(!isblank(str[u-1])){
                        cnam_offset[u] = cnam_offset[u-1];
                        str[u++]=' ';
                        cnam_offset[u] = i;
                        str[u++]=pch[i++]; //pch[i++] is ')'
                      }
		      //---added (Jan-11-08): (BIV; ), -> ( BIV; ) ,
		      else if(!isblank(pch[i+1])){
                        cnam_offset[u] = i;
			 str[u++]=pch[i++]; //pch[i++] is ')'
		      }
	              //---

                      if(!isblank(pch[i])) {
                        cnam_offset[u] = cnam_offset[u-1];
                        str[u++]=' '; //pch[i] must be after ')'
                      }
                   }
                   else {
                     cnam_offset[u] = i;
                     str[u++]=pch[i++];
                   }

		   if(sz>0) openChFlag1.pop_back();

                   break;

         case '[': 
                   //--- [h]alpha -> [h]alpha
                   ii=kk=i;
		   cflag=0; 
		   while(pch[ii] && !isblank(pch[ii])) { //pch[ii] can be '\0'
		     if(pch[ii]=='[') cflag -= 1;
		     else if(pch[ii]==']') { cflag += 1; kk=ii; }
		     ii++; 
		   }

		   if(!cflag && isalnum(pch[kk+1])) { //if alnum right after ')'
		     while(i<ii) {
                       cnam_offset[u] = i;
                       str[u++]=pch[i++];
                     }
		     break;
		   }
		   //---

                   if(isblank(str[u-1])){
                     cnam_offset[u] = i;
                     str[u++]=pch[i++];
                     if(!isblank(pch[i])){
                       cnam_offset[u] = cnam_offset[u-1];
                       str[u++]=' ';
                     }
                     openChFlag2.push_back(true);
                   }
                   else {
                     cnam_offset[u] = i;
                     str[u++]=pch[i++];
                     openChFlag2.push_back(false);
		   }

                   break;

        case ']':  sz=openChFlag2.size(); 
	           if(sz>0 && openChFlag2[sz-1]){ //modified Jan-28-08
                      if(!isblank(str[u-1])){
                        cnam_offset[u] = cnam_offset[u-1];
                        str[u++]=' ';
                        cnam_offset[u] = i;
                        str[u++]=pch[i++];
                      }
		      //---added (Jan-11-08): [BIV; ], -> [ BIV; ] ,
		      else if(!isblank(pch[i+1])){ 
                        cnam_offset[u] = i;
                        str[u++]=pch[i++];
		      }
	              //---
                      if(!isblank(pch[i])) {
                        cnam_offset[u] = cnam_offset[u-1];
                        str[u++]=' ';
                      }
                        
                   }
                   else {
                     cnam_offset[u] = i;
                     str[u++]=pch[i++];
                   }

		   if(sz>0) openChFlag2.pop_back();
		   
                   break;
         default:
           cnam_offset[u] = i;
           str[u++]=pch[i++];
      }
   }
   while((u>0)&&(isblank(str[u-1])))u--;
   str[u]='\0';    
      
   while(str[j]){
      while(isblank(str[j]))j++;
      i=j;
      while((str[j])&&(!isblank(str[j])))j++;

      offset_char_p offset_char;
      lst.push_back( offset_char );
      //      cout << (void*)lst.back().text << '\n';
      lst.back().set_text( &str[i], j-i );
      lst.back().offset = cnam_offset[i] + sentence_offset;

      if(str[j]){
         k++;
         j++;
      }
   }
   assert( lst.size() == k+1 );
}


  void AbbrvE::cleara(void){
    for(int i=0; i < abb.size(); i++ ) {
      delete [] abb[i].abbl;
      delete [] abb[i].abbs;
    }
    abb.clear();
  }

#if 0

//no space before and after abbs[] (because of using token)
int AbbrvE::Test(const char *str){
   long i,j,k;
   char b,c;

   if(strchr(str,'='))return(0);
   if(!strcmp(str,"author's transl"))return(0);
   if(!strcmp(str,"proceedings"))return(0);
   //---added (Jan-11-08) & (Apr 08)
   if(!strcmp(str,"see"))return(0);
   if(!strcmp(str,"and"))return(0);
   if(!strcmp(str,"comment"))return(0);
   if(!strcmp(str,"letter"))return(0);
   //---
   if((str[0]=='e')&&(str[1]=='g')){
      if(!(c=str[2])||(c=='.')||(c==','))return(0);
   }
   if((str[0]=='s')&&(str[1]=='e')&&(str[2]=='e')&&(((b=str[3])==' ')||(b==',')))return(0);
   if('p'==tolower(str[0])){
      if(strchr(str+1,'<'))return(0);
   }
   i=j=k=0;
   while((c=str[i])&&(c!=' ')){
      i++;
      if(isdigit(c))j++;
      if(isalpha(c))k++;
      if((i==j)&&(i==3))return(0);
   }
   if((i==j)||(k==0))return(0);
   else return(1);
} 

#endif

  bool AbbrvE::prefix_match( const char *str ) {
    size_t size = strlen(str);
    for ( int i = 0; i < prefix.size(); ++i ) {
      string& pre = prefix[i];
      if ( size > pre.size()  and
           0 == pre.compare( 0, pre.size(), str, pre.size() ) )
        return true;
    }
    return false;
  }

    
//no space before and after abbs[] (because of using token)
bool AbbrvE::Test(const char *str){

   if ( match.find(str) != match.end() ) return false;
   if ( prefix_match(str) ) return false;

   size_t length, letters, digits;
   length = letters = digits = 0;

   char c;
   while((c=str[length])&&(c!=' ')){
     length++;
     if ( isdigit(c) ) digits++;
     if ( isalpha(c) ) letters++;
     
     if( length==digits  and  length>=3 ) return false;
   }
   if ( digits == length ) return false;
   if ( letters <= 0 ) return false;

   return true;
} 

  void AbbrvE::setup_Test( void ) {

    match.insert("author's transl");
    match.insert("proceedings");
    match.insert("see");
    match.insert("and");
    match.insert("comment");
    match.insert("letter");
    match.insert("eg");

    prefix.push_back("=");
    prefix.push_back("eg.");
    prefix.push_back("eg,");
    prefix.push_back("see ");
    prefix.push_back("see,");
    prefix.push_back("p<");
    prefix.push_back("P<");

    // rules added in 2010
    match.insert("e.g.");
    match.insert("ie");
    match.insert("i.e.");
    match.insert("mean");
    match.insert("age");
    match.insert("std");
    match.insert("range");
    match.insert("young");
    match.insert("old");
    match.insert("male");
    match.insert("female");

  }

void AbbrvE::Proc(char *pxh){

   long i,j;
   char *pch,*ptr;
   pMt->segment(pxh);

   // determine sentence offsets
   vector<int> sentence_offsets( pMt->sent.size() );
   find_sentence_offsets( pxh, sentence_offsets );

   for(i=0;i<pMt->sent.size();i++){
     Extract2( (pMt->sent[i]).c_str(), sentence_offsets[i] );
   }
   
   //   seq.flag_seq( numa, abbs );
   seq.flag_seq( abb );
   for(i=abb.size()-1; i >= 0; i--){
     if( ! seq.rate(i) ){
       delete [] abb[i].abbs;
       delete [] abb[i].abbl;
       abb.erase( abb.begin() + i);
     }
   }

}

  void AbbrvE::find_sentence_offsets( const char * text,
                                      vector<int> & offsets ) {
    int loc = 0;
    int text_len = strlen(text);
    for( int i = 0; i < pMt->sent.size(); i++ ) {
      bool found = false;
      for ( ; loc < text_len; ++loc ) {
        char sent_first_char = pMt->sent[i][0];
        int sent_size = pMt->sent[i].size();
        if ( text[loc] == sent_first_char  and
             pMt->sent[i].compare( 0, sent_size, &text[loc], sent_size )
             == 0 ) {
          offsets[i] = loc;
          loc += sent_size;
          found = true;
          break;
        }
      }
      if ( not found ) {
        cerr << "Failed to find sentence: " << pMt->sent[i] << '\n'
             << "in text: " << text << '\n';
      }
    }
  }
  
}
