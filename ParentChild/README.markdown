# DataStructure Demos and Examples

A collection of OS (LINUX) experiments I've created, now open source and on GitHub, so please go ahead and help me hack this resource in to a wealth of demos that other authors can learn from.

# Aim

* If a user can hit view source on the demo, then we've done our job
* All content is open source and just enjoy it
* Individual demos, if authored by someone other than [@westhacker](http://twitter.com) can be credited as appropriate

# Language

* C in Linux

# ParentChild

* parent-child:调用a)的结果，产生一个包含10000个随机数的文本文件
* parent 进程: 产生 child 进程，从用户处得到文件的位置与名称，告诉child进程文件的位置与名称，接受 child 进程传回的信息，显示 child 进程计算的工作时间。
* child 进程: 从 parent 进程处得到文件的名称，产生一个包含10000个随机数的文本文件
* 调用TimeUse产生的可执行文件，并将所花费的时间报告给父进程，退出。

# Content

<pre><code>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
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

int main(int argc, char * argv[])
{
	//get filename from the user
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
	
	//define two pipes
	int filename_fd[2],time_fd[2];
	
	//define two pids
    pid_t father_pid,child_pid;
    
    //get father's pid
    father_pid = getpid();
    
    //test pipe
    if(pipe(filename_fd)<0)
    {
        printf("pipe create error ");
        return -1;
    }
    
    if(pipe(time_fd)<0)
    {
        printf("pipe create error ");
        return -1;
    }
    
    
    int status;
    
    //create child's process
    child_pid = fork( );
    
    //father process
    if (child_pid != 0) {
    printf("This is the parent process, with id %d. \n",(int) getpid());
    printf("My child process ID is %d .\n",(int) child_pid );
    
    //write filename into pipe
    close(filename_fd[0]);
    write(filename_fd[1],filename,sizeof(filename));
    
    //wait for child's exiting
    waitpid(-1, &status, 0);
    printf("The child process exited \n");
    
    //read usetime from pipe
    char read_time[255];
    close(time_fd[1]);
    read(time_fd[0],read_time,255);
    
    printf("I'm father.Used time of my child:%ssec\n",read_time);
    } 
    
    //child process
    else{
    printf("This is the child process, with id %d\n", (int) getpid ());
    printf("My father's id %d\n", (int) father_pid);
    
    //read filename from pipe
    char read_filename[255];
    close(filename_fd[1]);
    read(filename_fd[0],read_filename,255);
    
    struct timeval tpstart,tpend; 
	
    double timeuse; 
    
    //get time before generator starts
    gettimeofday(&tpstart,NULL); 
    
    //call generator function
	generator(read_filename);
	
	//get time after generator ends
	gettimeofday(&tpend,NULL); 
	
	//calculate the used time
    timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec; 
    timeuse/=1000000; 
    
    //write usetime into pipe
    char s[255]={0};
    sprintf(s, "%lf", timeuse);
    close(time_fd[0]);
    write(time_fd[1],s,sizeof(s));
    
    exit(0);
    }
    
    printf("Please press any key ...... \n"); getchar();
    return 0;
}

</code></pre>

# Thanks

Thanks for your reading and happy coding
