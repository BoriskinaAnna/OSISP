#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

int fd[4][4][2];
int id = 0;

int main ()
{
	char buf[256];
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			if (i != j)
				pipe(fd[i][j]);
	for (int i = 1; i < 4; ++i)
		if (!fork()){
			id = i;
			break;
		}
	for (int i = 0; i < 4; ++i)
		if (i != id)
			write (fd[id][i][1], buf, sprintf(buf, "I'm process number %d, it's %d millies", getpid(), clock()));          	
	fd_set readset;
	struct timeval timeout = {1, 0};
	for (int i = 0; i < 4; ++i)
		if (i != id){
			FD_ZERO(&readset);
			FD_SET(fd[i][id][0], &readset);
			if (select(25, &readset, 0, 0, &timeout)){
				memset(buf, 0, 256);
				read(fd[i][id][0], buf, 256);
				printf("Process %d got <%s>\n", getpid(), buf);
			}
		}
	return 0;
}
