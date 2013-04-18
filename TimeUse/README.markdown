# OS-Linux Demos and Examples

A collection of OS (LINUX) experiments I've created, now open source and on GitHub, so please go ahead and help me hack this resource in to a wealth of demos that other authors can learn from.

# Aim

* If a user can hit view source on the demo, then we've done our job
* All content is open source and just enjoy it
* Individual demos, if authored by someone other than [@westhacker](http://twitter.com) can be credited as appropriate

# Language

* C in Linux

# TimeUse

* 产生一个包含10000个随机数的文本文件，报告计算花费的时间（屏幕显示）
* 文件的名称由命令行的参数给出

# Content

<pre><code>
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
</code></pre>

# Thanks

Thanks for your reading and happy coding
