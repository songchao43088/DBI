#include "hf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int decode_i1(void *record, char *line, int *offset, int length){
	char* target = record + *offset;
	char i1 = target[0];
	sprintf(line,"%d",i1);
	*offset = *offset+length;
	return 0;
}

int decode_i2(void *record, char *line, int *offset, int length){
	short* target = record + *offset;
	short i1 = target[0];
	sprintf(line,"%d",i1);
	*offset = *offset+length;
	return 0;
}

int decode_i4(void *record, char *line, int *offset, int length){
	int* target = record + *offset;
	int i1 = target[0];
	sprintf(line,"%d",i1);
	*offset = *offset+length;
	return 0;
}

int decode_i8(void *record, char *line, int *offset, int length){
	long* target = record + *offset;
	long i1 = target[0];
	sprintf(line,"%lu",i1);
	*offset = *offset+length;
	return 0;
}

int decode_r4(void *record, char *line, int *offset, int length){
	float* target = record + *offset;
	float i1 = target[0];
	sprintf(line,"%f",i1);
	*offset = *offset+length;
	return 0;
}

int decode_r8(void *record, char *line, int *offset, int length){
	double* target = record + *offset;
	double i1 = target[0];
	sprintf(line,"%f",i1);
	*offset = *offset+length;
	return 0;
}

int decode_cx(void *record, char *line, int *offset, int length){
	char * target;
//printf("yyy\n");
	target = record + *offset;
//printf("xxx\n");
	memcpy(line, target, length);
//printf("zzz\n");
	line[length]=0;
	*offset = *offset+length;
	return 0;
}

int encode_i1(void *record, char *line, int *offset, int length){
	char tmp;
	tmp = atoi(line);
	((char *)record)[*offset]=tmp;
	*offset = *offset+length;
	return 0;
}

int encode_i2(void *record, char *line, int *offset, int length){
	short tmp;
	tmp = atoi(line);
	
	short * target;
	target = record + *offset;
	target[0]=tmp;
printf("print short %d\n",target[0]);
	*offset = *offset+length;
	return 0;
}
int encode_i4(void *record, char *line, int *offset, int length){
	int tmp;
	tmp = atoi(line);
	int * target;
	target = record + *offset;
	target[0]=tmp;
	*offset = *offset+length;
	return 0;
}
int encode_i8(void *record, char *line, int *offset, int length){
	long tmp;
	tmp = atoi(line);
	long * target;
	target = record + *offset;
	target[0]=tmp;
	*offset = *offset+length;
	return 0;
}
int encode_r4(void *record, char *line, int *offset, int length){
	float tmp;
	tmp = atof(line);
	float * target;
	target = record + *offset;
	target[0]=tmp;
	*offset = *offset+length;
	return 0;
}
int encode_r8(void *record, char *line, int *offset, int length){
	double tmp;
	tmp = atof(line);
	double * target;
	target = record + *offset;
	target[0]=tmp;
	*offset = *offset+length;
	return 0;
}
int encode_cx(void *record, char *line, int *offset, int length){
	char * target;
	target = record + *offset;
	if (strlen(line)<length)
		strncpy(target, line, strlen(line)+1);
	else
		strncpy(target, line, strlen(line));
	*offset = *offset+length;
	return 0;
}


int compareschema(char  **a, char **b, int length) {
	int i;
	for(i=0;i<length;i++){
		if (strcmp(a[i],b[i])!=0) {
			printf("%d vs %d\n",a[i][2],b[i][2]);
			return 1;}
	}
	return 0;
}


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
	FILE *fp=fopen(path,"rd");
	if (fp==NULL){
		printf("fail to open file.");
		return NULL;
	}
	printf(" i am at %lu\n",ftell(fp));
	fread(&n_records, sizeof(long),1,fp);
	

	hf->n_records=n_records;
printf("\nNo.of R:%lu\n",hf->n_records);
	fread(&n_fields, sizeof(int),1,fp);
	hf->n_fields=n_fields;
	hf->schema=(char**)malloc(n_fields*sizeof(char*));
	hf->schema_array=(int*)malloc(n_fields*sizeof(int));
	for(i=0;i<n_fields;i++){
		fread(tmp, sizeof(char),2,fp);
		hf->schema[i]=(char*)calloc(5, sizeof(char));
		if (tmp[0] !='c') {
			hf->schema[i][0]=tmp[0];
			hf->schema[i][1]=tmp[1];
			hf->schema[i][2]=0;
			if (tmp[0]=='i' && tmp[1]=='1')
				hf->schema_array[i]=0;
			else if (tmp[0]=='i' && tmp[1]=='2')
				hf->schema_array[i]=1;
			else if (tmp[0]=='i' && tmp[1]=='4')
				hf->schema_array[i]=2;
			else if (tmp[0]=='i' && tmp[1]=='8')
				hf->schema_array[i]=3;
			else if (tmp[0]=='r' && tmp[1]=='4')
				hf->schema_array[i]=4;
			else if (tmp[0]=='i' && tmp[1]=='8')
				hf->schema_array[i]=5;
		} else {
			sprintf(hf->schema[i], "c%d", tmp[1]);
			hf->schema_array[i]=6;
		}

	}
	hf->path=path;
	//TODO:data read.
	//test
	fclose(fp);
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
		//printf("ddd:%s\n",hf_field(hfp, i));
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
	long tmp;
	FILE *fp=fopen(hf->path,"r+");
	if (fp==NULL){
		printf("fail to create file.");
		exit(1);
	}
	fseek(fp,0,SEEK_END);
printf(" i am at the end%lu\n",ftell(fp));
	r = ftell(fp);
	fwrite(record, hf_record_length(hf), 1, fp);
	
	printf("rid:%lu\n",r);
	hf->n_records++;
	printf("%lu...",hf->n_records);
	fseek(fp,0,SEEK_SET);
printf(" i am at begin%lu\n",ftell(fp));
	fread(&tmp, sizeof(long), 1, fp);
	printf("%lu...***",tmp);
	fseek(fp,0,SEEK_SET);

	fwrite(&(hf->n_records), sizeof(long), 1, fp);
	fclose(fp);
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
	//printf("%s",(char*)record);
	fclose(fp);
	return 0;
}

long hf_records(HFILE *hf){
	return hf->n_records;
}





























