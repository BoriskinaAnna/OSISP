#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>


int firstProcess, root;
int pgid;
int number = 0;
int children[3] = {0};
int forexit = 0;
#define LOOPS 3

void handler1(int sig)
{
	printf("Process %d / %d got signal SIGUSR1 from process %d %d\n", number, getpid(), number==1 ? 0 : getppid(), clock());
	sleep(0.3);
	//printf("               %d             ", number);
	switch (number)
	{
		case 0:
			//printf("jfdjnkfndfj");
			//if(forexit > LOOPS){
			//	printf("jfdjnkfndfj");
			//	int stat;
			//	wait(&stat);
			//	system("ps fax");
				
			//}
			break;
		case 8:
			kill(firstProcess, SIGUSR2);
			printf("Process %d / %d sent signal SIGUSR2 to process %d %d\n", number, getpid(), firstProcess, clock());
printf("nnnn %d nnnnnnn", firstProcess);
			break;
		case 7:
			kill(children[0], SIGUSR1);
			printf("Process %d / %d sent signal SIGUSR1 to process %d %d\n", number, getpid(), children[0], clock());
			break;
		case 2:
				killpg(children[0], SIGUSR2);
				printf("Process %d / %d sent signal SIGUSR2 to process %d %d %d\n", number, getpid(), children[0], children[1], clock());
			break;
		 
		case 3:
		case 4:
			break;
	
	}	
}

void handler2(int sig)
{
	printf("Process %d / %d got signal SIGUSR2 from process %d %d\n", number, getpid(), getppid(), clock());
	sleep(0.3);
	switch (number)
	{
		
		case 1:
		if (forexit++ < LOOPS)
		{
				killpg(children[0], SIGUSR1);
				printf("Process %d / %d sent signal SIGUSR1 to process %d %d %d %d\n", number, getpid(), children[0], children[1], children[2], clock()); 
		}
		else 
			{
				//kill(root, SIGUSR1);
				//printf("aaaaaaaaaaaaaaaaa");
				int stat;
				for (int i= 0; i<3; ++i)
					wait(&stat);
				//system("ps fax");

			}
			break;
		case 6:
			kill(children[0], SIGUSR1);
			printf("Process %d / %d sent signal SIGUSR1 to process %d %d\n", number, getpid(), children[0], clock());
			break;
			
		case 5:
			break;
	}
	
}


int main(int argc, char *argv[])
{
	int count = 0, temp = 0;
	root = getpid();

	if(number == 0 && count == 0)
	{
		if (temp = fork())
		{
			children[count++] = temp;
		}
		else
		{
			number = 1;
			count = 0;
			firstProcess = getpid();
			
			
		}

	}

	while(number == 1 && count < 3)
	{
		if (temp = fork())
		{
			if(count == 0){
				pgid = temp;
			}
			children[count++] = temp;
		}
		else
		{

			number = count + 2;	
			setpgid(getpid(), pgid);
			count = 0;
		}

	}
	
	while(number == 2 && count < 2)
	{
		if (temp = fork())
		{
			if(count == 0){
				pgid = temp;
			}
			children[count++] = temp;
		}
		else
		{
			number = count + 5;
			setpgid(getpid(), pgid);
			
			count = 0;
		}

	}

	if(number == 6)
	{
		if (temp = fork())
		{
			children[count++] = temp;
		}
		else
		{
			number = 7;
			count = 0;
		}

	}
	if(number == 7)
	{
		if (temp = fork())
		{
			children[count++] = temp;
		}
		else
		{
			number = 8;
			count = 0;
		}

	}
	//sleep(2000);
	struct sigaction act;
	memset(&act, 0, sizeof(act));//инициализируем память нулями, т к си гг
	act.sa_handler = handler1;
	sigaction(SIGUSR1, &act, 0);
	act.sa_handler = handler2;
	sigaction(SIGUSR2, &act, 0);
	if (number == 0)
	{
		system("ps fax");
		printf("ffff  %d  fffff", firstProcess);
		kill(firstProcess, SIGUSR2);

printf("ddddd  %d  dddd", firstProcess);
printf("Process %d / %d sent signal SIGUSR2 to process %d %d\n", number, getpid(), firstProcess, clock());
	}

	int i = 0;
	while (i++ <= LOOPS)
		sleep(1);
	return 0;
}
