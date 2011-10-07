#include "hf.h"
#include <stdio.h>
#include <stdlib.h>

int hf_create(const char *path, char **schema, int schemac){
	HFILE hf;
	int i;
	hf.n_records = 0;
	hf.schema = schema;
	hf.content = NULL;
	FILE *fp=fopen(path,"w+");
	if (fp==NULL){
		printf("fail to create file.");
		return 1;
	}
	fprintf(fp,"%d\n",hf.n_records);
	for(i = 0; i < schemac; i++)
		fprintf(fp,"%s,",hf.schema[i]);
	fprintf(fp,"\n");
	fclose(fp);
	return 0;
}
