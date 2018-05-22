#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/types.h>
#include <math.h>
#include <sys/sem.h>
#include <semaphore.h>

char inputFile[PATH_MAX];
char outputFile[PATH_MAX];
int n;
sem_t *sem = 0;
       
void copyFile(const char* copy,const char* past, size_t offset, size_t blockSize)
{
	int copyFile, pastFile, n, err;
	int countCopyByte = 0;
    unsigned char buffer[4096];
	copyFile = open(copy, O_RDONLY);
    pastFile = open(past, O_WRONLY);
	if (copyFile == -1)
	{
		fprintf(stderr, "Error open file\n");
        	exit(EXIT_FAILURE);
    	}
	if (pastFile == -1)
	{
		fprintf(stderr, "Error open file.\n");
        	exit(EXIT_FAILURE);
    	}
	lseek(copyFile, offset, SEEK_SET);
	lseek(pastFile, offset, SEEK_SET);
	sem_wait(sem);
	do{
		if (blockSize < 4096){
			countCopyByte = blockSize;
		}
		else{
			countCopyByte = 4096;
		}
	
	}while(blockSize -= write(pastFile, buffer, read(copyFile, buffer, countCopyByte)));
	sem_post(sem);
	
	close(copyFile);
    close(pastFile);
	
}

int main(int argc, char *argv[]){
	sem = sem_open("/sem_1", O_CREAT | O_EXCL, 0777, 1);
	if (argc < 4)
	{
		puts("wrong number of parameters");
		return 1;
	}
	strcpy(inputFile, argv[1]);
	strcpy(outputFile, argv[2]);
	n = atoi(argv[3]);
	size_t offset = 0, blockSize = 0;
	close(open(outputFile, O_CREAT, 0666));
	struct stat stat_dirent;
	if(!stat(inputFile, &stat_dirent)){
		size_t fileSize = stat_dirent.st_size;
		blockSize = (int)round((float)fileSize/n);
		for (int i = 0; i < n; i++){
			if(!fork()){
				//sleep(1);
				if (i == n - 1)
					blockSize = fileSize - offset;
				copyFile(inputFile, outputFile, offset, blockSize);
				return 0;
			}
			else {
				offset += blockSize;
			}
		}
		int stat;
		//system("ps fax");
		for (int i = 0; i < n; ++i)
			wait(&stat);
	}
	else{
		fprintf( stderr,"Error statting %s: %s\n", inputFile, strerror( errno ) );
		return 1;
	}
	sem_close(sem);
	sem_unlink("/sem_1");	
	return 0;
}
