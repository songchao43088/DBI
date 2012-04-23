#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lh.h"

long long calculate_index_i1(void *record, long long s, long long n, int offset){
	char k = ((char*)(record+offset))[0];
	long long b;
	b = k % (2*s);
	if (b > n-1){
		b = b - s;
	} 

	return b;
}


long long calculate_index_i2(void *record, long long s, long long n, int offset){
	short k = ((short*)(record+offset))[0];
	long long b;
	b = k % (2*s);
	if (b > n-1){
		b = b - s;
	} 

	return b;
}

long long calculate_index_i4_r4(void *record, long long s, long long n, int offset){
	int k = ((int*)(record+offset))[0];
	long long b;
	b = k % (2*s);
	if (b > n-1){
		b = b - s;
	} 

	return b;
}

long long calculate_index_i8_r8(void *record, long long s, long long n, int offset){
	long long k = ((long long*)(record+offset))[0];
	long long b;
	b = k % (2*s);
	if (b > n-1){
		b = b - s;
	} 

	return b;
}


long long calculate_index_cx(void *record, long long s, long long n, int offset){
	int sum = 0;
	int i;
	long long b;
	for(i=0;i<strlen((char*)(record+offset));i++){
		sum += ((char*)(record+offset))[i];
	}
	b = sum % (2*s);
	if (b > n-1){
		b = b - s;
	} 

	return b;
}


void write_bucket(void *bucket, int bid, char *path){
	FILE *fp;
	fp = fopen(path, "r+");
	if(fp == NULL){
		printf("fail to open and write.\n");
		exit(1);
	}
	if (path[strlen(path)-1]=='t'){
		fseek(fp,bid*BSIZE,SEEK_SET);
	} else {
		fseek(fp,bid*BSIZE+5*sizeof(long long),SEEK_SET);
	}
		fwrite(bucket, sizeof(char), BSIZE, fp);
	fclose(fp);
}

void read_bucket(void *bucket, int bid, char *path){
	FILE *fp;
	fp = fopen(path, "r");
	if(fp == NULL){
		printf("fail to open.\n");
		exit(1);
	}
	if (path[strlen(path)-1]=='t'){
		fseek(fp,bid*BSIZE,SEEK_SET);
	} else {
		fseek(fp,bid*BSIZE+5*sizeof(long long),SEEK_SET);
	}
		fread(bucket, sizeof(char), BSIZE, fp);
	fclose(fp);
}


void get_index_file_name(const char* path, int i,  char *main_index_file,  char *overflow_index_file){

	char buf[10];
	sprintf(buf,"%d",i);
	
	strncpy(main_index_file, path, strlen(path)-2);
	main_index_file[strlen(path)-2]=0;
	strncpy(overflow_index_file, path, strlen(path)-2);
	overflow_index_file[strlen(path)-2]=0;
	strcat(main_index_file, buf);
	strcat(main_index_file, ".lht");

	strcat(overflow_index_file, buf);
	strcat(overflow_index_file, ".lho");

	return;
}

void init_index(int i, HFILE *hf){
	int j;
	INDEX_FILE indexfile;
	long long bigzero = 0;
	long long bignull = -1;
	FILE *fp1, *fp2;
	void* bucket;
	indexfile.next=0;
	indexfile.s=4;
	indexfile.n=4;
	int length = strlen(hf->path)+10;
	indexfile.main_path = calloc(length,sizeof(char));
	indexfile.overflow_path = calloc(length,sizeof(char));
	get_index_file_name(hf->path, i+1, indexfile.main_path, indexfile.overflow_path);
	indexfile.type = hf->schema[i];
	indexfile.column_number=i;
	fp1 = fopen(indexfile.main_path,"w");
	if (fp1==NULL){
		printf("fail to create file.");
		exit(1);
	}
	fp2 = fopen(indexfile.overflow_path,"w");
	if (fp2==NULL){
		printf("fail to create file.");
		exit(1);
	}
	bucket = calloc(BSIZE,sizeof(char));
	if (bucket==NULL){
		printf("Malloc failed");
		exit(1);
	}
	init_bucket(bucket);
	
	fclose(fp1);
	for(j=0;j<4;j++){
		write_bucket(bucket, j, indexfile.main_path);
		}	
	fwrite(&indexfile.next, sizeof(long long), 1, fp2);
	fwrite(&indexfile.s, sizeof(long long), 1, fp2);
	fwrite(&indexfile.n, sizeof(long long), 1, fp2);
	fwrite(&bignull, sizeof(long long), 1, fp2);
	fwrite(&bigzero, sizeof(long long), 1, fp2);
	free(bucket);
	fclose(fp2);
	return;
}

