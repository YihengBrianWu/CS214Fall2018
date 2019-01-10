#ifndef SIMPLECSVSORTER_H
#define SIMPLECSVSORTER_H

#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef struct _record{
	
	char* field; //cut space and ""

	char* Complete_Str; //og
	
	struct _record* next;

}record;

void appendLL(char* field, char* Complete_Str);

int findcolumn(char* tarfield, const char* path);

void Linkedlist(int pos, const char* path);

void mergeSort(record** headnode);

void split(record* head, record** left, record** right);

record* combine(record* left, record* right);

void getRecord(record** dest, record** src);

int isnumeric(char* field);

int compare(char* f1, char* f2) ;

void print(record* header);

#endif
