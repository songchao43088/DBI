#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hf.h"
#include "parse.h"
#include "lh.h"



void index_option(HFILE *hf, int argc, char** argv){
	int *index;
	int byesno = 0;
	int i;
	CURSOR *cur;
	void *record;

	record = calloc(hf_record_length(hf),sizeof(char));
	index = (int*)calloc(hf->n_fields,sizeof(int));
	for (i=2; i<argc;i++){
		if (argv[i][0]=='-' && argv[i][1]=='b'){
			if (atoi(&(argv[i][2]))==0) {
				printf("Invalid Column\n");
				return;
			}
			int column = atoi(&(argv[i][2]));
			byesno = 1;
			index[column-1]=1;
			hf->index_array[column-1]=1;
		} 
	}
	if (byesno == 1){
		for (i = 0; i< hf->n_fields; i++){
			if (index[i]==1){
				init_index(i, hf);
			}		
		}
	}
	if (byesno==1){
	//TODO:xianjiansuoyin
		cur = hf_scan_open(hf, NULL, 0);
		RID id;
		INDEX_FILE **index_file;
		index_file = (INDEX_FILE**)malloc(sizeof(INDEX_FILE*)*hf->n_fields);
		for (i=0;i<hf->n_fields;i++){
			if (index[i]==1){
				index_file[i]=open_index(i,hf);
			}
		}
		
			
		do {
			id = hf_scan_next(cur, record);
	
			if (id != -1){
				for(i=0;i<hf->n_fields;i++){
					//if (hf->index_array[i]==1){
					if (index[i]==1){
						hash_record(hf, record, i, id, index_file[i]);	//TODO:bianzhegehanshu
					}
				}
			}
		} while (id != -1);
		for (i=0;i<hf->n_fields;i++){
			if (index[i]==1){
				close_index(hf, i, index_file[i]);
				//print_index(hf, i, index_file[i]);
			}
		}
		free(index_file);
		hf_scan_close(cur);
	}
	free(record);
	free(index);

}



int main(int argc, char** argv){
	
   
	char line[1000];	// stdin buffer
	int is_new = 0;		// flag, indicating if the imported CSV file is of a different schema	
	int i=0,j=0,k=0;
	int fields = 1;
	char **inputschema;     // schema of imported CSV file
	//char n = '\n';
	//printf("%d\n",n);
	// map to different encode functions
        // based on different data types 

	encode[0]=encode_i1;
	encode[1]=encode_i2;
	encode[2]=encode_i4;
	encode[3]=encode_i8;
	encode[4]=encode_r4;
	encode[5]=encode_r8;
	encode[6]=encode_cx;
	// map to different decode functions
        // based on different data types 
	decode[0]=decode_i1;
	decode[1]=decode_i2;
	decode[2]=decode_i4;
	decode[3]=decode_i8;
	decode[4]=decode_r4;
	decode[5]=decode_r8;
	decode[6]=decode_cx;
	// map to different compare functions
        // based on different data types 	
	compare[0]=compare_i1;
	compare[1]=compare_i2;
	compare[2]=compare_i4;
	compare[3]=compare_i8;
	compare[4]=compare_r4;
	compare[5]=compare_r8;
	compare[6]=compare_cx;

	calculate_index[0]=calculate_index_i1;
	calculate_index[1]=calculate_index_i2;
	calculate_index[2]=calculate_index_i4_r4;
	calculate_index[3]=calculate_index_i8_r8;
	calculate_index[4]=calculate_index_i4_r4;
	calculate_index[5]=calculate_index_i8_r8;
	calculate_index[6]=calculate_index_cx;

	update_bucket[0]=update_bucket_i1;
	update_bucket[1]=update_bucket_i2;
	update_bucket[2]=update_bucket_i4;
	update_bucket[3]=update_bucket_i8;
	update_bucket[4]=update_bucket_r4;
	update_bucket[5]=update_bucket_r8;
	update_bucket[6]=update_bucket_cx;

	read_keypair[0]=read_keypair_i1;
	read_keypair[1]=read_keypair_i2;
	read_keypair[2]=read_keypair_i4;
	read_keypair[3]=read_keypair_i8;
	read_keypair[4]=read_keypair_r4;
	read_keypair[5]=read_keypair_r8;
	read_keypair[6]=read_keypair_cx;

	// parse command pattern of the form "./my_program heap_file_path -i < example.csv"
	if(argc>=3 && !strcmp(argv[2],"-i")){
		HFILE *hf;

		hf = hf_open(argv[1]);

		// heap file doesn't exist
		if (hf == NULL)
			is_new =1;

		// scan the first line (schema line) in CSV file
		line[i] = getchar();
		while (line[i]!= '\n'){
			if (line[i]==',')
				fields++;
			i++;
			line[i] = getchar();
		}
		line[i]=0;
		
		// if heapfile already exists
		// check the number of fields
		if ((is_new == 0) && (fields != hf->n_fields)) {
			printf("The number of fields does not match ! \n");
			return 1;
		}
                
		inputschema=(char**)malloc((fields)*sizeof(char*));

		i = 0;
		j = 0;
		k = 0;

		inputschema[0]=(char*)malloc(5*sizeof(char));
		// parse the first line stored in "line[]"
		// get the CSV schema info.
                // save the schema into "inputschema"
		while(line[i]!=0){
			
			while(line[i]!=','){

				if (line[i]==0) {
					break;
				}
				inputschema[j][k]=line[i];
				i++;
				k++;
			}
			
			inputschema[j][k]=0;
			if (line[i]==0)
				break;
			i++;
			k=0;
			j++;

			if (j<fields)
				inputschema[j]=(char*)malloc(5*sizeof(char));
		}

		// if there is no such existing heap file, create one
		if (is_new ==1) {
			hf_create(argv[1], inputschema, fields);
			printf("A new heap file is created!\n");

			hf = hf_open(argv[1]);

			index_option(hf, argc, argv);
			import_record(hf);
		} 
		// if heap file existed, it should be already opened as "hf"
		else {
			
			if (compareschema(hf->schema,inputschema,fields)!=0) {
				printf("Fields do not match ! \n");
				return 1;
			} else {
				index_option(hf, argc, argv);
				import_record(hf);
			}
		}
		for (i=0;i<=j;i++)
			free(inputschema[i]);
		free(inputschema);
		hf_close(hf);
	} else {
          query(argc, argv);
	}
	return 0;
}