INDEX_FILE *open_index(int i, HFILE *hf){
	INDEX_FILE *indexfile;
	FILE *fp1, *fp2;
	indexfile=(INDEX_FILE*)malloc(sizeof(INDEX_FILE));
	int length = strlen(hf->path)+10;
	indexfile->main_path = calloc(length,sizeof(char));
	indexfile->overflow_path = calloc(length,sizeof(char));
	get_index_file_name(hf->path, i+1, indexfile->main_path, indexfile->overflow_path);
	indexfile->type = hf->schema[i];
	indexfile->column_number=i;
	fp1 = fopen(indexfile->main_path,"r");
	if (fp1==NULL){
		printf("fail to open file.");
		exit(1);
	}
	fp2 = fopen(indexfile->overflow_path,"r");
	if (fp2==NULL){
		printf("fail to open file.");
		exit(1);
	}
	fread(&indexfile->next, sizeof(long long), 1, fp2);
	fread(&indexfile->s, sizeof(long long), 1, fp2);
	fread(&indexfile->n, sizeof(long long), 1, fp2);
	
	fclose(fp1);
	fclose(fp2);
	return indexfile;
}

void close_index(HFILE *hf, int i, INDEX_FILE *index_file){
	FILE *fp2;
	fp2 = fopen(index_file->overflow_path,"r+");
	fwrite(&index_file->next, sizeof(long long), 1, fp2);
	fwrite(&index_file->s, sizeof(long long), 1, fp2);
	fwrite(&index_file->n, sizeof(long long), 1, fp2);
	fclose(fp2);
	print_index(hf, i, index_file);
	free(index_file->main_path);
	free(index_file->overflow_path);
	free(index_file);
	return;
}

void update_bucket_i1(void *bucket, void *record, RID rid, int size, int offset){
	int usage = ((int*)bucket)[0];
	void *current_position=bucket+usage;
	((char*)current_position)[0]=((char*)(record+offset))[0];
	((RID*)(current_position+1))[0]=rid;
	((int*)bucket)[0]=usage + size;
}

void update_bucket_i2(void *bucket, void *record, RID rid, int size, int offset){
	int usage = ((int*)bucket)[0];
	void *current_position=bucket+usage;
	((short*)current_position)[0]=((short*)(record+offset))[0];
	((RID*)(current_position+2))[0]=rid;
	((int*)bucket)[0]=usage + size;
}

void update_bucket_i4(void *bucket, void *record, RID rid, int size, int offset){
	int usage = ((int*)bucket)[0];
	void *current_position=bucket+usage;
	((int*)current_position)[0]=((int*)(record+offset))[0];
	((RID*)(current_position+4))[0]=rid;
	((int*)bucket)[0]=usage + size;
}

void update_bucket_i8(void *bucket, void *record, RID rid, int size, int offset){
	int usage = ((int*)bucket)[0];
	void *current_position=bucket+usage;
	((long long*)current_position)[0]=((long long*)(record+offset))[0];
	((RID*)(current_position+8))[0]=rid;
	((int*)bucket)[0]=usage + size;
}

void update_bucket_r4(void *bucket, void *record, RID rid, int size, int offset){
	int usage = ((int*)bucket)[0];
	void *current_position=bucket+usage;
	((float*)current_position)[0]=((float*)(record+offset))[0];
	((RID*)(current_position+4))[0]=rid;
	((int*)bucket)[0]=usage + size;
}

void update_bucket_r8(void *bucket, void *record, RID rid, int size, int offset){
	int usage = ((int*)bucket)[0];
	void *current_position=bucket+usage;
	((double*)current_position)[0]=((double*)(record+offset))[0];
	((RID*)(current_position+8))[0]=rid;
	((int*)bucket)[0]=usage + size;
}

void update_bucket_cx(void *bucket, void *record, RID rid, int size, int offset){
	int usage = ((int*)bucket)[0];
	void *current_position=bucket+usage;
	strncpy((char*)current_position, (char*)(record+offset),size-sizeof(long long));
	((RID*)(current_position+size-sizeof(long long)))[0]=rid;
	((int*)bucket)[0]=usage + size;
}

