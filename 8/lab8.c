#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <math.h>

int fd[3][2];
int id = 0;
int a,b,c;

int getDiscriminant()
{
	int discriminant = b*b -4*a*c;
	if (discriminant < 0)
	{
		printf("No roots");
		return -1;
	}
	printf ("Discriminant is equal to %d\n", discriminant);
	return discriminant;
}


double* getRoots(int discriminant)
{
	double* roots;
	roots = (double*)calloc(2, sizeof(double));
	roots[0] = (((-b) + sqrt(discriminant))/(2*a));
	roots[1] = (((-b) - sqrt(discriminant))/(2*a));
	printf("%f %f", roots[0], roots[1]);
	return roots;
}

void VietasTheorem(double * roots)
{
	if (round(roots[0] * roots[1]*100) != round((float)c/a*100) || round((roots[0] + roots[1])*100) != round((float)-b/a*100))
		printf("\nCalculations are incorrect\n");
	else
		printf("\nCalculations are correct\n");
}


int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		puts("wrong number of parameters");
		return 1;
	}
	a = atoi(argv[1]);
	b = atoi(argv[2]);
	c = atoi(argv[3]);
	

	for (int i = 0; i < 3; i++)
		pipe(fd[i]);
	for (int i = 1; i < 3; i++)
		if (!fork()){
			id = i;
			break;
		}
	double *roots;
	char buf[256];
	
	switch(id)
	{
		case 0:
			write (fd[0][1], buf, sprintf(buf, "%d", getDiscriminant()));
			break;
		case 1:
			memset(buf, 0, 256);
			read(fd[0][0], buf, 256);
			roots = getRoots(atoi(buf));
			write (fd[1][1], buf, sprintf(buf, "%f %f", roots[0], roots[1]));
			break;
		case 2:
			memset(buf, 0, 256);
			read(fd[1][0], buf, 256);
			char temp[100];
			int indexInString = 0;
			
			roots = (double*)calloc(2, sizeof(double));
			int i = 0;
			while(i < strlen(buf))
			{
				
				if (buf[i] != ' ')
				{
					temp[indexInString] = buf[i];
					indexInString++;
				}
				else
				{
					indexInString = 0;
					roots[0] = atof(temp);
					
				}
				i++;

			}
			roots[1] = atof(temp);
			VietasTheorem(roots);
			break;
	}

	return 0;
}
