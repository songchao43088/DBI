#ifndef CU_HF
#define CU_HF

typedef long RID;

struct hfile{
   long n_records;
   char ** schema;
   int *schema_array;
   int n_fields;
   void * content;
   const char *path;
};

typedef struct hfile HFILE;





int compareschema(char  **a, char **b, int length);

int encode_i1(void *record, char *line, int *offset, int length);

int encode_i2(void *record, char *line, int *offset, int length);

int encode_i4(void *record, char *line, int *offset, int length);

int encode_i8(void *record, char *line, int *offset, int length);

int encode_r4(void *record, char *line, int *offset, int length);

int encode_r8(void *record, char *line, int *offset, int length);

int encode_cx(void *record, char *line, int *offset, int length);

int (*encode[7])(void *record, char *line, int *offset, int length);

int decode_i1(void *record, char *line, int *offset, int length);

int decode_i2(void *record, char *line, int *offset, int length);

int decode_i4(void *record, char *line, int *offset, int length);

int decode_i8(void *record, char *line, int *offset, int length);

int decode_r4(void *record, char *line, int *offset, int length);

int decode_r8(void *record, char *line, int *offset, int length);

int decode_cx(void *record, char *line, int *offset, int length);

int (*decode[7])(void *record, char *line, int *offset, int length);

int compare_i1(void *record, int offset, void *value, int length);

int compare_i2(void *record, int offset, void *value, int length);

int compare_i4(void *record, int offset, void *value, int length);

int compare_i8(void *record, int offset, void *value, int length);

int compare_r4(void *record, int offset, void *value, int length);

int compare_r8(void *record, int offset, void *value, int length);

int compare_cx(void *record, int offset, void *value, int length);

int (*compare[7])(void *record, int offset, void *value, int length);


// create a heap file on path using the types defined in the schema argument
int hf_create(const char *path, char **schema, int schemac);

// open heap file and return a heap file pointer
// in case of error returns NULL
HFILE *hf_open(const char *path);

// close the heap file
void hf_close(HFILE *hf);

// return number of fields
int hf_fields(HFILE *hfp);

// return a specific field
char *hf_field(HFILE *hfp, int field);

// return the total record length
int hf_record_length(HFILE *hfp);

// insert a record in the heap file and return its RID
RID hf_insert(HFILE *hf, void *record);

// read a record by giving the RID
int hf_record(HFILE *hf, RID id, void *record);

// return the total number of records in the file
long hf_records(HFILE *hf);

// condition details
typedef struct {
        int field;  // field the condition is on
        const char *op;  // the operator =, >, <, >=, <=, <>
        void *value;  // value to compare against
} COND;

struct cursor{
   HFILE *hf;
   COND *con;
   int conditions;
   RID current;
};

typedef struct cursor CURSOR;

// start a scan (cursor) in the file
CURSOR *hf_scan_open(HFILE *hf, COND* con, int conditions);

// stop a scan in the file
void hf_scan_close(CURSOR *cur);

int calculate_offset(char **schema, int field);

int filter(void *record, COND *con, int conditions, HFILE *hf);

// get the next entry of a scan
// returns (rid_t) -1 in case we are out of records
RID hf_scan_next(CURSOR *cur, void *record);

void printrecord(HFILE *hf, void *record, int * projection, int pyesno);

void query(int argc, char** argv);

#endif
