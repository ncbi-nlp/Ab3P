/*
make a hash set for word (freq>=100, len>=3) 
need a source word list that is formated word|freq on each line
need a "path" file that designates a directoy to save the hash set
*/

#include <fstream>
#include "Dmap.h"
#include <Hash.h>

using namespace std;
using namespace iret;

int main(int argc, char **argv)
{

  if(argc!=2) {
    cout << "Usuage: " << argv[0] << " WordFilename" << endl;
    return 1;
  }
  char * file = argv[1];

  ifstream fin(file); 
  if(!fin) {
    cout << "Cannot open " << file << endl;
    return 1;
  }

  strMap Ct;

  string word;
  long num;
  string dummy;

  long cnt = 0;
  while(getline(fin,word,'|')) {
    fin >> num; 
    getline(fin,dummy);           //remove endl;
    if( word.size() <3) continue; // want length>=3
    if(num < 100) continue;        // want freq>=100
    Ct.add_count(word.c_str(),num);
    cnt++;
  }

  cout << cnt << " words selected" << endl;

  Chash Csh("wrdset3"); 
  Csh.set_path_name("Ab3P");
  Csh.create_ctable(Ct,3);

  return 0;
}
