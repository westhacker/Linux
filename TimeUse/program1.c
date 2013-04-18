#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <time.h>
#include <sys/time.h> 
#include <math.h>

//gettimeofday function
int gettimeofday(struct timeval *tv,struct timezone *tz); 

//generator function 
void generator(char filename[])              
{
	int i;
	int n;
	FILE* fp;
	if((fp=fopen(filename,"w+"))==NULL)
	{
		printf("Fail creating file！");
	}
	srand((unsigned)time(NULL));
	for(i=0;i<10000;i++)
	{
		n=rand()%10000;
		fprintf(fp,"%d ",n);
    }
	fclose(fp);
}

//main program
int main(int argc, char * argv[])
{
	char filename[255];

	if(argc>1)
	{
		strcpy(filename,argv[1]);
	}
	else
	{
		printf("Please input the file name to output:");
		scanf("%s",filename);
	}
	
	struct timeval tpstart,tpend; 
	
    double timeuse; 
    
    //get time before generator starts
    gettimeofday(&tpstart,NULL); 
    
    //call generator function
	generator(filename);
	
	//get time after generator ends
	gettimeofday(&tpend,NULL); 
	
	//calculate the used time
    timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec; 
    timeuse/=1000000; 
    
    printf("Used Time:%fsec\n",timeuse);
    return 0;
}




	

