#ifndef CU_HF
#define CU_HF

typedef long long RID;

/* heap file struct */
struct hfile{
   long long n_records;	//number of records
   char ** schema;
   int *schema_array;
   int *index_array;
   int n_fields;	//number of fields
   void * content;	//pointer to content
   const char *path;
};
typedef struct hfile HFILE;


// condition details
typedef struct {
        int field;  // field the condition is on
        const char *op;  // the operator =, >, <, >=, <=, <>
        void *value;  // value to compare against
} COND;

// cursor details
struct cursor{
   HFILE *hf;
   COND *con;
   int conditions;
   RID current;	// the current location (offset)
   int index;
   int index_mo;
   int index_current_bucket;
   int index_bucket_offset;
};
typedef struct cursor CURSOR;


// compare two schema
int compareschema(char  **a, char **b, int length);

/* field processing, read from CSV file, write to heap file
   encode CSV data into binary representation in heap file  */

int (*encode[7])(void *record, char *line, int *offset, int length);

int encode_i1(void *record, char *line, int *offset, int length);

int encode_i2(void *record, char *line, int *offset, int length);

int encode_i4(void *record, char *line, int *offset, int length);

int encode_i8(void *record, char *line, int *offset, int length);

int encode_r4(void *record, char *line, int *offset, int length);

int encode_r8(void *record, char *line, int *offset, int length);

int encode_cx(void *record, char *line, int *offset, int length);


/* field processing, read from heap file, write as string to buffer
   Decode heap file binary data to string that could be printed out or 
   converted to CSV file as output  */

int (*decode[7])(void *record, char *line, int *offset, int length);

int decode_i1(void *record, char *line, int *offset, int length);

int decode_i2(void *record, char *line, int *offset, int length);

int decode_i4(void *record, char *line, int *offset, int length);

int decode_i8(void *record, char *line, int *offset, int length);

int decode_r4(void *record, char *line, int *offset, int length);

int decode_r8(void *record, char *line, int *offset, int length);

int decode_cx(void *record, char *line, int *offset, int length);

/* field processing, compare the value given in selection clauses with 
   records in heap file */

int (*compare[7])(void *record, int offset, void *value, int length);

int compare_i1(void *record, int offset, void *value, int length);

int compare_i2(void *record, int offset, void *value, int length);

int compare_i4(void *record, int offset, void *value, int length);

int compare_i8(void *record, int offset, void *value, int length);

int compare_r4(void *record, int offset, void *value, int length);

int compare_r8(void *record, int offset, void *value, int length);

int compare_cx(void *record, int offset, void *value, int length);



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
long long hf_records(HFILE *hf);



// start a scan (cursor) in the file
CURSOR *hf_scan_open(HFILE *hf, COND* con, int conditions);

// stop a scan in the file
void hf_scan_close(CURSOR *cur);

// calculate the offset within a row of a particular field (column)
int calculate_offset(char **schema, int field);

// filter records according to conditions in a heap file
int filter(void *record, COND *con, int conditions, HFILE *hf);

// get the next entry of a scan
// returns (rid_t) -1 in case we are out of records
RID hf_scan_next(CURSOR *cur, void *record);



#endif
