#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// print out records given projection rules in a heap file
// projection is a bitmap array of columns that need to be projected
// pyesno is a flag indicating whether project or not
void printrecord(HFILE *hf, void *record, int * projection, int pyesno);

// parse user input queries, including select "-s" and project "-p" operations
void query(int argc, char** argv);

void import_record(HFILE *hf);
