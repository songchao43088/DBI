#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hf.h"
#include "parse.c"


int main(int argc, char** argv){
	
   
	char line[1000];	// stdin buffer
	int is_new = 0;		// flag, indicating if the imported CSV file is of a different schema	
	int i=0,j=0,k=0;
	int fields = 1;
	char **inputschema;     // schema of imported CSV file

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

	// parse command pattern of the form "./my_program heap_file_path -i < example.csv"
	if(argc==3 && !strcmp(argv[2],"-i")){
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
                // TODO: 
		//printf("%d\n",fields);
		//printf("%s\n",line);
		inputschema=(char**)malloc((fields)*sizeof(char*));
		inputschema[4]=(char*)malloc(5*sizeof(char));
		i = 0;
		j = 0;
		k = 0;

		inputschema[0]=(char*)malloc(5*sizeof(char));
		// parse the first line stored in "line[]"
		// get the CSV schema info.
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

		if (is_new ==1) {
			hf_create(argv[1], inputschema, fields);
			printf("A new heap file is created!\n");
                        // TODO: import the records
		} else {
			if (compareschema(hf->schema,inputschema,fields)!=0) {
				printf("Fields do not match ! \n");
				return 1;
			} else {
				
				void * record = calloc(hf_record_length(hf),sizeof(char));
				int offset = 0;
				i = 0;	
				k = 0;			
				line[i] = getchar();
				
				while(line[i]!=EOF){	
		
					while(line[i]!='\n'){

						printf("!!!%c\n",line[i]);
						while (line[i]!= ','){
							if (line[i]=='\n') {
								break;
							}
							i++;
							line[i] = getchar();

						}
						
						
						if (line[i]=='\n')
							break;
						line[i]=0;

						(*encode[hf->schema_array[k]])(record, line, &offset,atoi((hf->schema[k])+1));
						k++;
						i=0;
						line[i] = getchar();
					}
					if (line[i]=='\n') {
						line[i]=0;

						(*encode[hf->schema_array[k]])(record, line, &offset,atoi((hf->schema[k])+1));
					}

					hf_insert(hf, record);
					line[i]=0;
					i=0;
					k=0;
					offset = 0;
					line[i] = getchar();
				}
				
			}
		}
	} else 
          query(argc, argv);

	return 0;
}











