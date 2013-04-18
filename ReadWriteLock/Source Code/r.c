#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>

#define SHM_SIZE    1024           //size of shared memory

union semun{
int                val;
struct semid_ds    *buf;
unsigned short     *array;
};

//create semaphroe
int createsem(const char *pathname,int proj_id,int members,int init_val)
{
key_t msgkey;
int index,sid;
union semun semopts;

if ((msgkey = ftok(pathname,proj_id)) == -1)
    {
      perror("ftok error!\n");
      return -1;
    }

if ((sid = semget(msgkey,members,IPC_CREAT|0666)) == -1)
    {
      perror("semget call failed.\n");
      return -1;
    }

//init
semopts.val = init_val;
for (index = 0;index < members;index++)
    {
      semctl(sid,index,SETVAL,semopts);
    }
return (sid);
}


//open semaphroe
int opensem(const char *pathname,int proj_id)
{
key_t msgkey;
int sid;

if ((msgkey = ftok(pathname,proj_id)) == -1)
    {
      perror("fail ftok!\n");
      return -1;
    }

if ((sid = semget(msgkey,0,IPC_CREAT|0666)) == -1)
    {
      perror("fail semget call.\n");
      return -1;
    }

return (sid);
}

//P operation
int sem_p(int semid,int index)
{
struct sembuf buf ={0,-1,IPC_NOWAIT};

if (index < 0)
    {
      perror("wrong index value!");
      return -1;
    }

buf.sem_num = index;
if (semop(semid,&buf,1) == -1)
    {
      perror("wrong operation!");
      return -1;
    }
return 0;
}

//V operation
int sem_v(int semid,int index)
{
struct sembuf buf = {0,+1,IPC_NOWAIT};

if (index < 0)
    {
      perror("wrong index value!\n");
      return -1;
    }

buf.sem_num = index;
if (semop(semid,&buf,1) == -1)
    {
      perror("wrong operation!\n");
      return -1;
    }

return 0;
}

//delete semaphroe
int sem_delete(int semid)
{
return(semctl(semid,0,IPC_RMID));
}

//wait semaphroe
int wait_sem(int semid,int index)
{
while(semctl(semid,index,GETVAL,0) == 0)
    {
      printf("Waiting to get into the shared memory...\n");
      sleep(1);
    }

return 1;
}

//create shared memory
int createshm(char *pathname,int proj_id,size_t size)
{
key_t shmkey;
int sid;

//get shmkey
if ((shmkey = ftok(pathname,proj_id)) == -1)
    {
      perror("fail ftok!\n");
      return -1;
    }

if ((sid = shmget(shmkey,size,IPC_CREAT|0666)) == -1)
    {
      perror("fail shmget call.\n");
      return -1;
    }

return (sid);
}

int main()
{
int semid,shmid;
char *shmaddr;
char choice;

if ((shmid = createshm(".",'m',SHM_SIZE)) == -1)
    {
      exit(1);
    }

if ((shmaddr = shmat(shmid,(char *)0,0)) == (char *)-1)
    {
      perror("attach shared memory error!\n");
      exit(1);
    }

if ((semid = opensem(".",'s')) == -1)
    {
      exit(1);
    }

while (1)
    {
      //printf("I'm reader 1. Do you want to read now?(y or n):");
      //printf("I'm reader 2. Do you want to read now?(y or n):");
      //printf("I'm reader 3. Do you want to read now?(y or n):");
      printf("I'm reader 4. Do you want to read now?(y or n):");
      scanf("%c",&choice);
      getchar();
      if(choice=='y'){
      wait_sem(semid,0); //等待信号值为1
      sem_p(semid,0);     //P操作
      printf("I'm Ready now. \nInformation read:");
      printf("%s\n",shmaddr);
      //printf("Do you want to end the reading operation?(y or n):");
      //scanf("%c",&choice);
      //getchar();
      printf("Reading Operation Done.\n");
      sem_v(semid,0);     //V操作
      }else continue;
    }
}

