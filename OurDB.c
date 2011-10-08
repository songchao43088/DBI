#include <stdio.h>
#include <stdlib.h>
#include "hf.h"
#include <string.h>
#include <unistd.h>

void printrecord(HFILE *hf, void *record, int * projection, int pyesno){
	int j;
	char buf[100];
	int offset = 0;
	(*decode[hf->schema_array[0]])(record, buf, &offset,atoi((hf->schema[0])+1));
	if (pyesno == 0 || projection[0] == 1)					
		printf("%s",buf);
	for (j = 1; j < hf->n_fields; j++)
	{

		(*decode[hf->schema_array[j]])(record, buf, &offset,atoi((hf->schema[j])+1));
		if (pyesno == 0 || projection[j] == 1)						
			printf(",%s",buf);
		//parse
	}
	printf("\n");
}

void query(int argc, char** argv){
	int i;
	int num = 0;
	HFILE *hf;
	CURSOR *cur;
	void *record;
	int *projection;
	int pyesno = 0;
	hf = hf_open(argv[1]);
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
	if (con != NULL){
		for (i=2; i<argc;i++){
			if (argv[i][0]=='-' && argv[i][1]=='s'){
				char *op = (char*)malloc(5*sizeof(char));
				void *value = malloc(110*sizeof(char));
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
	return;
}



int main(int argc, char** argv){
	/*RID r;
	const char *path = "OurDB.db";
	char *schema[4];
	schema[0]="c9";
	schema[1]="c9";
	schema[2]="i2";
	schema[3]="r4";
	hf_create(path, schema, 4);
	r = hf_insert(hf_open(path),(void*)"badworld");
	void *record = malloc(sizeof(char)*20);
	hf_record(hf_open(path), r, record);
*/

	char line[1000];
	int is_new = 0;
//	char field[5];
	int i=0,j=0,k=0;
	int fields = 1;
	char **inputschema;

	encode[0]=encode_i1;
	encode[1]=encode_i2;
	encode[2]=encode_i4;
	encode[3]=encode_i8;
	encode[4]=encode_r4;
	encode[5]=encode_r8;
	encode[6]=encode_cx;

	decode[0]=decode_i1;
	decode[1]=decode_i2;
	decode[2]=decode_i4;
	decode[3]=decode_i8;
	decode[4]=decode_r4;
	decode[5]=decode_r8;
	decode[6]=decode_cx;
	
	compare[0]=compare_i1;
	compare[1]=compare_i2;
	compare[2]=compare_i4;
	compare[3]=compare_i8;
	compare[4]=compare_r4;
	compare[5]=compare_r8;
	compare[6]=compare_cx;

	if(argc==3 && !strcmp(argv[2],"-i")){
		HFILE *hf;
		hf = hf_open(argv[1]);
		if (hf == NULL)
			is_new =1;
		line[i] = getchar();
		printf("\ngetchar:%c\n",line[i]);
		while (line[i]!= '\n'){
			if (line[i]==',')
				fields++;
			i++;
			line[i] = getchar();
printf("\ngetchar:%c\n",line[i]);
		}
		line[i]=0;
		if ((is_new == 0) && (fields != hf->n_fields)) {
			printf("Field No. not match");
			return 1;
		}
		printf("%d\n",fields);
				printf("%s\n",line);
		inputschema=(char**)malloc((fields)*sizeof(char*));
inputschema[4]=(char*)malloc(5*sizeof(char));
		i = 0;
		j = 0;
		k = 0;

		inputschema[0]=(char*)malloc(5*sizeof(char));
printf("111\n");
		while(line[i]!=0){
			printf("123%c\n",line[i]);
			while(line[i]!=','){

				if (line[i]==0) {
					break;
				}
				inputschema[j][k]=line[i];
				i++;
				k++;
			}
			printf("%d and %d",j,k);
			inputschema[j][k]=0;
			if (line[i]==0)
				break;
			i++;
			
			k=0;
			j++;
printf("2552%d\n",j);
			if (j<fields)
				inputschema[j]=(char*)malloc(5*sizeof(char));
printf("321%c\n",line[i]);	
		}
printf("222\n");
		if (is_new ==1) {
			hf_create(argv[1], inputschema, fields);
			printf("create!!\n");
		} else {
			if (compareschema(hf->schema,inputschema,fields)!=0) {
				printf("Fields do not match\n");
				return 1;
			} else {
				
				void * record = calloc(hf_record_length(hf),sizeof(char));
				int offset = 0;
				i = 0;	
				k = 0;			
				line[i] = getchar();
				
				while(line[i]!=EOF){	
printf("jinmeijin\n");			
					while(line[i]!='\n'){
printf("jinmeijinani\n");
						printf("!!!%c\n",line[i]);
						while (line[i]!= ','){
							if (line[i]=='\n') {
								break;
							}
							i++;
							line[i] = getchar();
printf("jafdsafas%c,(%d,%d)\n",line[i],i,k);
						}
						
						
						if (line[i]=='\n')
							break;
						line[i]=0;
printf("before encodes: %s\n",line);
						(*encode[hf->schema_array[k]])(record, line, &offset,atoi((hf->schema[k])+1));
						k++;
						i=0;
						line[i] = getchar();
					}
					if (line[i]=='\n') {
						line[i]=0;
printf("before encodes: %s\n",line);
						(*encode[hf->schema_array[k]])(record, line, &offset,atoi((hf->schema[k])+1));
					}
//printf("before insert: %s\n",(char*)record);
//printf("before insert: %s\n",(char*)record+9);
//printf("before insert: %d\n",((short*)((char*)record+18))[0]);
//printf("before insert: %f\n",((float*)((char*)record+20))[0]);
					hf_insert(hf, record);
					line[i]=0;
					i=0;
					k=0;
					offset = 0;
					line[i] = getchar();
				}
				
			}
		}
	} else /*if (argc == 2) {
		HFILE *hf;
		hf = hf_open(argv[1]);
		if (hf == NULL){
			printf("file does not exist.\n");
			return 1;
		} else {
			int i;
			printf("fields: %s",hf->schema[0]);
			for (i = 1; i<hf->n_fields; i++) {
				printf(",%s",hf->schema[i]);
			}
			printf("\n");
			long id;
			void * record = calloc(hf_record_length(hf), sizeof(char));
			int j;
			char buf[100];
			int offset = 0;
			//printf("question?\n");
			id = sizeof(long)+sizeof(int)+2*hf->n_fields+0*hf_record_length(hf);
			for (i = 0; i< hf->n_records; i++){		
				hf_record(hf, id, record);
			//printf("question? :::%s\n",(char*)record);
//printf("before decode: %s\n",(char*)record);
//printf("before decode: %s\n",(char*)record+9);
//printf("before decode: %d\n",((short*)((char*)record+18))[0]);
//printf("before decode: %f\n",((float*)((char*)record+20))[0]);
				(*decode[hf->schema_array[0]])(record, buf, &offset,atoi((hf->schema[0])+1));					
					printf("%s",buf);
				for (j = 1; j < hf->n_fields; j++)
				{

					(*decode[hf->schema_array[j]])(record, buf, &offset,atoi((hf->schema[j])+1));					
					printf(",%s",buf);
					//parse
				}
				printf("\n");
				id += hf_record_length(hf);
				offset = 0;
			}



		}
	} else */query(argc, argv);

	return 0;
}











