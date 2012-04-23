#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lh.h"
#include "hf.h"

/* field processing, compare value given by selection clause in command line
   with corresponding fields of records in the heap file
   return 0: equal
   return -1: record < value
   return 1: record > value
*/
int compare_i1(void *record, int offset, void *value, int length){
	char* target = record + offset;
	char x = target[0];
	char y = atoi((char*)value);

	return x < y ? -1 : x > y ? 1 : 0;
}

int compare_i2(void *record, int offset, void *value, int length){
	short* target = record + offset;
	short x = target[0];
	short y = atoi((char*)value);

	return x < y ? -1 : x > y ? 1 : 0;
}

int compare_i4(void *record, int offset, void *value, int length){
	int* target = record + offset;
	int x = target[0];
	int y = atoi((char*)value);

	return x < y ? -1 : x > y ? 1 : 0;
}

int compare_i8(void *record, int offset, void *value, int length){
	long  long* target = record + offset;
	long long x = target[0];
	long long y = atoi((char*)value);

	return x < y ? -1 : x > y ? 1 : 0;
}

int compare_cx(void *record, int offset, void *value, int length){
	char* target = record + offset;
	char x[length+1];
	char y[length+1];
	strncpy(x, target, length);
	x[length]=0;
	strncpy(y, (char *)value, length);
	y[length]=0;
	
	return strcmp(x,y);
}

int compare_r4(void *record, int offset, void *value, int length){	
	float* target = record + offset;
	float x = target[0];
	float y = atof((char*)value);

	return x < y ? -1 : x > y ? 1 : 0;
}

int compare_r8(void *record, int offset, void *value, int length){
	double* target = record + offset;
	double x = target[0];
	double y = atof((char*)value);

	return x < y ? -1 : x > y ? 1 : 0;
}


/* field processing, decode 
   read data at offset within heapfile 
   write it to buffer line
   return 0 if there is no segmentation fault
*/

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
	//printf("in decode:%d\n",i1);
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
	long long* target = record + *offset;
	long long i1 = target[0];
	sprintf(line,"%lld",i1);
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
	target = record + *offset;
	memcpy(line, target, length);
	line[length]=0;
	*offset = *offset+length;

	return 0;
}


/* 
   field processing, encode 
   write the parsed string line to certain field of a record in heapfile
   specified by the offset
   return 0 if there is no segmentation fault
*/

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
	long long tmp;
	tmp = atoi(line);
	long long * target;
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


/* 
   compare schema a with schema b, given the same number of fields "length"
   return 1: not match
   return 0: match
*/
int compareschema(char  **a, char **b, int length) {
	int i;

	for(i=0;i<length;i++){
		if (b[i][strlen(b[i])-1]==13)
			b[i][strlen(b[i])-1]=0;
		if (strcmp(a[i],b[i])!=0) 
			return 1;

	}

	return 0;
}


/* 
   given a schema and the number of fields in schema
   create a heapfile under specified path
*/
int hf_create(const char *path, char **schema, int schemac){
	HFILE hf;
	int i;
	hf.n_records = 0;
	hf.schema = schema;
	hf.n_fields = schemac;
	hf.content = NULL;
	hf.path = path;
	

	FILE *fp=fopen(path,"w+");
	if (fp==NULL){
		printf("Fail to create file.\n");
		exit(1);
	}
	// write the number of records and fields to heapfile
	fwrite(&hf.n_records,sizeof(long long),1,fp);
	fwrite(&hf.n_fields,sizeof(int),1,fp);
	// write the schema to heapfile
	for(i = 0; i < schemac; i++)
		if (schema[i][0] !='c'){
	
			fwrite(hf.schema[i],sizeof(char),2,fp);}
		else {
	
			char tmp;
			fwrite(hf.schema[i],sizeof(char),1,fp);
			tmp=atoi(hf.schema[i]+1);
			fwrite(&tmp,sizeof(char),1,fp);
		}

	fclose(fp);
	return 0;
}

