#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "simpleCSVsorter.h"
#include "mergesort.c"

//record* head = NULL;





void RunProgram(char* path, char* target, char* csvfile)
{


	int pid = fork();
	int csvlen = (int)strlen(csvfile) + 1;
	int targetlen = (int)strlen(target) + 1;

	char* csvOutputFile = (char*)malloc((csvlen+targetlen+12)*sizeof(char));
	snprintf(csvOutputFile, sizeof(csvOutputFile), "%s-sorted-%s.csv", target, csvfile);

	
	int status = 0;

	if(pid == 0)
	{
		
		head = (record*)malloc(sizeof(record));
		head = NULL;

		int position = findposition(target, path);

		if(position == 0)
		{
			printf("Couldn't locate target input.\n");
			exit(0);
		}
		//printf("hi\n");
		//buildlist(position, path);
		int sizeoffile = buildlist(position, path);

		//printf("hi\n");
	   	MergeSort(&head, finaltype);
	   	//printf("hi\n");
	   	printList(head, finaltype, sizeoffile, csvOutputFile);
	}

	
   	wait(&status);
   	free(csvOutputFile);
	free(head);
	return;
}



void dirsort(char* path, char* col_name)	
{

	struct dirent *de
    DIR *dr = opendir(path);
    int status = 0;

    if(dr == NULL){
    	printf("can't open this path: %s", path);
    	return;
    }

    while(de = readdir(dr) £¡= NULL){

    	char newpath[1024];

    	if(de -> d_type == DT_DIR){

    		if(strcmp(de -> d_name, ".") == 0 || strcmp(de -> d_name, "..") == 0){

    			continue;

    		}

    		snprintf(newpath, sizeof(newpath), "%s/%s", path, de -> d_name);
    		int pid = fork();
    		if(pid == 0){

    			dirsort(newpath, col_name);

    		}

    		wait(&status);

    	}

    	else{

    		int i = 0;
    		int flag = 0;
    		while(de -> d_name[i] != '\0'){
    			if(de -> d_name[i] == '.'){
    				i++;
    				if(de -> d_name[i] == 'c'){
    					i++;
    					if(de -> d_name[i] == 's'){
    						i++;
							if(de -> d_name[i] == 'v'){
								i++;{
									if(de -> d_name[i] == '\0'){
										flag = 1;
									}
								}
							}
    					}
    				}
    			}
    			i++;
    		}
    		if(csv == 1){

    			int pid = fork();
    			if(pid == 0){
    				head = (record*)malloc(sizeof(record));
					head = NULL;

					int position = findposition(target, path);

					if(position == 0)
					{
						printf("Couldn't locate target input.\n");
						exit(0);
					}

					Linkedlist(current_column);

					mergeSort(&head);

					print(head);
    			}

    			wait(&status);

    		}




    	}

    }

    closedir(dr);
    return;

}






int main(int argc, char *argv[])
{

	if(argc != 3){

		fprintf(stderr,"Wrong number of arguments\n");
		return 0;
	}

	if(strcmp(argv[1],"-c") != 0){
		fprintf(stderr,"Wrong argument\n");
		return 0;
	}

	int columnTosort = findcolumn(argv[2]);

	if(columnTosort == 0) {
		fprintf(stderr,"Column not exists\n");
		return 0;
	}

	Linkedlist(columnTosort);

	mergeSort(&head);

	print(head);

	return 0;

}

/*

*/
char* trim(char* str) {

	int length = strlen(str);
	char ans[length - 1];

	int i;

	if (length == 0) {
		return str;
	}

	int empty = 0;
	for (i = 0; i < length; i++) {
		if (str[i] != ' ') {
			empty = 1;
		}
	}

	if (empty == 0) {
		return "\0";
	}

	if (str[0] != ' ' && str[length - 1] != ' ') {
		return str;
	}

	else if (str[0] == ' ' && str[length - 1] != ' ') {
		for (i = 1; i < length; i++) {
			ans[i - 1] = str[i];
		}
		ans[i - 1] = '\0';
	}

	else if (str[0] != ' ' && str[length - 1] == ' ') {
		for (i = 0; i < length; i++) {
			ans[i] = str[i];
		}
		ans[i - 1] = '\0';
	}
	else {
		for (i = 1; i < length; i++) {
			ans[i - 1] = str[i];
		}
		ans[i - 1] = '\0';
	}

	return strdup(ans);
}



