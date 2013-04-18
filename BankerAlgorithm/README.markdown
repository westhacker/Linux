# DataStructure Demos and Examples

A collection of OS (LINUX) experiments I've created, now open source and on GitHub, so please go ahead and help me hack this resource in to a wealth of demos that other authors can learn from.

# Aim

* If a user can hit view source on the demo, then we've done our job
* All content is open source and just enjoy it
* Individual demos, if authored by someone other than [@westhacker](http://twitter.com) can be credited as appropriate

# Language

* C in Linux

# Banker's Algorithm

* 实现单种资源的银行家算法（4个客户）
* main-thread 产生 4 个 work-threads
* 银行家 （main-thread） 接受顾客的贷款计划，判断顾客的分期贷款的安全性，并做出响应。
* 顾客 （work-threads）首先提出贷款目标，并分期向银行家提出贷款的请求。
* 查看程序的运行情况： 使用信号机制，显示当前银行家的状况、顾客的状况，资金的状况...
* 设计了一个菜单，管理贷款的总额，客户的信息，客户的目标...
可以让程序多次运行菜单。
管理的算法有效的话贷款不会失败，成功返回以后，可以启动下一回的运行...

# Content

<pre><code>
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <sys/wait.h>


#define N 4                    //number of customers
pthread_t ThreadId[N];

pthread_mutex_t Mutex;

int Available = 10;            //Available money
int Goal[N]= {6,5,4,7};		   //Goal of customers
int Hold[N] = {0,0,0,0};       //holding money of customers
int Need[N];				   //need of customers
int Request;				   //request of customer
int cusflag;				   //cusflag decides which customer to request money
int Left;					   //Left available money after requesting
int Finish[N];				   //execuable or not

void init()
{
	cusflag = -1;
	//calculate need of customers
	int i,j;
	for(j=0; j<N; ++j)
		Finish[j] = 1;
	Left = Available;
	for(i=0; i<N; ++i)
	{
		Need[i]= Goal[i] - Hold[i];
    }
	printf("=================================OS program3====================================\n");
}

int CheckSafty(int thr)
{
	//Initialize data
	int i,j,k;
	Left = Available;
	for(j=0; j<N; ++j)
		Finish[j] = 0;
		
	int customer[N];			//queue of safe customers
	int cnt = 1;				//number of safe customers
	customer[cnt-1] = thr;
	int iflag = 1;	            //safe or not
	Finish[thr] = 1;
	
	if(Need[thr]<=Available){
	printf("Request of customer %d  is safe!\n",thr);
	return 1;
    }
    else{

	//Critical section
	pthread_mutex_lock(&Mutex);
	for(k=0; k<N; ++k)
	{
		for(i=0; i<N; ++i)
		{
			if(!Finish[i])
			{
					if(Need[i] > Left) 
					{iflag = 0;}
				if(iflag)
				{
					customer[cnt++] = i;   //add to queue of safe customers
					Finish[i] = 1;         //execuable
					Left += Hold[i];
				}
				else iflag = 1;
			}
		}
		if(cnt == N) break;
	}
	pthread_mutex_unlock(&Mutex);

	//check if all customers are safe
	if(cnt != N)
		return 0;

	//show information
	printf("Request of customer %d  is safe!\nThe safe order of customers is:\n",thr);
	for(i=0; i<N; ++i)
		printf("%d ",customer[i]);
	putchar('\n');
	return 1;
}
}

void menu()
{
	int i;
	printf("=================================infomation====================================\n");
	printf("Hello,I am the banker. The information has been summed up as follows:\n");
	printf("Available money:              %d\n",Available);
	for(i=0; i<N; ++i)
	printf("Customer %d holds money:       %d      Goal:%d      Need:%d     State:%d\n",i,Hold[i],Goal[i],Need[i],Finish[i]);
	printf("=================================menu==========================================\n");
	printf("q -- exit   r -- request\n");  
	
}

int RequestMoney(int thr)
{
	int j;
	//check if exceeded
	if(Request > Need[thr])
		return -1; 
	
	//check if available
	if(Request > Available)
		return -2;
		
	//Critical section
	pthread_mutex_lock(&Mutex);

	Available -= Request;
	Hold[thr] += Request;
	Need[thr] -= Request;

	pthread_mutex_unlock(&Mutex);
	
	if(0 == Need[thr])
	{
		Available += Hold[thr];
	    Hold[thr] = 0;
	    Need[thr] = 0;
	    Finish[thr]=0;
	    printf("The customer %d has returned money.\n");
	}
		
	//if not safe, the request will be canceled
	if(!CheckSafty(thr))
	{
		//Critical section
		pthread_mutex_lock(&Mutex);

	    Available += Request;
		Hold[thr] -= Request;
		Need[thr] += Request;

		pthread_mutex_unlock(&Mutex);
		
		return 1;      //Fail requesting
	}
	
	return 0;          //Succeed requesting
}

void *runner(void *param)
{
	int i,result,thr = (int)param;
	while(1)
	{
		sleep(1);
		if(cusflag == thr)
		{
			result = RequestMoney((int)param);
			switch(result)
			{
				case -2:
					printf("Request for Customer %d is not available\n",thr);
					break;
				case -1:
					printf("Request for Customer %d is exceeded\n",thr);
					break;
				case 1:
					printf("Request for Customer %d is not Safe!\n",thr);
					break;
				default: break;
			}
			
			cusflag = -1;
			menu();
			printf("Please input your choice:");  
		}
	}
}

void mainthread()
{
	menu();
	printf("Please input your choice:");  

	while(1)
	{
		char choice;
		scanf("%c",&choice);
		if(choice == 'q') break;
		if(choice == 'r'){
		printf("Choose customer:");
                int cus;
		scanf("%d",&cus);
		printf("Input request:");
		scanf("%d",&Request);
		cusflag=cus;
		//flag = rand() % N;
		sleep(1);
	    }
	    else continue;
	}
	
	printf("The main thread ends.\n");
}


int main()
{	
	int i;
	init();
	//srand((unsigned)time(NULL));
	
	//pthread_t ThreadId[N];
	//pthread_attr_t attr[N];
	
	//for(i=0; i<N; ++i)
	//pthread_attr_init(&attr[i]);
	
	for(i=0; i<N; ++i)
	{
		int error;
		error=pthread_create(&ThreadId[i],0,runner,i);
		if(error)
		printf("Fail creating customer[i], error type:%d\n",error);
	}
	
	mainthread();

	for(i=0; i<N; ++i)
        if(ThreadId[i] !=0) {                  
                pthread_join(ThreadId[i],NULL);
                printf("Customer %d ends\n",i);
        }
	system("pause");
	return 0;
}

</code></pre>

# Thanks

Thanks for your reading and happy coding