/*
    open and return a heapfile given the path

*/
HFILE *hf_open(const char *path){
	int n_records;	
	int n_fields;
	char tmp[2];
	int i;
	HFILE *hf = (HFILE*)malloc(sizeof(HFILE));
	char *main_index_file;
	char *overflow_index_file;
	if (hf==NULL){
		printf("Not enough memory.\n");
		exit(1);
	}

	FILE *fp=fopen(path,"r");
	if (fp==NULL){
		printf("Fail to open heap file.\n");
		return NULL;
	}
	
	// read the number of records
	fread(&n_records, sizeof(long long),1,fp);
	hf->n_records=n_records;

        // read the number of fields
	fread(&n_fields, sizeof(int),1,fp);
	hf->n_fields=n_fields;

        // read the schema
        // field processing
        // map schema to "schema_array" based on different data types
        // "schema_array" determines accesses to different read/write/compare functions  
	hf->schema=(char**)malloc(n_fields*sizeof(char*));
	hf->schema_array=(int*)malloc(n_fields*sizeof(int));
	hf->index_array=(int*)malloc(n_fields*sizeof(int));
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
			else if (tmp[0]=='r' && tmp[1]=='8')
				hf->schema_array[i]=5;
		} else {
			sprintf(hf->schema[i], "c%d", tmp[1]);
			hf->schema_array[i]=6;
		}

	}
	hf->path=path;

	int length = strlen(hf->path)+10;
	main_index_file = calloc(length,sizeof(char));
	overflow_index_file = calloc(length,sizeof(char));

	for(i=0;i<n_fields;i++){
		FILE *fp1 , *fp2;
		
		get_index_file_name(hf->path, i+1, main_index_file, overflow_index_file);

		fp1 = fopen(main_index_file,"r");
		fp2 = fopen(overflow_index_file,"r");
		if (fp1 != NULL && fp2 != NULL){
			hf->index_array[i]=1;
			fclose(fp1);
			fclose(fp2);

		} else {
			hf->index_array[i]=0;
		}

		//free(main_index_file);

		//free(overflow_index_file);

		
	}
	fclose(fp);
	//free(main_index_file);
	//free(overflow_index_file);

	return hf;
	
	
}

/* 
     close heapfile and free memory allocation
*/
void hf_close(HFILE *hf){

	int i;
	for (i=0;i<hf->n_fields;i++)
		free(hf->schema[i]);
	free(hf->schema);
	free(hf->schema_array);
	free(hf);
        return;
}


/*
     return number of fields in a heapfile
*/
int hf_fields(HFILE *hfp){
	return hfp->n_fields;
}


/*
     return a specific field
*/
char *hf_field(HFILE *hfp, int field){
	return hfp->schema[field-1];
}


