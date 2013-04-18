# DataStructure Demos and Examples

A collection of OS (LINUX) experiments I've created, now open source and on GitHub, so please go ahead and help me hack this resource in to a wealth of demos that other authors can learn from.

# Aim

* If a user can hit view source on the demo, then we've done our job
* All content is open source and just enjoy it
* Individual demos, if authored by someone other than [@westhacker](http://twitter.com) can be credited as appropriate

# Language

* C in Linux

# Banker's Algorithm

* ʵ�ֵ�����Դ�����м��㷨��4���ͻ���
* main-thread ���� 4 �� work-threads
* ���м� ��main-thread�� ���ܹ˿͵Ĵ���ƻ����жϹ˿͵ķ��ڴ���İ�ȫ�ԣ���������Ӧ��
* �˿� ��work-threads�������������Ŀ�꣬�����������м�������������
* �鿴�������������� ʹ���źŻ��ƣ���ʾ��ǰ���мҵ�״�����˿͵�״�����ʽ��״��...
* �����һ���˵������������ܶ�ͻ�����Ϣ���ͻ���Ŀ��...
�����ó��������в˵���
������㷨��Ч�Ļ������ʧ�ܣ��ɹ������Ժ󣬿���������һ�ص�����...

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
