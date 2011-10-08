#include "hf.h"
#include "parse.h"
/*
	parse.c
*/

//

void import_record(HFILE *hf){
	void * record = calloc(hf_record_length(hf),sizeof(char));
				int offset = 0;
				int i = 0;	
				int k = 0;
char line[1000];			
				line[i] = getchar();
				
				while(line[i]!=EOF){	
		
					while(line[i]!='\n'){

						//printf("!!!%c\n",line[i]);
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

void printrecord(HFILE *hf, void *record, int * projection, int pyesno){
        //printf("Enter into print records!!\n");
	int j;
	char buf[100];
	int offset = 0;
	int first = 0;
	(*decode[hf->schema_array[0]])(record, buf, &offset,atoi((hf->schema[0])+1));
	//printf("%d,%d%d%d%d",pyesno,projection[0],projection[1],projection[2],projection[3]);
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
		//parse
	}
	printf("\n");
}


//
void query(int argc, char** argv){
        //printf("ENter into the qeury!!!!!\n");
	int i;
	int num = 0;
	HFILE *hf;
	CURSOR *cur;
	void *record;
	int *projection;
	int pyesno = 0;
	hf = hf_open(argv[1]);
	if (hf == NULL)
		return ;
	projection = (int*)calloc(hf->n_fields,sizeof(int));
	
	//printf("%d, %d, %d ,%d \n", hf->schema_array[0],hf->schema_array[1],hf->schema_array[2],hf->schema_array[3]);
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
	//printf("conds: %d\n",num);
	num = 0;
	if (1/*con != NULL*/){
		for (i=2; i<argc;i++){
			//printf("%c,%c",argv[i][0],argv[i][1]);
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
				//printf("op:%s\n",op);
				i++;
				strcpy(value,argv[i]);
				//printf("argv :%s\n",argv[i]);
				//printf("value :%s\n",(char*)value);
				con[num].value = value;
				//printf("convalue :%s\n",(char*)con[num].value);
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
	}
				//	printf("con0value :%s\n",(char*)con[0].value);
	//printf("conds2: %d, %d, %s, %s\n",num,con[0].field,con[0].op,(char*)con[0].value);
	if (pyesno == 0 || projection[0] == 1)		
		printf("%s",hf->schema[0]);
	for (i = 1; i<hf->n_fields; i++) {
		if (pyesno == 0 || projection[i] == 1)	
			printf(",%s",hf->schema[i]);
	}
	printf("\n");
	cur = hf_scan_open(hf, con, num);
	//printf("created\n");
	RID id;
	do {
		id = hf_scan_next(cur, record);
	//		printf("get next\n");
		if (id != -1){
		//print//projection
			printrecord(hf, record, projection, pyesno);
		}
	} while (id != -1);
	hf_scan_close(cur);
	free(projection);
	hf_close(hf);
	free(record);
	return;
}


