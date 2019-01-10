
#include<stdio.h> 
#include<stdlib.h> 
#include<string.h> 
#include<ctype.h>

#include"simpleCSVsorter.h"
//#include"mergesort.h"

void mergeSort(record** headnode) {
	record* headPtr = *headnode;
	record* left;
	record* right;

	if (headPtr == NULL || headPtr->next == NULL) {
		return;
	}

	split(headPtr,&left,&right);

	mergeSort(&left);
	mergeSort(&right);

	*headnode = combine(left, right);
}

void split(record* head, record** left, record** right) {
	record* front;
	record* behind;

	front = head;
	behind = head->next;

	while (behind != NULL) {					//behind goes two time faster than the front
		behind = behind->next;
		if (behind != NULL) {
			front = front->next;
			behind = behind->next;
		}
	}
	*left = head;
	*right = front->next;
	front->next = NULL;
}

record* combine(record* left, record* right) { //combine splited list to head
	record temp;
	record* end = &temp;
	temp.next = NULL;

	while (1) {
		if (left == NULL) {
			end->next =  right;
			break;
		}
		else if (right == NULL) {
			end->next = left;
			break;
		}

		if (compare(left->field,right->field) == 0) {
			getRecord(&(end->next), &left);
		}
		else {
			getRecord(&(end->next), &right);
		}

		end = end->next;
	}
	return (temp.next);
}

void getRecord(record** dest, record** src) {   //move src to dest
	record* temp = *src;
	if (temp == NULL) {
		printf("getRecord src=NULL\n");
		return;
	}
	*src = temp->next;
	temp->next = *dest;
	*dest = temp;
}

int compare(char* f1, char* f2) {   // 0 for equal or f1 bigger,1 for f2 bigger

	if (isnumeric(f1) == 0) {
		if (isnumeric(f2) ==0) {
			if (atoi(f1) <= atoi(f2)) {
				return 0;
			}
			else {
				return 1;
			}
		}
		else {
			return 1;
		}
	}
	else if (isnumeric(f2) == 0) {
		return 0;
	}
	else {
		if (strcmp(f1,f2) <= 0) {
			return 0;
		}
		else {
			return 1;
		}
	}
}	



int isnumeric(char* field) {
	

	int length = strlen(field);
	int i = 0;
	int j = 0;

	if (field[0] == '\0') {
		return -1;
	}
	while (isdigit(field[i]) == 1) {
		i++;
	}


	if (i == length) {
		return 0;
	}
	else if (field[i] == '.') {
		j = i + 1;
		while (isdigit(field[j]) == 1) {
			j++;
		}
	}

	if (j == length) {
		return 1;
	}

	return -1;
}

/*
int type(record* head) {


	record* tmp = head;

	while (tmp->next != NULL) {
		if (tmp.field == NULL) {
			tmp->tmp.next;
		}
		else {
			return isnumeric(tmp.field);
		}
	}

	return 2;
	

}
*/

