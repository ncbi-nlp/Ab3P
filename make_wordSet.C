/*
make a hash set for a set of strings
need a file that lists each string on a separate line
need a "path" file that designates a directoy to save the hash set
*/
#include <fstream>
#include "Dmap.h"
#include <Hash.h>

using namespace std;
using namespace iret;

int main(int argc, char **argv)
{

  if(argc!=3) {
    cerr << "Usuage: " << argv[0] << " word_set_file word_set_hash" << endl;
    return 1;
  }

  char * file = argv[1];
  char * hash = argv[2];

  ifstream fin(file);
  if(!fin) {
    cerr << "Cannot open " << file << endl;
    return 1;
  }

  strMap Lst;
  string line;
  while( getline(fin,line) ) {
    Lst.add_count(line.c_str(), 1);
  }

  Hash Hsh(hash); //name of a hash set
  Hsh.set_path_name("Ab3P");
  Hsh.create_htable(Lst,3);

  return 0;
}
