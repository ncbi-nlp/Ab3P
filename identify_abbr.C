/*
Identify sf & lf pairs from free text using multi-stage algorithm
process one line at a time and print out:
line
  sf|lf|P-precision|strategy
*/

#include "Ab3P.h"
#include <fstream>
#include <vector>
#include <string>

using namespace std;
using namespace iret;

ostream & operator<<( ostream & out, AbbrOut & abbr ) {
  abbr.print(out);
  return out;
}

int main(int argc, char **argv)
{

  //get text from a file
  if(argc!=2) {
    cout << "Usage: " << argv[0] << " filename\n"; 
    exit(1);
  }

  Ab3P ab3p;

  char * file = argv[1];
  ifstream fin(file);
  if(!fin) {
    cout << "Cannot open " << file << endl;
    exit(1);
  }

  string line;
  vector<AbbrOut> abbrs;
  while( getline(fin, line) ) {

    ab3p.get_abbrs( line, abbrs );
    cout << line << endl;
    for ( int i = 0; i < abbrs.size() ; ++i )
      cout << abbrs[i] << endl;
  }

  return 0;
}
