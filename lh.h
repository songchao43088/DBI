#define BSIZE 120
#ifndef HF_H
#define HF_H
#include "hf.h"
#endif

struct index_file {
	long long next;
	long long s;
	long long n;
	char* main_path;
	char* overflow_path;
	char *type;
	int column_number;
};

typedef struct index_file INDEX_FILE;

void get_index_file_name(const char* path, int i,  char *main_index_file,  char *overflow_index_file);

void init_index(int i, HFILE *hf);

INDEX_FILE *open_index(int i, HFILE *hf);



void hash_record(HFILE *hf, void* record, int i, RID rid, INDEX_FILE *index_file);

long long (*calculate_index[7])(void *record, long long s, long long n, int offset);

long long calculate_index_i1(void *record, long long s, long long n, int offset);
long long calculate_index_i2(void *record, long long s, long long n, int offset);
long long calculate_index_i4_r4(void *record, long long s, long long n, int offset);
long long calculate_index_i8_r8(void *record, long long s, long long n, int offset);
long long calculate_index_cx(void *record, long long s, long long n, int offset);

void (*update_bucket[7])(void *bucket, void *record, RID rid, int size, int offset);

void update_bucket_i1(void *bucket, void *record, RID rid, int size, int offset);
void update_bucket_i2(void *bucket, void *record, RID rid, int size, int offset);
void update_bucket_i4(void *bucket, void *record, RID rid, int size, int offset);
void update_bucket_i8(void *bucket, void *record, RID rid, int size, int offset);
void update_bucket_r4(void *bucket, void *record, RID rid, int size, int offset);
void update_bucket_r8(void *bucket, void *record, RID rid, int size, int offset);
void update_bucket_cx(void *bucket, void *record, RID rid, int size, int offset);

void write_bucket(void *bucket, int bid, char *path);
void read_bucket(void *bucket, int bid, char *path);
int find_bucket(long long hash_index, long long *bid, int*mo, INDEX_FILE *index_file);

long long create_overflow_bucket(INDEX_FILE *index_file);
void init_bucket(void *bucket);

void mylink(long long from, int frommo, long long to, INDEX_FILE *index_file);

void rehash(HFILE *hf, int i, INDEX_FILE *index_file);
void rehash_keys(HFILE *hf, int i, INDEX_FILE *index_file);

RID (*read_keypair[7])(void *bucket, void *buf, int offset, int size);

RID read_keypair_i1(void *bucket, void *buf, int offset, int size);
RID read_keypair_i2(void *bucket, void *buf, int offset, int size);
RID read_keypair_i4(void *bucket, void *buf, int offset, int size);
RID read_keypair_i8(void *bucket, void *buf, int offset, int size);
RID read_keypair_r4(void *bucket, void *buf, int offset, int size);
RID read_keypair_r8(void *bucket, void *buf, int offset, int size);
RID read_keypair_cx(void *bucket, void *buf, int offset, int size);

long long find_free_list_tail(INDEX_FILE *index_file);
void add_to_free_list(long long bid, INDEX_FILE *index_file);

void print_index(HFILE *hf, int i, INDEX_FILE *index_file);
void close_index(HFILE *hf, int i, INDEX_FILE *index_file);
