#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>

#define M_L	1024
#define BS	1024
#define E		96 
#define DEFAULT_SIZE	8192

typedef struct F_B
{
	char name[M_L];
	char times[M_L];
	unsigned int size;
	char *data;
	struct F_B *next;
}F_B_t;

void init();
void readf(const char *fnames);
void savef(const char *fnames);
void put(const char *fnames);
void get(const char *fnames);
void removes(const char *fnames);
void paths();
void ends(char *fnames);

char *getArg(char *cmd);
void addData(F_B_t *cn , FILE *fd);

F_B_t *head;

int main()
{
	char currCmd[M_L];
	char buffer[BS];
	char fnames[M_L];
	char *arg1 , *arg2;

	init();

	while(1)
	{
		printf("PFS>");
		fgets(currCmd , M_L , stdin);
		arg1 = currCmd;
		arg2 = getArg(currCmd);

		if (!strcmp(arg1 , "open"))
		{
			readf(arg2);
			strcpy(fnames , arg2);
		}
		else if (!strcmp(arg1 , "put"))
		{
			put(arg2);
		}
		else if (!strcmp(arg1 , "get"))
		{
			get(arg2);
		}
		else if (!strcmp(arg1 , "removes"))
		{
			removes(arg2);
		}
		else if (!strcmp(arg1 , "paths"))
		{
			paths();
		}
		
		else if(!strcmp(arg1 , "ends"))
		{
			ends(fnames);
			exit(0);
		}
		else if (!strcmp(arg1 , "quit"))
		{
			break;
		}
		else
			printf("command input error.\n");
	}

	savef(fnames);

	return 0;
}

void init()
{
	head = (F_B_t *)(malloc(sizeof(F_B_t)));
	if (!head)
	{
		printf("malloc memory  error.\n");
		exit(1);
	}

	head->next = NULL;

	return ;
}

void readf(const char *fnames)
{
	F_B_t *cn = NULL;
	FILE *fd = NULL;

	fd = fopen(fnames , "w+");

	if (!fd)
	{
		printf("file open error.\n");
		return ;
	}

	while(1)
	{
		cn = (F_B_t *)(malloc(sizeof(F_B_t)));
		if (!cn)
		{
			printf("malloc memory  error.\n");
			exit(1);
		}
		cn->size = 0;

		cn->data = (char *)(malloc(sizeof(char) * DEFAULT_SIZE));

		if (fscanf(fd , "%s" , cn->name) < 1)
			break;
		fgetc(fd);
		fgets(cn->times , M_L , fd);
		fgetc(fd);
		if (fscanf(fd , "%d" , &cn->size) < 1)
			break;

		fgetc(fd);//jump ' '

		addData(cn , fd);

		cn->next = head->next;
		head->next = cn;
	}

	fclose(fd);

	return ;
}

void addData(F_B_t *cn , FILE *fd)
{
	int tmpCh = 0;
	int index = 0;

	while( (tmpCh = fgetc(fd)) != EOF)
	{
		if (tmpCh == E)
			break;

		if (index >= DEFAULT_SIZE)
			++ cn->size;
		else
		{
			++ cn->size;
			cn->data[index ++] = tmpCh;
		}
	}

	cn->data[index] = E;

	return ;
}

void savef(const char *fnames)
{
	F_B_t *cn = head->next;
	FILE *fd = NULL;
	int index = 0;

	fd =  fopen(fnames , "w");
	if (!fd)
	{
		printf("file open error.\n");
		return ;
	}

	while(cn)
	{
		fprintf(fd , "%s\n" , cn->name );
		fputs(cn->times , fd);
		fprintf(fd , "%d\n" , cn->size);

		for (index = 0 ; index < cn->size && cn->data[index] != E ; ++ index)
			fprintf(fd , "%c" , cn->data[index]);

		fputc(E , fd);
		cn = cn->next;
	}

	return ;
}

void put(const char *fnames)
{
	FILE *fd = NULL;
	F_B_t *cn = NULL;
	char *tmp = NULL;
	time_t timer;
	time(&timer);
	int currCh = 0;

	fd = fopen(fnames , "r");
	if (!fd)
	{
		printf("file open error.\n");
		return ;
	}

	cn = (F_B_t *)(malloc(sizeof(F_B_t)));
	cn->data = (char *)(malloc(sizeof(char) * DEFAULT_SIZE));
	if (!cn || !cn->data)
	{
		printf("malloc memory error.\n");
		return ;
	}
	cn->size = 0;
	strcpy(cn->name , fnames);
	tmp = ctime(&timer);
	strcpy(cn->times , tmp);

	addData(cn , fd);

	cn->next = head->next;
	head->next = cn;

	fclose(fd);

	return ;
}

void get(const char *fnames)
{
	FILE *fd = NULL;
	F_B_t *p = head->next;
	int index = 0;

	while(p)
	{
		if (!strcmp(p->name , fnames))
			break;
		p = p->next;
	}

	if (!p)
	{
		printf("file not exist.\n");
		return ;
	}

	fd = fopen(fnames , "w");
	if (!fd)
	{
		printf("file open error.\n");
		return ;
	}

	for (index = 0 ; index < p->size && p->data[index] != E ; ++ index)
		fputc(p->data[index] , fd);

	fclose(fd);

	return ;
}

void removes(const char *fnames)
{
	F_B_t *p = head;
	F_B_t *tmp;

	while(p->next)
	{
		if (!strcmp(p->next->name , fnames))
		{
			tmp = p->next;
			p->next = p->next->next;
			free(tmp->data);
			free(tmp);
		}
		else
			p = p->next;
	}

	return ;
}

void paths()
{
	F_B_t *p = head->next;

	while(p)
	{
		printf("%s\t%d\t%s\n" , p->name , (p->size / 1024 + 1) , p->times );
		p = p->next;
	}

	return ;
}



void ends(char *fnames)
{
	if (remove(fnames))
	{
		printf("remove error.\n");
		return ;
	}

	return ;
}

char *getArg(char *cmd)
{
	char *p = cmd;
	char *results;

	while( *p && *p != ' ')
		++ p;

	if (*(p - 1) == '\n')
		*(p - 1) = 0;

	if (!*p)
		return NULL;

	*p = 0;

	++ p;
	results = p;

	while(*p)
	{
		if (*p == '\n')
			*p = 0;

		++ p;
	}

	return results;
}
