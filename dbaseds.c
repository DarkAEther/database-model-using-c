/*
Author : VISHWAS RAJASHEKAR
Purpose: Database implemented using unconventional Data Structures
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HASHCONST 10
#define FIELDNAMELEN 10

typedef union data{
	int integer;
	char* string;
	double dbl;
} DATA;

struct record{
	DATA* fields;
};
struct dbase{
	char name[100];
	int number_of_records;
	int field_count;
	int capacity;
	struct record*** start;
	char** headers;
	int* discriminator;
};
int hash_int(struct record** memory,int searchkey);
int hash_string(struct record** memory,char* searchkey);
int hash_double(struct record** memory,double searchkey);
void print_dbases(struct dbase** allbases,int number_of_databases);
void print_all_columns_header_form(struct dbase** allbases, int selected_dbase_index);
void print_all_columns_selection_form(struct dbase** allbases, int selected_dbase_index);


int main(){
	int choice = -1;
	struct dbase* dbase[100];
	int dbases = 0;
	char name[100];
	while (1){
		printf("What would you like to do?\n 1) Create a new database\n 2) Query an existing database\n 3) Create a new record\n 4) Delete a record\n 5) Edit a record\n 6) Print all records\n 7) Quit\n");
		scanf("%d",&choice);
		switch (choice){
			case 1:{
				printf("Enter the name of the dbase and press Enter\n");
				scanf("%s",name);
				struct dbase* temp = (struct dbase*)malloc(sizeof(struct dbase));
				
				strcpy(temp->name,name);
				temp->number_of_records = 0;
				temp->capacity = 10;
				
				char fieldname[20],type[3];
				int numfields;
				printf("Enter number of fields and press Enter: "); scanf("%d",&numfields);
				temp->headers = (char**)malloc(sizeof(char*)*numfields); //stores all the strings
				temp->field_count = numfields;
				temp->discriminator = (int*)malloc(sizeof(int)*numfields);
				temp->start = (struct record***)malloc(sizeof(struct record**)*temp->field_count);
				for(int v = 0; v < temp->field_count;v++){
					temp->start[v] = (struct record**)malloc(sizeof(struct record*)*temp->capacity);
				}
				printf("Enter FIELDNAME<space>DATATYPE where DATATYPE is (int,dbl,str)\n");

				for (int j = 0; j < numfields; j++){
					scanf("%s",fieldname);
					scanf("%s",type);
					if (fieldname == "-1") break;
					temp->headers[j] = (char*)malloc(sizeof(char)*FIELDNAMELEN);
					strcpy(temp->headers[j],fieldname);
					//printf("%s\t",type);
					if (strcmp(type,"int")==0) temp->discriminator[j] = 1;
					else if (strcmp(type,"str")== 0) temp->discriminator[j] = 2;
					else if (strcmp(type,"dbl")==0) temp->discriminator[j] = 3;
				}

				dbase[dbases++] = temp;
				printf("Database: %s successfully created\n",name);
			}break;

			case 2:{
				int dbnum = -1;
				char value[100];
				int index;
				if(dbases == 0) {printf("No databases present\n"); break;}
				print_dbases(dbase,dbases);
				printf("Enter the index number of the database you want and press Enter\n");
				
				scanf("%d",&dbnum);
				printf("\nColumns:\n");
				print_all_columns_selection_form(dbase,dbnum);

				printf("Enter (column_index search_value) of choice:\n");
				scanf("%d",&index);scanf("%s",value);

				
				int desc = dbase[dbnum]->discriminator[index];
				int hash;
				int rogue = 0;
				switch (desc){
					case 1:{
						//integer
						int hashorig = atoi(value)%HASHCONST;
						hash = hashorig;
						while (dbase[dbnum]->start[index][hash]->fields[index].integer != atoi(value) && !rogue){
							hash = (hash+1)%HASHCONST;
							if (hash==hashorig) {printf("Not Found\n");rogue=1;}
						} 
						
					}break;
					case 2:{ //string
						int hashorig = strlen(value)%HASHCONST;
						hash = hashorig;
						while (strcmp(dbase[dbnum]->start[index][hash]->fields[index].string,value) !=0 && !rogue){
							hash = (hash+1)%HASHCONST;
							if (hash==hashorig) {printf("Not Found\n");rogue=1;}
						} 
					}break;
					case 3:{//double

						int hashorig = ((int)atof(value))%HASHCONST;
						hash = hashorig;
						while (dbase[dbnum]->start[index][hash]->fields[index].dbl != atof(value) && !rogue){
							hash = (hash+1)%HASHCONST;
							if (hash==hashorig) {printf("Not Found\n");rogue=1;}
						}
					}break;
				}
				if (rogue == 0){
					printf("Record Found\n");
					print_all_columns_header_form(dbase,dbnum);
					for (int g = 0; g < dbase[dbnum]->field_count;g++){
						if (dbase[dbnum]->discriminator[g] == 1) printf("%d\t",dbase[dbnum]->start[index][hash]->fields[g].integer);
						else if (dbase[dbnum]->discriminator[g] == 2) printf("%s\t",dbase[dbnum]->start[index][hash]->fields[g].string);
						else if (dbase[dbnum]->discriminator[g] == 3) printf("%le\t",dbase[dbnum]->start[index][hash]->fields[g].dbl);
					}
					printf("\n");
				}

			}break;
			case 3:{
				int dbnum = -1;
				char* value;
				int index;
				printf("Enter the index number of the database you want\n");
				print_dbases(dbase,dbases);
				scanf("%d",&dbnum);
				print_all_columns_header_form(dbase,dbnum);

				struct record* newrec= (struct record*)malloc(sizeof(struct record));
				newrec->fields = (DATA*)malloc(sizeof(DATA)*dbase[dbnum]->field_count);
				//printf("%d",dbase[dbnum]->field_count);

				for (int k = 0; k<dbase[dbnum]->field_count;k++){
					//printf("%d\t",dbase[dbnum]->discriminator[k]);
					if (dbase[dbnum]->discriminator[k] == 1) scanf("%d",&(newrec->fields[k].integer));
					else if (dbase[dbnum]->discriminator[k] == 2){
						value = (char*)malloc(sizeof(char)*50);
						scanf("%s",value);
						newrec->fields[k].string= value;
					} else if (dbase[dbnum]->discriminator[k] == 3) scanf("%le",&(newrec->fields[k].dbl));
				}

				for (int k = 0; k<dbase[dbnum]->field_count;k++){ // multi hash
					int field_type = dbase[dbnum]->discriminator[k];
					switch (field_type){
						case 1:{ // integer
							//printf("Integer %d\n",hash_int(dbase[dbnum]->start[k],newrec->fields[k].integer));
							dbase[dbnum]->start[k][hash_int(dbase[dbnum]->start[k],newrec->fields[k].integer)] = newrec;
						}break;
						case 2:{ // string
							//printf("String %d\n",hash_string(dbase[dbnum]->start[k],newrec->fields[k].string));
							dbase[dbnum]->start[k][hash_string(dbase[dbnum]->start[k],newrec->fields[k].string)] = newrec;
						}break;
						case 3:{ // double
							//printf("Double %d\n",hash_double(dbase[dbnum]->start[k],newrec->fields[k].dbl));
							dbase[dbnum]->start[k][hash_double(dbase[dbnum]->start[k],newrec->fields[k].dbl)] = newrec;
						}break;
					}
					
				}
				dbase[dbnum]->number_of_records++;
				printf("\nThe record was successfully created\n");

			}break;
			case 4:{
				int dbnum = -1;
				char value[100];
				int index;
				printf("Enter the index number of the database you want\n");
				print_dbases(dbase,dbases);

				scanf("%d",&dbnum);
				printf("\nColumns:\n");
				// for(int o = 0; o < dbase[dbnum]->field_count;o++){
				// 	printf("%d|-->%s\n",o,dbase[dbnum]->headers[o]);
				// }
				print_all_columns_selection_form(dbase, dbnum);
				printf("Enter (column_index search_value) of choice:\n");
				scanf("%d",&index);scanf("%s",value);

				//TODO n hash tables..you are insane..eyy im done
				int desc = dbase[dbnum]->discriminator[index];
				int hash;
				int rogue = 0;
				switch (desc){
					case 1:{
						//integer
						int hashorig = atoi(value)%HASHCONST;
						hash = hashorig;
						while (dbase[dbnum]->start[index][hash]->fields[index].integer != atoi(value) && !rogue){
							hash = (hash+1)%HASHCONST;
							if (hash==hashorig) {printf("Not Found\n");rogue=1;}
						} 
						
					}break;
					case 2:{ //string
						int hashorig = strlen(value)%HASHCONST;
						hash = hashorig;
						while (strcmp(dbase[dbnum]->start[index][hash]->fields[index].string,value) !=0 && !rogue){
							hash = (hash+1)%HASHCONST;
							if (hash==hashorig) {printf("Not Found\n");rogue=1;}
						} 
					}break;
					case 3:{//double

						int hashorig = ((int)atof(value))%HASHCONST;
						hash = hashorig;
						while (dbase[dbnum]->start[index][hash]->fields[index].dbl != atof(value) && !rogue){
							hash = (hash+1)%HASHCONST;
							if (hash==hashorig){ printf("Not Found\n");rogue=1;}
						}
					}break;
				}
				if (rogue == 0){
					printf("Record Found\n");
					for (int g = 0; g < dbase[dbnum]->field_count;g++){
						if (dbase[dbnum]->discriminator[g] == 1) printf("%d\t",dbase[dbnum]->start[index][hash]->fields[g].integer);
						else if (dbase[dbnum]->discriminator[g] == 2) printf("%s\t",dbase[dbnum]->start[index][hash]->fields[g].string);
						else if (dbase[dbnum]->discriminator[g] == 3) printf("%le\t",dbase[dbnum]->start[index][hash]->fields[g].dbl);
					}
					printf("\n");
					free(dbase[dbnum]->start[index][hash]);
					dbase[dbnum]->start[index][hash] = NULL;
					dbase[dbnum]->number_of_records--;
					printf("Record Deleted\n");
				}

			}break;
			case 6:{
				int dbnum = -1;
				char value[100];
				int index;
				printf("Enter the index number of the database you want\n");
				print_dbases(dbase,dbases);

				scanf("%d",&dbnum);
				print_all_columns_header_form(dbase, dbnum);

				for (int k = 0; k < dbase[dbnum]->field_count;k++){
					for (int t = 0; t < dbase[dbnum]->capacity;t++){
						if (dbase[dbnum]->start[k][t]!=NULL){
							for (int g = 0; g < dbase[dbnum]->field_count;g++){
									if (dbase[dbnum]->discriminator[g] == 1) printf("%d\t",dbase[dbnum]->start[k][t]->fields[g].integer);
									else if (dbase[dbnum]->discriminator[g] == 2) printf("%s\t",dbase[dbnum]->start[k][t]->fields[g].string);
									else if (dbase[dbnum]->discriminator[g] == 3) printf("%le\t",dbase[dbnum]->start[k][t]->fields[g].dbl);
							}
							printf("\n");
						}
					}printf("\n");
				}
			}break;
			case 7:{
				return 0;
			}break;
			case 5:{
				int dbnum = -1;
				char value[100];
				int index;
				if(dbases == 0){printf("No databases present\n"); break;}
				print_dbases(dbase,dbases);
				printf("Enter the index number of the database you want and press Enter\n");
				
				scanf("%d",&dbnum);
				printf("\nColumns:\n");
				print_all_columns_selection_form(dbase,dbnum);

				printf("Enter (column_index search_value) of choice:\n");
				scanf("%d",&index);scanf("%s",value);

				
				int desc = dbase[dbnum]->discriminator[index];
				int hash;
				char* sval;
				int rogue = 0;
				switch (desc){
					case 1:{
						//integer
						int hashorig = atoi(value)%HASHCONST;
						hash = hashorig;
						while (dbase[dbnum]->start[index][hash]->fields[index].integer != atoi(value) && !rogue){
							hash = (hash+1)%HASHCONST;
							if (hash==hashorig) {printf("Not Found\n");rogue=1;}
						} 
						
					}break;
					case 2:{ //string
						int hashorig = strlen(value)%HASHCONST;
						hash = hashorig;
						while (strcmp(dbase[dbnum]->start[index][hash]->fields[index].string,value) !=0 && !rogue){
							hash = (hash+1)%HASHCONST;
							if (hash==hashorig) {printf("Not Found\n");rogue=1;}
						} 
					}break;
					case 3:{//double

						int hashorig = ((int)atof(value))%HASHCONST;
						hash = hashorig;
						while (dbase[dbnum]->start[index][hash]->fields[index].dbl != atof(value) && !rogue){
							hash = (hash+1)%HASHCONST;
							if (hash==hashorig) {printf("Not Found\n");rogue=1;}
						}
					}break;
				}
				if (rogue == 0){
					printf("Record Found\n");printf("Re-enter the record\n");
					print_all_columns_header_form(dbase,dbnum);
					for (int g = 0; g < dbase[dbnum]->field_count;g++){
						if (dbase[dbnum]->discriminator[g] == 1) printf("%d\t",dbase[dbnum]->start[index][hash]->fields[g].integer);
						else if (dbase[dbnum]->discriminator[g] == 2) printf("%s\t",dbase[dbnum]->start[index][hash]->fields[g].string);
						else if (dbase[dbnum]->discriminator[g] == 3) printf("%le\t",dbase[dbnum]->start[index][hash]->fields[g].dbl);
					}
					printf("\n");

					for (int k = 0; k<dbase[dbnum]->field_count;k++){
					//printf("%d\t",dbase[dbnum]->discriminator[k]);
					if (dbase[dbnum]->discriminator[k] == 1) scanf("%d",&(dbase[dbnum]->start[index][hash]->fields[k].integer));
					else if (dbase[dbnum]->discriminator[k] == 2){
						sval = (char*)malloc(sizeof(char)*50);
						scanf("%s",sval);
						dbase[dbnum]->start[index][hash]->fields[k].string= sval;
					} else if (dbase[dbnum]->discriminator[k] == 3) scanf("%le",&(dbase[dbnum]->start[index][hash]->fields[k].dbl));
					}
					struct record* newrec = dbase[dbnum]->start[index][hash];
					for (int k = 0; k<dbase[dbnum]->field_count;k++){ // multi hash
						int field_type = dbase[dbnum]->discriminator[k];
						switch (field_type){
							case 1:{ // integer
								//printf("Integer %d\n",hash_int(dbase[dbnum]->start[k],newrec->fields[k].integer));
								dbase[dbnum]->start[k][hash] = NULL;
								dbase[dbnum]->start[k][hash_int(dbase[dbnum]->start[k],newrec->fields[k].integer)] = newrec;
							}break;
							case 2:{ // string
								//printf("String %d\n",hash_string(dbase[dbnum]->start[k],newrec->fields[k].string));
								dbase[dbnum]->start[k][hash] = NULL;
								dbase[dbnum]->start[k][hash_string(dbase[dbnum]->start[k],newrec->fields[k].string)] = newrec;
							}break;
							case 3:{ // double
								//printf("Double %d\n",hash_double(dbase[dbnum]->start[k],newrec->fields[k].dbl));
								dbase[dbnum]->start[k][hash] = NULL;
								dbase[dbnum]->start[k][hash_double(dbase[dbnum]->start[k],newrec->fields[k].dbl)] = newrec;
							}break;
						}
						
					}
					printf("Record successfully editted\n");
				}
			}
		}
	}
}

int hash_int(struct record** memory,int searchkey){
	//what the heck do I do with this searchkey, hash it ofcourse
	int hashorig = searchkey%HASHCONST;
	int hash = hashorig;
	while (memory[hash] != NULL){
		hash = (hash+1)%HASHCONST;
		if (hash==hashorig) return -1;
	}
	return hash;
}

int hash_string(struct record** memory,char* searchkey){
	int hashorig = strlen(searchkey)%HASHCONST; //simple length based hash
	int hash = hashorig;
	while (memory[hash] != NULL){
		hash = (hash+1)%HASHCONST;
		if (hash==hashorig) return -1;
	}
	return hash;
}	

int hash_double(struct record** memory,double searchkey){
	int hashorig = ((int)searchkey)%HASHCONST;
	int hash = hashorig;
	while (memory[hash] != NULL){
		hash = (hash+1)%HASHCONST;
		if (hash==hashorig) return -1;
	}
	return hash;
}
void print_dbases(struct dbase** allbases,int number_of_databases){
	for (int j = 0; j < number_of_databases;j++){
		printf("%d |--> %s\n",j, allbases[j]->name);
	}
}

void print_all_columns_header_form(struct dbase** allbases, int selected_dbase_index){
	for (int g = 0; g < allbases[selected_dbase_index]->field_count;g++){
		printf("%s\t",allbases[selected_dbase_index]->headers[g]);
	}printf("\n");
}

void print_all_columns_selection_form(struct dbase** allbases, int selected_dbase_index){
	for (int g = 0; g < allbases[selected_dbase_index]->field_count;g++){
		printf("%d|-->%s\n",g,allbases[selected_dbase_index]->headers[g]);
	}
	printf("\n");
}
