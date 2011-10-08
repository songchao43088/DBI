#include <stdio.h>
#include <stdlib.h>
#include "hf.h"
int main(int argc, char** argv){
	RID r;
	const char *path = "OurDB.db";
	char *schema[3];
	schema[0]="i2";
	schema[1]="c9";
	schema[2]="r4";
	hf_create(path, schema, 3);
	r = hf_insert(hf_open(path),(void*)"badworld");
	void *record = malloc(sizeof(char)*20);
	hf_record(hf_open(path), r, record);
	return 0;
}