void appendLL(char* field, char* Complete_Str) {

	record* append = (record*)malloc(sizeof(record));
	record* curr = head;

	char* trim_field = trim(field);


	int fieldlength = strlen(field) + 1;
	int stringlength = strlen(Complete_Str) + 1;

	char* fie = (char*)malloc(fieldlength * sizeof(char));
	char* str = (char*)malloc(stringlength * sizeof(char));

	strcpy(fie,trim_field);
	strcpy(str, Complete_Str);



	append->field = fie;
	append->Complete_Str = str;
	append->next = NULL;

	if (head == NULL) {
		head = append;
	}
	else {
		while (curr->next != NULL) {
			curr = curr->next;
		}
		curr->next = append;
	}


	return;

}



int findcolumn(char* name){

	int length = strlen(name);
	char readIn = NULL;


	int column = 0;
	int total = 0;
	int i = 0;
	int j = 0;
	int match = 0;

	char* headbuffer = (char*)malloc(sizeof(char)*10000);


	while(readIn != '\n'){

		read(STDIN, &readIn, 1);
		headbuffer[j++] = readIn;

		if(readIn == ','){
			total++;
		}

		if(match == 1){

			if(readIn == ','){
				column = total;
				match = 0;
			}
			else if(isspace(readIn)){
				column = total+1;
				match = 0;
			}
			else{
				match = 0;
				i = 0;
			}

		}

		if(readIn == name[i]){
			i++;

			if(i == length){
				match = 1;
			}

		}else{
			i = 0;
		}


	}

	headbuffer[j] = '\0';

	if(column == 0){
		free(headbuffer);
		return column;
	}else{
		printf("%s", headbuffer);
	}

	free(headbuffer);
	return column;




}

void Linkedlist(int pos) {

	char readIn = NULL;
	int status = 1;

	int fieldsize = 250;
	int stringsize = 500;

	int i = 0;
	int j = 0;

	int current_column = 0;

	int inquotation = 0;



	char* fieldbuffer = (char*)malloc(fieldsize * sizeof(char));
	char* stringbuffer = (char*)malloc(stringsize * sizeof(char));

	pos -= 1;




	status = read(STDIN, &readIn, 1);

	while (status > 0) {

		if (i <= stringsize) {


			stringbuffer[i] = readIn;
			i++;

		}
		else {


			stringsize += stringsize;
			stringbuffer = (char*)realloc(stringbuffer, stringsize);
			stringbuffer[i] = readIn;
			i++;


		}

		if(current_column == pos && readIn == '\n'){
			stringbuffer[i] = '\0';
			fieldbuffer[j-1] = '\0';

			inquotation = 0;

			current_column = 0;


			i = 0;
			j = 0;

			appendLL(fieldbuffer, stringbuffer);
		}

		else if (current_column == pos) {


	

			if (readIn == '\"' && inquotation == 0) {
				inquotation == 1;
				status = read(STDIN, &readIn, 1);
				continue;
			}
			else if (readIn == '\"' && inquotation == 1) {
				inquotation == 0;
				status = read(STDIN, &readIn, 1);
				continue;
			}

			if (readIn == ','&& inquotation == 0) {

				current_column++;
			}
			else {
				if (j <= fieldsize) {
					fieldbuffer[j] = readIn;
					j++;

				}
				else {
					fieldsize += fieldsize;
					fieldbuffer = (char*)realloc(fieldbuffer, fieldsize);
					fieldbuffer[j] = readIn;
					j++;

				}
			}
		}
		else if (readIn == '\"') {

			if (inquotation == 0) {

				inquotation = 1;

			}
			else {

				inquotation = 0;

			}


		}
		else if (readIn == ',') {

			if (inquotation == 0) {

				current_column++;

			}


		}
		else if (readIn == '\n') {

			stringbuffer[i] = '\0';
			fieldbuffer[j] = '\0';

			inquotation = 0;

			current_column = 0;


			i = 0;
			j = 0;

			appendLL(fieldbuffer, stringbuffer);


		}

		status = read(STDIN, &readIn, 1);

	}



	free(fieldbuffer);
	free(stringbuffer);
	return;

}



void print(record* head){
	record* tmp = head;
	while(tmp != NULL){
		printf("%s",tmp -> Complete_Str);
		tmp = tmp -> next;
	}
}
