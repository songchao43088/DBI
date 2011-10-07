#ifndef CU_HF
#define CU_HF

typedef long RID;

struct hfile{
   int n_records;
   char ** schema;
   void * content;
};

typedef struct hfile HFILE;

struct cursor{
   void * pre_record;
   void * next_record;
   RID id;
};

typedef struct cursor CURSOR;



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

// start a scan (cursor) in the file
CURSOR *hf_scan_open(HFILE *hf, COND*, int conditions);

// stop a scan in the file
void hf_scan_close(CURSOR *cur);

// get the next entry of a scan
// returns (rid_t) -1 in case we are out of records
RID hf_scan_next(CURSOR *cur, void *record);


#endif
