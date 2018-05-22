#define _GNU_SOURCE

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
#include <pthread.h>
#include <sys/syscall.h>

#define COPY_BUF_SIZE 1024
#define CHAR_BUF_SIZE 256
#define gettid() syscall(SYS_gettid)


char *module;
int max_count_theards;

typedef struct  _param{
    char copy[PATH_MAX];
	char past[PATH_MAX];
    int xwr;
	int theardsNumber;
} param;

pthread_t * theards;
int * freePlaces;






int cpyFile(const char* copy,const char* past, int xwr)
{
	int copyFile, pastFile, n, err;
	int countBite = 0;
    unsigned char buffer[4096];
	copyFile = open(copy, O_RDONLY);
    pastFile = open(past, O_CREAT|O_WRONLY);
	if (copyFile == -1)
	{
		fprintf(stderr, "Error open file\n");
		return 1;
        	//exit(EXIT_FAILURE);
    	}
	if (pastFile == -1)
	{
		fprintf(stderr, "Error open file.\n");
		return 1;
        	//exit(EXIT_FAILURE);
    	}
	while (1) 
	{
		err = read(copyFile, buffer, 4096);
        if (err == -1) 
		{
			fprintf(stderr, "Error reading file.\n");
         	exit(EXIT_FAILURE);
        }
		n = err;
		countBite +=err;
		if (err == 0) break;
			err = write(pastFile, buffer, n);
        if (err == -1) 
		{
			fprintf(stderr, "Error writing to file.\n");
         	exit(EXIT_FAILURE);
        }
	}
	close(copyFile);
    close(pastFile);
	chmod(past, xwr);
	return countBite;
}



void* threadsFunction(void *args) {
	param *info = (param*)args;
	int countByte = cpyFile(info->copy, info->past, info->xwr);
	fprintf (stderr, "tid = %d, pid=%d, path: %s, count copybyte: %d\n", gettid(), getpid(), info->past, countByte);
	freePlaces[info->theardsNumber] = 1;
	free(args);
	
}



int SearchDifferentFiles(const char *path_dir1, const char *path_dir2)
{
	DIR *ptrDir1 = opendir(path_dir1);
	DIR *ptrDir2 =opendir (path_dir2);

	struct dirent *currFile1;
	struct dirent *currFile2;
	struct stat stat_dirent1;
	struct stat stat_dirent2;
	char temp[PATH_MAX] = "";
	char fullPath1[PATH_MAX] = "";
	char fullPath2[PATH_MAX] = "";
	int process_count = 0;	
	if (ptrDir1 != NULL && ptrDir2 != NULL)
	{
		while(currFile1 = readdir(ptrDir1))
		{
			strcpy(fullPath1, path_dir1);
			strcat(fullPath1, "/");		
			strcat(fullPath1, currFile1->d_name);
			if (!stat(fullPath1, &stat_dirent1)) 
			{
		    	if ( strcmp( currFile1->d_name, ".")  &&  strcmp( currFile1->d_name, "..") ) 
				{
					
			   		if (S_ISREG(stat_dirent1.st_mode) )
					{
			      		int isFind = 0;
						strcpy(temp, path_dir2);
						strcat(temp, "/");
						strcat (temp, currFile1->d_name);
						
			      		while(currFile2 = readdir(ptrDir2))
						{
				 			strcpy(fullPath2, path_dir2);
				 			strcat(fullPath2, "/");		
				 			strcat(fullPath2, currFile2->d_name);

				 			if (!stat(fullPath2, &stat_dirent2)) 
							{
				    				if ( strcmp( currFile1->d_name, ".")  &&  strcmp( currFile1->d_name, ".." ) ) 
								{
									if (S_ISREG(stat_dirent2.st_mode) )
									{
									    if (!strcmp(currFile1->d_name, currFile2->d_name))
										{
					      					isFind = 1;
					      					break;

					   					}
									}

				    				}
				 			}
							else
							{
						   		printf( "Error statting %s: %s\n", fullPath2, strerror( errno ) );
						   		return 1;
						 	}
			       		}		
			       		if (!isFind){
				  			int i = 0;
							while(1)
							{
								
								if(i == max_count_theards)
									i = 0;
								if (freePlaces[i])
										break;
								i++;
							}
							
							freePlaces[i] = 0;
							param* parameter = (param *)malloc(1*sizeof(param));
							parameter->xwr  = stat_dirent1.st_mode;
							parameter->theardsNumber = i;
							strcpy(parameter->copy, fullPath1);
							
							strcpy(parameter->past, temp);
							
							int result;
				  			result = pthread_create(theards + i, NULL, threadsFunction, (void*)parameter);
							//sleep(0.5); 
							if (result != 0) {
								fprintf( stderr,"Error creating new thread %s: %s\n", fullPath1, strerror( errno ) );
								return 1;
							}
						}	
					}
	  			}
			}
			else
			{
		   		fprintf( stderr,"Error statting %s: %s\n", fullPath1, strerror( errno ) );
		   		return 1;
         	}	
		}
	}		
	else{
		fprintf( stderr,"Error opening %s: %s", path_dir1, strerror( errno ) );
	}	
	
	closedir(ptrDir1);	
	closedir (ptrDir2);
	return 0;
}



int main(int argc, char *argv[]){

	module = basename(argv[0]);
	if (argc < 4)
	{
		puts("wrong number of parameters");
		return 1;
	}

    char dir1_realpath[PATH_MAX];
    char dir2_realpath[PATH_MAX];
    char* dir1 = realpath(argv[1], dir1_realpath);
    char* dir2 = realpath(argv[2], dir2_realpath);
	if (dir1 == NULL)
	{
		fprintf( stderr,"Can't open directory");
		return 1;
	}
	if (dir2 == NULL)
	{
		fprintf( stderr,"Can't open directory");
		return 1;
	}

	max_count_theards = atoi(argv[3]);	
	if (max_count_theards < 2)
	{
		fprintf( stderr,"Can't open directory");
		return 1;
	}
	theards = (pthread_t*) calloc(max_count_theards, sizeof(pthread_t));
	freePlaces = (int*) calloc(max_count_theards, sizeof(int));
	if (theards == NULL || freePlaces == NULL)
	{
		fprintf(stderr, "Error create array threads\n");
      		exit(1);
	} 
	for (int i = 0; i < max_count_theards; i++)
	{
		freePlaces[i] = 1;
		theards[i] = 0;	
	}	


	
	SearchDifferentFiles(dir1_realpath, dir2_realpath);

	for (int i = 0; i < max_count_theards; i++)
		pthread_join(theards[i], NULL);
	free(freePlaces);
	free(theards);
		
	return 0;
}