void hash_record(HFILE *hf, void* record, int i, RID rid, INDEX_FILE *index_file){
	//void *buf;
	int offset=0;
	int j;
	long long hash_index;
	long long bid;
	int split;
	int mo;
	void *bucket;
	char *path;

	bucket = calloc(BSIZE,sizeof(char));
	if (bucket==NULL){
		printf("Malloc failed");
		exit(1);
	}
	//buf = malloc(105*sizeof(char));
	/*if (buf==NULL){
		printf("Malloc failed");
		exit(1);
	}*/
	for(j=0;j<i;j++){
		offset+=atoi((hf->schema[j])+1);
	}

	//(*decode[hf->schema_array[i]])(record, buf, &offset,atoi((hf->schema[i])+1));
	hash_index = (*calculate_index[hf->schema_array[i]])(record,index_file->s,index_file->n,offset);

	split = find_bucket(hash_index, &bid, &mo, index_file);

	if (mo==0){
		path = index_file->main_path;
	} else {
		path = index_file->overflow_path;
	}
	read_bucket(bucket, bid, path);

	(*update_bucket[hf->schema_array[i]])(bucket,record,rid,atoi((hf->schema[i])+1)+sizeof(long long),offset);

	write_bucket(bucket, bid, path);

	if (split==1){
		rehash(hf, i, index_file);
	}

	free(bucket);
	//free(buf);
}

int find_bucket(long long hash_index, long long *bid, int*mo, INDEX_FILE *index_file){
	void *bucket;
	int tmp_bid;
	bucket = calloc(BSIZE,sizeof(char));
	if (bucket==NULL){
		printf("Malloc failed");
		exit(1);
	}
	read_bucket(bucket, hash_index, index_file->main_path);
	if (((long long*)(bucket+4))[0]==-1){
		if (((int*)bucket)[0] < BSIZE-sizeof(long long)-atoi((index_file->type)+1)) {
			*bid = hash_index;
			*mo = 0;
			free(bucket);
			return 0;
		} else {

			*bid = create_overflow_bucket(index_file);
			mylink(hash_index, 0, *bid, index_file);
			*mo = 1;

			free(bucket);
			return 1;
		}
	} else {
		long long next_bid =  ((long long*)(bucket+4))[0];
        	do
		{
			read_bucket(bucket, next_bid, index_file->overflow_path);
			tmp_bid = next_bid;
			next_bid = ((long long*)(bucket+4))[0];	
		}
		while(next_bid != -1);
		if (((int*)bucket)[0] < BSIZE-sizeof(long long)-atoi((index_file->type)+1)) {
			*bid = tmp_bid;			
			*mo = 1;
			free(bucket);
			return 0;
		} else {
			*bid = create_overflow_bucket(index_file);
			mylink(tmp_bid, 1, *bid, index_file);
			*mo = 1;
			free(bucket);
			return 1;
		}
	}
}

void mylink(long long from, int frommo, long long to, INDEX_FILE *index_file){
	void *bucket;
	char *path;
	bucket = calloc(BSIZE,sizeof(char));
	if (bucket==NULL){
		printf("Malloc failed");
		exit(1);
	}
        if(frommo == 0){
	path = index_file->main_path;
	read_bucket(bucket, from, path);
        }else{
	path = index_file->overflow_path;
	read_bucket(bucket, from, path);
	}
	((long long*)(bucket+4))[0]=to;
	write_bucket(bucket, from, path);
	free(bucket);
	return;
}

long long create_overflow_bucket(INDEX_FILE *index_file){
	FILE *fp;
	long long free_head;
	void * bucket;
	long long n_buckets;
	bucket = calloc(BSIZE,sizeof(char));
	if (bucket==NULL){
		printf("Malloc failed");
		exit(1);
	}
	fp=fopen(index_file->overflow_path,"r+");
	if (fp == NULL){
		printf("fail to open file\n");
		exit(1);	
	}
	fseek(fp, 3*sizeof(long long), SEEK_SET);
	fread(&free_head, sizeof(long long),1,fp);
	fread(&n_buckets, sizeof(long long),1,fp);
	if (free_head == -1){
		init_bucket(bucket);
		write_bucket(bucket, n_buckets, index_file->overflow_path);
		n_buckets++;
		fseek(fp, 4*sizeof(long long), SEEK_SET);
		fwrite(&n_buckets, sizeof(long long),1,fp);
		fclose(fp);
		free(bucket);

		return n_buckets-1;
		//allocate new
	} else {
		read_bucket(bucket, free_head, index_file->overflow_path);
		long long next_free = ((long long*)(bucket+4))[0];
		fseek(fp, 3*sizeof(long long), SEEK_SET);
		fwrite(&next_free, sizeof(long long), 1, fp);
		init_bucket(bucket);
		write_bucket(bucket, free_head, index_file->overflow_path);
		fclose(fp);
		free(bucket);

		return free_head;
		
	}
}

