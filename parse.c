#include "hf.h"
#include "parse.h"


/*
	function: import_record
	import records from CSV file (parsed as stdin, saved in line[])
        encode these records into heap file "hf" by calling (*encode[])(param_list)
*/
void import_record(HFILE *hf){
	void * record = calloc(hf_record_length(hf),sizeof(char));
				int offset = 0;
				int i = 0;	
				int k = 0;
				char line[1000];			
				line[i] = getchar();
				
				while(line[i]!=EOF){	
		
					while(line[i]!='\n'){

						
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
	free(record);
}


/*
	function: printrecord
        print out records decoded from heapfile "hp"
	and do projection on-the-fly
	@param:
	projection: an bitmap array of projected columns
	pyesno: a flag indicating whether to project or not
*/
void printrecord(HFILE *hf, void *record, int * projection, int pyesno){
       
	int j;
	char buf[100];
	int offset = 0;
	int first = 0;
	(*decode[hf->schema_array[0]])(record, buf, &offset,atoi((hf->schema[0])+1));
	
	if (pyesno == 0 || projection[0] == 1){					
		printf("%s",buf);
		first = 1;
		}
	for (j = 1; j < hf->n_fields; j++)
	{

		(*decode[hf->schema_array[j]])(record, buf, &offset,atoi((hf->schema[j])+1));
		if (pyesno == 0 || projection[j] == 1) {
			if (first == 1)	{		
				printf(",%s",buf);
			} else {
				printf("%s",buf);
				first = 1;
			}
		}
		
	}
	printf("\n");
}



/*
	function: query
	parse the command line query including selection and projection
	like "./my_program heap_file_path -s1 ">=" 5 -p1 -p2 "
	put all the condition clauses(column, operator, value) into struct COND *con
	free memory allocation after each query
*/
void query(int argc, char** argv){
        
	int i;
	int num = 0;
	HFILE *hf;
	CURSOR *cur;
	void *record;
	int *projection;
	int pyesno = 0;
	int flag = 0;
	hf = hf_open(argv[1]);
	if (hf == NULL)
		return ;
	projection = (int*)calloc(hf->n_fields,sizeof(int));
	
	
	record = calloc(hf_record_length(hf),sizeof(char));

	for (i=2; i<argc;i++){
		if (argv[i][0]=='-' && argv[i][1]=='s')
			num++;
	}
	COND *con;
	if (num == 0)
		con = NULL;
	else {
		con=(COND*)malloc(sizeof(COND)*num);
		if (con == NULL){
			printf("Not enough memory.\n");
			exit(1);
		}
	}
	
	num = 0;
	
		for (i=2; i<argc;i++){
			
			if (argv[i][0]=='-' && argv[i][1]=='s'){
				char *op = (char*)malloc(5*sizeof(char));
				void *value = malloc(110*sizeof(char));
				if (atoi(&(argv[i][2]))==0) {
					printf("Invalid Column\n");
					return;
				}
				con[num].field=atoi(&(argv[i][2]));
				
				i++;
				strcpy(op,argv[i]);
				con[num].op=op;
				
				i++;
				strcpy(value,argv[i]);
				
				con[num].value = value;
				
				num++;
			} else if (argv[i][0]=='-' && argv[i][1]=='p'){
				if (atoi(&(argv[i][2]))==0) {
					printf("Invalid Column\n");
					return;
				}
				int column = atoi(&(argv[i][2]));
				pyesno = 1;
				projection[column-1]=1;
			}
		}
	
				
	if (pyesno == 0 || projection[0] == 1) {		
		printf("%s",hf->schema[0]);
                flag = 1;
	}

	for (i = 1; i<hf->n_fields; i++) {
		if (pyesno == 0 || projection[i] == 1) {
			if(flag == 1){	
			printf(",%s",hf->schema[i]);
			}
			else {
				printf("%s",hf->schema[i]);
				flag = 1;
			}
		}

	}
	printf("\n");
	cur = hf_scan_open(hf, con, num);
	
	RID id;
	do {
		id = hf_scan_next(cur, record);
	
		if (id != -1){
		
			printrecord(hf, record, projection, pyesno);
		}
	} while (id != -1);
	hf_scan_close(cur);
	free(projection);
	hf_close(hf);
	free(record);
	return;
}


