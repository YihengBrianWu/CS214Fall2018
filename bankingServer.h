typedef struct _Account{
	char name[256];
	float balance;
	int insession;
	struct _Account* next;
}Account;

typedef struct socknode{
	int sock;
	struct socknode* next;
}snode;

Account* getacc(char* name);

int addacc(char* name);

int startacc(char* name);

void* listencon(void* listensock)

void* clientThread(void* pt);