void init_bucket(void *bucket){
	long long bignull = -1;
	((int*)bucket)[0]=12;
	((long long*)(bucket+4))[0]=bignull;
	return;
}

void rehash(HFILE *hf, int i, INDEX_FILE *index_file){
	void *bucket;
	bucket = calloc(BSIZE,sizeof(char));
	if (bucket==NULL){
		printf("Malloc failed");
		exit(1);
	}
	init_bucket(bucket);
	
	write_bucket(bucket, index_file->n, index_file->main_path);

	rehash_keys(hf, i, index_file);
	index_file->n++;
	index_file->next++;
	if (index_file->next == index_file-> s){
		index_file->next = 0;
		index_file->s *= 2;
	}
	free(bucket);
	
}

void rehash_keys(HFILE *hf, int i, INDEX_FILE *index_file){
	void *original_bucket;
	void *old_bucket;
	void *new_bucket;
	void *buf;
	int original_offset=12;
	int original_usage;
	int old_bucket_write_position = -1;
	int new_bucket_write_position = -1;
	int free_start_point;
	
	original_bucket = calloc(BSIZE,sizeof(char));
	if (original_bucket==NULL){
		printf("Malloc failed");
		exit(1);
	}
	old_bucket = calloc(BSIZE,sizeof(char));
	if (old_bucket==NULL){
		printf("Malloc failed");
		exit(1);
	}
	new_bucket = calloc(BSIZE,sizeof(char));
	if (new_bucket==NULL){
		printf("Malloc failed");
		exit(1);
	}
	buf = calloc(105,sizeof(char));
	if (buf==NULL){
		printf("Malloc failed");
		exit(1);
	}
	read_bucket(original_bucket, index_file->next, index_file->main_path);
	original_usage = ((int*)original_bucket)[0];
	read_bucket(old_bucket, index_file->next, index_file->main_path);
	((int*)old_bucket)[0]=12;

	read_bucket(new_bucket, index_file->n, index_file->main_path);
	for (original_offset = 12; original_offset < original_usage; original_offset += atoi((hf->schema[i])+1)+sizeof(long long)){
		RID rid = (*read_keypair[hf->schema_array[i]])(original_bucket, buf, original_offset, atoi((hf->schema[i])+1));
		long long new_index = (*calculate_index[hf->schema_array[i]])(buf,index_file->s,index_file->n+1, 0);
		if (new_index==index_file->next){//old
			if (((int*)old_bucket)[0] >= BSIZE-sizeof(long long)-atoi((index_file->type)+1)){//full, allocate new		
				if (old_bucket_write_position == -1){
					write_bucket(old_bucket, index_file->next, index_file->main_path);
				} else {
					write_bucket(old_bucket, old_bucket_write_position, index_file->overflow_path);
				}
				old_bucket_write_position = ((long long*)(old_bucket+4))[0];
				read_bucket(old_bucket, old_bucket_write_position, index_file->overflow_path);
				((int*)old_bucket)[0]=12;	
			}

			(*update_bucket[hf->schema_array[i]])(old_bucket,buf,rid,atoi((hf->schema[i])+1)+sizeof(long long),0);

		} else { //new
			if (((int*)new_bucket)[0] >= BSIZE-sizeof(long long)-atoi((index_file->type)+1)){//full, allocate new		
				if (new_bucket_write_position == -1){
					write_bucket(new_bucket, index_file->n, index_file->main_path);
					new_bucket_write_position = create_overflow_bucket(index_file);
					mylink(index_file->n, 0, new_bucket_write_position, index_file);
				} else {
					RID tmp = new_bucket_write_position;
					write_bucket(new_bucket, new_bucket_write_position, index_file->overflow_path);
					new_bucket_write_position = create_overflow_bucket(index_file);
					mylink(tmp, 1, new_bucket_write_position, index_file);
				}
				read_bucket(new_bucket, new_bucket_write_position, index_file->overflow_path);	
			}
			(*update_bucket[hf->schema_array[i]])(new_bucket,buf,rid,atoi((hf->schema[i])+1)+sizeof(long long),0);
		}
	}
	while (((long long*)(original_bucket+4))[0]!=-1){
		read_bucket(original_bucket, ((long long*)(original_bucket+4))[0], index_file->overflow_path);
		original_usage = ((int*)original_bucket)[0];
		for (original_offset = 12; original_offset < original_usage; original_offset += atoi((hf->schema[i])+1)+sizeof(long long)){
			RID rid = (*read_keypair[hf->schema_array[i]])(original_bucket, buf, original_offset, atoi((hf->schema[i])+1));
			long long new_index = (*calculate_index[hf->schema_array[i]])(buf,index_file->s,index_file->n+1, 0);
			if (new_index==index_file->next){//old
				if (((int*)old_bucket)[0] >= BSIZE-sizeof(long long)-atoi((index_file->type)+1)){//full, allocate new		
					if (old_bucket_write_position == -1){
						write_bucket(old_bucket, index_file->next, index_file->main_path);
					} else {
						write_bucket(old_bucket, old_bucket_write_position, index_file->overflow_path);
					}
					old_bucket_write_position = ((long long*)(old_bucket+4))[0];

					read_bucket(old_bucket, old_bucket_write_position, index_file->overflow_path);
					((int*)old_bucket)[0]=12;	
				}
				(*update_bucket[hf->schema_array[i]])(old_bucket,buf,rid,atoi((hf->schema[i])+1)+sizeof(long long),0);
			} else { //new
				if (((int*)new_bucket)[0] >= BSIZE-sizeof(long long)-atoi((index_file->type)+1)){//full, allocate new		
					if (new_bucket_write_position == -1){
						write_bucket(new_bucket, index_file->n, index_file->main_path);
						new_bucket_write_position = create_overflow_bucket(index_file);
						mylink(index_file->n, 0, new_bucket_write_position, index_file);
					} else {
						RID tmp = new_bucket_write_position;
						write_bucket(new_bucket, new_bucket_write_position, index_file->overflow_path);
						new_bucket_write_position = create_overflow_bucket(index_file);
						mylink(tmp, 1, new_bucket_write_position, index_file);
					}
					read_bucket(new_bucket, new_bucket_write_position, index_file->overflow_path);	
				}
				(*update_bucket[hf->schema_array[i]])(new_bucket,buf,rid,atoi((hf->schema[i])+1)+sizeof(long long),0);
			}
		}
	}
	free_start_point = ((long long*)(old_bucket+4))[0];
	if (free_start_point != -1){

		int original_free_tail = find_free_list_tail(index_file);
		if (original_free_tail != -1) {
			mylink(original_free_tail, 1, free_start_point, index_file);
		} else {
			add_to_free_list(free_start_point, index_file);
		}
	}
	((long long*)(old_bucket+4))[0] = -1;
	if (old_bucket_write_position == -1){
		write_bucket(old_bucket, index_file->next, index_file->main_path);
	} else {
		write_bucket(old_bucket, old_bucket_write_position, index_file->overflow_path);
	}
	if (new_bucket_write_position == -1){
		write_bucket(new_bucket, index_file->n, index_file->main_path);
	} else {
		write_bucket(new_bucket, new_bucket_write_position, index_file->overflow_path);
	}
	
	free(original_bucket);
	free(old_bucket);
	free(new_bucket);
	free(buf);
}