/*
     return the total record length
*/
int hf_record_length(HFILE *hfp){
	int sum=0;
	int i;
	for (i=1;i<=hf_fields(hfp);i++){
		
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


/*
	insert a record in the heap file and return its RID
*/
RID hf_insert(HFILE *hf, void *record){
	RID r;
	//long tmp;
	FILE *fp=fopen(hf->path,"r+");
	if (fp==NULL){
		printf("Fail to open file.\n");
		exit(1);
	}

        // move cursor to the end of file 
        // append the record, n_records++
	fseek(fp,0,SEEK_END);
	r = ftell(fp);
	fwrite(record, hf_record_length(hf), 1, fp);
	hf->n_records++;

	// draw back the cursor to the beginning of file
        // overwrite the number of records to update
	fseek(fp,0,SEEK_SET);
	fwrite(&(hf->n_records), sizeof(long long), 1, fp);
	fclose(fp);

	return r;
}


/*
	read a record by giving the RID
        The RID id is used as the offset
*/
int hf_record(HFILE *hf, RID id, void *record){
	FILE *fp=fopen(hf->path,"r");
	if (fp==NULL){
		printf("fail to create file.");
		exit(1);
	}
	fseek(fp,id,SEEK_SET);
	fread(record, hf_record_length(hf), 1, fp);
	
	fclose(fp);
	return 0;
}


/*
	return the total number of records in the file
*/
long long hf_records(HFILE *hf){
	return hf->n_records;
}


/*
	start a scan (cursor) in the file
*/
CURSOR *hf_scan_open(HFILE *hf, COND* con, int conditions){
	CURSOR *cur = (CURSOR*)malloc(sizeof(CURSOR));
	int i;
	cur->hf = hf;
	cur->con = con;
	cur->conditions = conditions;
        // set the initial position of the cursor
        // point to the first record
        // we use 2-byte for the storage of each field in schema
	cur->current = sizeof(long long)+sizeof(int)+2*hf->n_fields;
	cur->index = -1;
	for (i=0;i<conditions;i++){

		if (strcmp(con[i].op,"=")==0 && hf->index_array[con[i].field-1]==1){

			cur->index = i;
			break;
		}
	}
	cur->index_mo = 0;
	cur->index_current_bucket = -1;
	cur->index_bucket_offset = 12;
	return cur;
}


/*
	close cursor and free memory allocation
*/
void hf_scan_close(CURSOR *cur){
	
	int i;
	for(i=0;i<cur->conditions;i++){
		free((void *)cur->con[i].op);
		free(cur->con[i].value);
	}
	free(cur->con);
	free(cur);
        return;
}


/*
	calculate the offset within a row of a particular field (column)
        according to schema
        return: offset of a particular field	
*/
int calculate_offset(char **schema, int field){
	int offset = 0;
	int i;
	
	for(i=1; i<field; i++){
	
		offset += atoi(schema[i-1]+1);
	}
	
	return offset;
}


/*
	filter records according to conditions in a heap file
        return 0: record gets filtered off
        return 1: record meets the conditions
*/
int filter(void *record, COND *con, int conditions, HFILE *hf){
	int i;
	
	for (i=0;i<conditions;i++){
		// prepare parameters for function *compare
		int offset = calculate_offset(hf->schema, con[i].field);
                int column = (con[i].field)-1;
		int function_num = hf->schema_array[column];
		int field_length = atoi((hf->schema[column])+1);
		
		// compare the value in condition clause with corresponding field in records
		int result = (*compare[function_num])(record, offset, con[i].value, field_length);
		
		if (!strcmp(con[i].op,"=") && result !=0)
			return 0;
		else if (!strcmp(con[i].op,">") && result <=0)
			return 0;
		else if (!strcmp(con[i].op,"<") && result >=0)
			return 0;
		else if (!strcmp(con[i].op,">=") && result <0)
			return 0;
		else if (!strcmp(con[i].op,"<=") && result >0)
			return 0;
		else if (!strcmp(con[i].op,"<>") && result ==0)
			return 0;
	}

	return 1;
}


/*
	get the next entry of a scan
        returns the RID of the next record that meets all the conditions
	returns (rid_t) -1 in case we are out of records
*/
RID hf_scan_next(CURSOR *cur, void *record){
	if (cur->index == -1){

		long long schema_space = 2*cur->hf->n_fields;
   	     long long record_space = (cur->hf->n_records) * hf_record_length(cur->hf);
		long long file_size = sizeof(long long) + sizeof(int) + schema_space + record_space;
		
		if (cur->current >= file_size)
			return -1;

		hf_record(cur->hf, cur->current, record);
	
		cur->current += hf_record_length(cur->hf);
     
   	     // if the current record doesn't meet all the conditions, fetch and check the next
		while (filter(record, cur->con, cur->conditions, cur->hf) == 0){
			
			if (cur->current >= file_size)
				return -1;
			hf_record(cur->hf, cur->current, record);
				
			cur->current += hf_record_length(cur->hf);
		}
			
		return cur->current - hf_record_length(cur->hf);
	} else {

		INDEX_FILE *index_file;
		index_file = open_index(cur->con[cur->index].field-1, cur->hf);
		void *buf2 = calloc(105,sizeof(char));
		int zero = 0;
		if (buf2==NULL){
			printf("Malloc failed");
			exit(1);
		} 
		if (cur->index_current_bucket == -1){

			(*encode[cur->hf->schema_array[cur->con[cur->index].field-1]])(buf2, (char*)cur->con[cur->index].value, &zero,atoi((cur->hf->schema[cur->con[cur->index].field-1])+1));
			zero = 0;
			cur->index_current_bucket = (*calculate_index[cur->hf->schema_array[cur->con[cur->index].field-1]])(buf2,index_file->s,index_file->n,0);
		}
		int i = cur->con[cur->index].field-1;
	
		int original_offset;
		int original_usage;
		void *original_bucket = calloc(BSIZE,sizeof(char));
		//void *bucket;
		//bucket = calloc(BSIZE,sizeof(char));
		if (original_bucket==NULL){
			printf("Malloc failed");
			exit(1);
		}
		void *buf = calloc(105,sizeof(char));
		if (buf==NULL){
			printf("Malloc failed");
			exit(1);
		}
		if (cur->index_mo == 0){
			read_bucket(original_bucket, cur->index_current_bucket, index_file->main_path);
		} else {
			read_bucket(original_bucket, cur->index_current_bucket, index_file->overflow_path);
		}
		original_usage = ((int*)original_bucket)[0];
		for (original_offset = cur->index_bucket_offset; original_offset < original_usage; original_offset += atoi((cur->hf->schema[i])+1)+sizeof(long long)){
			RID rid = (*read_keypair[cur->hf->schema_array[i]])(original_bucket, buf, original_offset, atoi((cur->hf->schema[i])+1));
			hf_record(cur->hf, rid, record);
			if (filter(record, cur->con, cur->conditions, cur->hf) == 1){
				cur->index_bucket_offset = original_offset + atoi((cur->hf->schema[i])+1)+sizeof(long long);
				free(original_bucket);
				free(buf);
				return rid;
			}
		}
		while (((long long*)(original_bucket+4))[0]!=-1){
			int current_bucket = ((long long*)(original_bucket+4))[0];
			read_bucket(original_bucket, ((long long*)(original_bucket+4))[0], index_file->overflow_path);
			original_usage = ((int*)original_bucket)[0];
			for (original_offset = 12; original_offset < original_usage; original_offset += atoi((cur->hf->schema[i])+1)+sizeof(long long)){
				RID rid = (*read_keypair[cur->hf->schema_array[i]])(original_bucket, buf, original_offset, atoi((cur->hf->schema[i])+1));
				hf_record(cur->hf, rid, record);
				if (filter(record, cur->con, cur->conditions, cur->hf) == 1){
					cur->index_bucket_offset = original_offset + atoi((cur->hf->schema[i])+1)+sizeof(long long);
					cur->index_current_bucket = current_bucket;
					cur->index_mo = 1;
					free(original_bucket);
					free(buf);
					return rid;
				}
			}
		}
		free(original_bucket);
		free(buf);
		free(buf2);
		return -1;
	}
}























