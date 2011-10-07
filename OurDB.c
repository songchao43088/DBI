#include <stdio.h>
#include "hf.h"
int main(int argc, char** argv){
	const char *path = "OurDB.db";
	char *schema[3];
	schema[0]="i2";
	schema[1]="c9";
	schema[2]="r4";
	hf_create(path, schema, 3);
	return 0;
}