RID read_keypair_i1(void *bucket, void *buf, int offset, int size){
	((char*)buf)[0] = ((char*)(bucket+offset))[0];
	return ((long long*)(bucket+offset+size))[0];
}

RID read_keypair_i2(void *bucket, void *buf, int offset, int size){
	((short*)buf)[0] = ((short*)(bucket+offset))[0];
	return ((long long*)(bucket+offset+size))[0];
}

RID read_keypair_i4(void *bucket, void *buf, int offset, int size){
	((int*)buf)[0] = ((int*)(bucket+offset))[0];
	return ((long long*)(bucket+offset+size))[0];
}

RID read_keypair_i8(void *bucket, void *buf, int offset, int size){
	((long long*)buf)[0] = ((long long*)(bucket+offset))[0];
	return ((long long*)(bucket+offset+size))[0];
}

RID read_keypair_r4(void *bucket, void *buf, int offset, int size){
	((float*)buf)[0] = ((float*)(bucket+offset))[0];
	return ((long long*)(bucket+offset+size))[0];
}

RID read_keypair_r8(void *bucket, void *buf, int offset, int size){
	((double*)buf)[0] = ((double*)(bucket+offset))[0];
	return ((long long*)(bucket+offset+size))[0];
}

RID read_keypair_cx(void *bucket, void *buf, int offset, int size){
	strncpy(buf,bucket+offset,size);
	return ((long long*)(bucket+offset+size))[0];
}

