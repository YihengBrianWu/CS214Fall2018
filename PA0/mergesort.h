#ifndef SIMPLECSVSORTER_H
#define SIMPLECSVSORTER_H

void mergeSort(record** headnode);

void split(record* head, record** left, record** right);

record* combine(record* left, record* right);

void getRecord(record** dest, record** src);

int isnumeric(char* field);

int type(record* head);

void print(record** header);