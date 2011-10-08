#include "hf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hf_create(const char *path, char **schema, int schemac){
	HFILE hf;
	int i;
	hf.n_records = 0;
	hf.schema = schema;
	hf.n_fields = schemac;
	hf.content = NULL;
	hf.path = path;
	printf("%d",hf_record_length(&hf));
	FILE *fp=fopen(path,"w+");
	if (fp==NULL){
		printf("fail to create file.");
		exit(1);
	}
	fwrite(&hf.n_records,sizeof(long),1,fp);
	fwrite(&hf.n_fields,sizeof(int),1,fp);
	printf("1\n");
	for(i = 0; i < schemac; i++)
		if (schema[i][0] !='c'){
	printf("2\n");
			fwrite(hf.schema[i],sizeof(char),2,fp);}
		else {
	printf("3\n");
			char tmp;
			fwrite(hf.schema[i],sizeof(char),1,fp);
			tmp=atoi(hf.schema[i]+1);
			fwrite(&tmp,sizeof(char),1,fp);
		}
	fclose(fp);
	return 0;
}

HFILE *hf_open(const char *path){
	int n_records;	
	int n_fields;
	char tmp[2];
	int i;
	HFILE *hf = (HFILE*)malloc(sizeof(HFILE));

	if (hf==NULL){
		printf("Not enough memory.");
		exit(1);
	}
	FILE *fp=fopen(path,"r");
	if (fp==NULL){
		printf("fail to open file.");
		exit(1);
	}
	fread(&n_records, sizeof(long),1,fp);
	

	hf->n_records=n_records;

	fread(&n_fields, sizeof(int),1,fp);
	hf->n_fields=n_fields;
	hf->schema=(char**)malloc(n_fields*sizeof(char*));

	for(i=0;i<n_fields;i++){
		fread(tmp, sizeof(char),2,fp);
		hf->schema[i]=(char*)calloc(5, sizeof(char));
		if (tmp[0] !='c') {
			hf->schema[i][0]=tmp[0];
			hf->schema[i][1]=tmp[1];
			hf->schema[i][2]=0;
		} else {
			hf->schema[i][0]=tmp[0];
			sprintf( tmp+1, "%d", tmp[1] );
		}

	}
	hf->path=path;
	//TODO:data read.
	//test
	
	return hf;
	
	
}

void hf_close(HFILE *hf){
//TODO: free substructure first.
	free(hf);
}

int hf_fields(HFILE *hfp){
	return hfp->n_fields;
}

char *hf_field(HFILE *hfp, int field){
	return hfp->schema[field-1];
}

int hf_record_length(HFILE *hfp){
	int sum=0;
	int i;
	for (i=1;i<=hf_fields(hfp);i++){
		printf("ddd:%s\n",hf_field(hfp, i));
		if (!strcmp(hf_field(hfp, i),"i1"))
			sum+=1;
		else if (!strcmp(hf_field(hfp, i),"i2"))
			sum+=2;
		else if (!strcmp(hf_field(hfp, i),"i4"))
			sum+=4;
		else if (!strcmp(hf_field(hfp, i),"i8"))
			sum+=8;
		else if (!strcmp(hf_field(hfp, i),"r4"))
			sum+=4;
		else if (!strcmp(hf_field(hfp, i),"r8"))
			sum+=8;
		else sum += atoi(hf_field(hfp, i)+1);
	}
	return sum;

}

RID hf_insert(HFILE *hf, void *record){
	RID r;
	FILE *fp=fopen(hf->path,"a+");
	if (fp==NULL){
		printf("fail to create file.");
		exit(1);
	}
	fseek(fp,0,SEEK_END);
	r = ftell(fp);
	fwrite(record, hf_record_length(hf), 1, fp);
	fclose(fp);
	printf("rid:%lu\n",r);
	hf->n_records++;
	fseek(fp,0,SEEK_SET);
	fwrite(&hf->n_records, sizeof(long), 1, fp);
	return r;
}

int hf_record(HFILE *hf, RID id, void *record){
	FILE *fp=fopen(hf->path,"r");
	if (fp==NULL){
		printf("fail to create file.");
		exit(1);
	}
	fseek(fp,id,SEEK_SET);
	fread(record, hf_record_length(hf), 1, fp);
	printf("%s",(char*)record);
	fclose(fp);
	return 0;
}

