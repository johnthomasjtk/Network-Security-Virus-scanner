#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ftw.h>
#include <crypt.h>
#include <openssl/md5.h>

// Files to list all the binary hashes
FILE* hashedExecListPrev;
FILE* hashedExecListCurr;

// required as buffer file to copy it back to reference hash file
FILE* tempHashList;

// hashes the binary files one by one basically.
// takes one binary file, hashes it and puts it into the 
// file that containes hash of all binary files
// Following function is written with the help of stackoverflow based queries.
int hashAndListBinary(char *fileToBeHashed);

// containes hash of all non-infected files.
// copy current hashed values to the reference file.
int createReferHashList(FILE *destFile, FILE *srcFile);

// Comapres previous hash values with current calculated
// hash values
int compareHash(FILE *prevFile, FILE *currFile);

// Takes all the binary files one by one hashes it
// If the hash mismatches from the previous one
// checks if it is infected.

int main(){
	while(1){
		char line[1000];
		char *filePrev = "hashedListPrev.txt";
		char *fileCurr = "hashedListCurr.txt";
		char *fileTemp = "tempHashedList.txt";
		
		// Two files to keep hash list, previous values and
		// values calculated after a fixed time.
		hashedExecListPrev = fopen(filePrev,"r");
		hashedExecListCurr = fopen(fileCurr,"w");

		// File to contain list of all binary files
		FILE *F = fopen("./binaryfile.txt", "r");
		
		if (F == NULL){
			printf("No binary files to be scanned!");
			return 0;
		}

		// start hashing all binary files
		while(fgets(line,2000,F) != NULL){
			line[strcspn(line,"\n")] = 0;
			char *sendFile = (char *)line;
			hashAndListBinary(sendFile);
		}

		fclose(hashedExecListCurr);
		fclose(F);

		// compare hash values
		if(hashedExecListPrev != NULL){
			fseek (hashedExecListPrev, 0, SEEK_END);
    		int size = ftell(hashedExecListPrev);

    		if (0 == size) {
    			// there is no previous hash list to compare with
        		fseek(hashedExecListPrev, 0, SEEK_SET);
        		hashedExecListPrev = fopen(filePrev,"w");
				hashedExecListCurr = fopen(fileCurr,"r");
				createReferHashList(hashedExecListPrev, hashedExecListCurr);
   				fclose(hashedExecListPrev);
   				fclose(hashedExecListCurr);
   			}else{
   				// compare prev hash with current hash
   				hashedExecListPrev = fopen(filePrev,"r");
				hashedExecListCurr = fopen(fileCurr,"r");
   				compareHash(hashedExecListPrev, hashedExecListCurr);
   				fclose(hashedExecListPrev);
   				fclose(hashedExecListCurr);

   				//now copy the temp file to prev file
   				hashedExecListPrev = fopen(filePrev,"w");
				tempHashList = fopen(fileTemp,"r");
				createReferHashList(hashedExecListPrev, tempHashList);
				fclose(hashedExecListPrev); 
				fclose(tempHashList);
   			}		
		}else{
			printf("creates empty hashedListPrev.txt");
		}
		sleep(4);
	}
	return 0;
}

int hashAndListBinary(char *fileToBeHashed){
	fflush(stdout);
	int i;
	int bytes;
	MD5_CTX mdContext;
    unsigned char data[1024];
	unsigned char c[MD5_DIGEST_LENGTH];
    
    FILE *takeFile = fopen(fileToBeHashed,"rb");

    if (takeFile == NULL) {
        printf ("%s can't be opened.\n", fileToBeHashed);
        return 0;
    }

    MD5_Init (&mdContext);
    
    while ((bytes = fread (data, 1, 1024, takeFile)) != 0)
        MD5_Update (&mdContext, data, bytes);
    
    MD5_Final (c,&mdContext);

    for(i = 0; i < MD5_DIGEST_LENGTH; i++){
    	fprintf(hashedExecListCurr,"%02x", c[i]);
    }
    fprintf(hashedExecListCurr,"@%s\n",fileToBeHashed);
    
    fclose (takeFile);
    return 0;
}

int createReferHashList(FILE *destFile, FILE *srcFile){
	size_t l1;
	size_t l2;
	unsigned char buffer[8192];

	while((l1 = fread(buffer, 1, sizeof buffer, srcFile)) > 0) {
		l2 = fwrite(buffer, 1, l1, destFile);
	  	if(l2 < l1) {
    		if(ferror(destFile)){
	    		printf("%s\n","error" );
	    	}
	    else{
	    		printf("%s\n","Handle media full" ); 
	    	}
	    }
	}
	return 0;
}

int compareHash(FILE *prevFile, FILE *currFile){
	char *hashValuePrev;
	char *fileNamePrev;
	char *hashValueCurr;
	char *fileNameCurr;
	char line1[1000];
	char line2[1000];
	int flag1 = 0;
	int flag2 = 0;
	int i=0;
	int j=0;

	char *tempFile = "tempHashedList.txt";
	tempHashList = fopen(tempFile,"w");
	while(fgets(line1, 1000, currFile) != NULL){
		flag1 = 0;
		hashValueCurr = strtok(line1,"@");
		fileNameCurr = strtok(NULL,"\n");
		while(fgets(line2, 1000, prevFile) != NULL){
			hashValuePrev = strtok(line2,"@");
			fileNamePrev = strtok(NULL,"\n");
			if (strncmp(hashValueCurr,hashValuePrev,
				strlen(hashValuePrev)) == 0){
					flag1 = 1;
					break;
			}
		}
		// current calculated hash matched with previous hash
		// take the current hash and put it in temp file
		if (flag1 == 1){
			fprintf(tempHashList, "%s@%s\n",hashValueCurr,fileNameCurr);
		}else{
			// the hash of this particular binary file did not
			// match with previous hash values, check for DEADBEEFDEAD
			// in the binary
			flag2 = 0;
			FILE *file;
			unsigned char *buffer;
			unsigned long fileLen;

			file = fopen(fileNameCurr, "rb");
			if (!file)
			{
				fprintf(stderr, "can't open file %s",fileNameCurr);
				exit(1);
			}

			fseek(file, 0, SEEK_END);
			fileLen=ftell(file);
			fseek(file, 0, SEEK_SET);

			buffer=(unsigned char *)calloc(fileLen+1, sizeof(unsigned char *));

			if (!buffer)
			{
				fprintf(stderr, "Memory error!");
		        fclose(file);
				exit(1);
			}

			fread(buffer, fileLen, 1, file);
			fclose(file);
			for(i=0; i<fileLen; ++i) {
			  	if(buffer[i] == 0xDE) {
			  		i = i+1;
			  		if (buffer[i] == 0xAD){
			  			i = i+1;
			  			if (buffer[i] == 0XBE){
			  				i = i+1;
			  				if(buffer[i] == 0XEF){
			  					i = i+1;
			  					if(buffer[i] == 0xDE) {
							  		i = i+1;
							  		if (buffer[i] == 0xAD){
							  			flag2 = 1;
							  		}
							  	}
							 }
			  			}
					}
				}
			}
			free(buffer);

			// check if DEADBEEFDEAD was present in the file
			// if yes, notify, and do not keep the current hash
			// refernce for next iteration
			if(flag2 == 1)
			{

				printf("File is infected: %s\n",fileNameCurr);
			}
			else
			{
				// hash of binary is changed
				// but the binary is not infected
				// keep this hash value for next iteration
				fprintf(tempHashList, "%s@%s\n",hashValueCurr,fileNameCurr);
			}
		}
	}
	fclose(tempHashList);
	return 0;
}
