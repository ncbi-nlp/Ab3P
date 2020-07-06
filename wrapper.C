#include "Ab3P.h"
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>

extern "C" void get_abbrevs_cpp(char *text, char *output, int *output_size) {
    Ab3P ab3p;
    vector<AbbrOut> abbrs;
    string abbrevs;

    ab3p.get_abbrs(text, abbrs); // call main algorithm
    for (int i = 0; i < abbrs.size(); ++i) {
        string line = abbrs[i].sf + "|" + abbrs[i].lf + "\n";
        abbrevs.append(line);
    }

    *output_size = abbrevs.length();

    strcpy(output, abbrevs.c_str()); // Copy result into provided buffer
}