long long find_free_list_tail(INDEX_FILE *index_file){
	FILE *fp;
	long long free_head;
	void *bucket;
	long long last;
	bucket = calloc(BSIZE,sizeof(char));
	if (bucket==NULL){
		printf("Malloc failed");
		exit(1);
	}
	fp=fopen(index_file->overflow_path,"r");
	if (fp == NULL){
		printf("fail to open file\n");
		exit(1);	
	}
	fseek(fp, 3*sizeof(long long), SEEK_SET);
	fread(&free_head, sizeof(long long),1,fp);
	if (free_head == -1){
		fclose(fp);
		free(bucket);
		return -1;
	} else {
		do{
			read_bucket(bucket, free_head, index_file->overflow_path);
			last = free_head;
			free_head = ((long long*)(bucket+4))[0];
		} while (free_head != -1);
		fclose(fp);
		free(bucket);
		return last;
	}

}

void add_to_free_list(long long bid, INDEX_FILE *index_file){
	FILE *fp;
	fp=fopen(index_file->overflow_path,"r+");
	if (fp == NULL){
		printf("fail to open file\n");
		exit(1);	
	}
	fseek(fp, 3*sizeof(long long), SEEK_SET);
	fwrite(&bid, sizeof(long long),1,fp);
	fclose(fp);
	return;
}


void print_index(HFILE *hf, int i, INDEX_FILE *index_file){
	FILE *fp1, *fp2;
	fp1 = fopen(index_file->main_path,"r");
	fp2 = fopen(index_file->overflow_path,"r");
	void *bucket;
	bucket = calloc(BSIZE,sizeof(char));
	int iter;
	int j;
	long long free_head;
	long long n_buckets;
	int zero = 0;
 void *buf = calloc(105,sizeof(char));
	if (buf==NULL){
		printf("Malloc failed");
		exit(1);
	}
char *line = (char*)calloc(105,sizeof(char));
	if (line==NULL){
		printf("Malloc failed");
		exit(1);
	}
	fseek(fp2, 3*sizeof(long long), SEEK_SET);
	fread(&free_head, sizeof(long long),1,fp2);
	fread(&n_buckets, sizeof(long long),1,fp2);
	fclose(fp1);
	fclose(fp2);
	printf("basic info: next=%lld, n=%lld, s=%lld, free_list_header=%lld, number of buckets in overflow=%lld\n",index_file->next,index_file->n, index_file->s, free_head,n_buckets);
	printf("main bucket:\n");
	for (iter=0; iter <index_file->n; iter++){
		read_bucket(bucket, iter, index_file->main_path);
		int offset = ((int*)bucket)[0];
		long long pointer = ((long long*)(bucket+4))[0];
		printf("offset=%d, pointer=%lld\n",offset,pointer);
		printf("key-rid pairs:\n");
		
		int original_usage = ((int*)bucket)[0];
		for (j = 12; j < original_usage; j += atoi((hf->schema[i])+1)+sizeof(long long)){
			RID rid = (*read_keypair[hf->schema_array[i]])(bucket, buf, j, atoi((hf->schema[i])+1));
			(*decode[hf->schema_array[i]])(buf, line, &zero,atoi((hf->schema[i])+1));
			zero = 0;
			printf("(key:%s rid:%lld) ",line, rid);
		}
		printf("\n");
	}
		printf("overflow bucket:\n");
	for (iter=0; iter <n_buckets; iter++){
		read_bucket(bucket, iter, index_file->overflow_path);
		int offset = ((int*)bucket)[0];
		long long pointer = ((long long*)(bucket+4))[0];
		printf("offset=%d, pointer=%lld\n",offset,pointer);
		printf("key-rid pairs:\n");
		
		int original_usage = ((int*)bucket)[0];
		for (j = 12; j < original_usage; j += atoi((hf->schema[i])+1)+sizeof(long long)){
			RID rid = (*read_keypair[hf->schema_array[i]])(bucket, buf, j, atoi((hf->schema[i])+1));
			(*decode[hf->schema_array[i]])(buf, line, &zero,atoi((hf->schema[i])+1));
			zero = 0;
			printf("(key:%s rid:%lld) ",line, rid);
		}
		printf("\n");
	}
	free(bucket);
	free(buf);
	free(line);
}



