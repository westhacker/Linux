# OS-Linux Demos and Examples

A collection of OS (LINUX) experiments I've created, now open source and on GitHub, so please go ahead and help me hack this resource in to a wealth of demos that other authors can learn from.

# Aim

* If a user can hit view source on the demo, then we've done our job
* All content is open source and just enjoy it
* Individual demos, if authored by someone other than [@westhacker](http://twitter.com) can be credited as appropriate

# Language

* C in Linux

# ShellCommandExplainer

* 实现简单的 SHELL- 命令解释器
* 使用 fork-exec-wait-exit 模型，
* 命令行包含管道、I/O 重定向
* 实现常用的内部命令
* 处理外部命令

# Content

<pre><code>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>

char cmd[40]="";		//the command to input
char *arg[10]={NULL};	//the parted command by " "
char addr[40]="";		//work directory
int cn=0;			    //number of parts of cmd
pid_t PID;			    //child's pid
int f=0;

//add " " before or after < >> >
char *addBlank(char cmd[40])
{
	char buf[40];
	char temp[40];
	char temp1[40];
	char *p;
	int t=0;
	int j=0,i=0;

	strcpy(buf,cmd);

	if((p=strstr(buf,">>")) != NULL) 	
	{
		t=(p-buf)/sizeof(char);		
		
		for(i=0;i<t;i++)
		{
			strcpy((temp+i),(buf+i));
		}
		strcpy((temp+t)," ");
		strcpy((temp+t+1),(buf+t));
		strcpy((temp+t+2),(buf+t+1));
		strcpy((temp+t+3)," ");
		for(i=t+4,j=t+2;j<sizeof(buf);i++,j++)
		{
			strcpy((temp+i),(buf+j));
		}

		if((p=strstr(temp,"<")) != NULL)
		{
			t=(p-temp)/sizeof(char);
			
			for(i=0;i<t;i++)
			{
				strcpy((temp1+i),(temp+i));
			}
			strcpy((temp1+t)," ");
			strcpy((temp1+t+1),(temp+t));
			strcpy((temp1+t+2)," ");
			for(i=t+3,j=t+1;j<sizeof(temp);i++,j++)
			{
				strcpy((temp1+i),(temp+j));
			}
			strcpy(cmd,temp1);
		}
		else
		{
			strcpy(cmd,temp);
		}
	}
	else if((p = strstr(buf,">")) != NULL)
	{
		t=(p-buf)/sizeof(char);
		for(i=0;i<t;i++)
		{
			strcpy((temp+i),(buf+i));
		}
		strcpy((temp+t)," ");
		strcpy((temp+t+1),(buf+t));
		strcpy((temp+t+2)," ");

		for(i=t+3,j=t+1;j<sizeof(buf);i++,j++)
		{
			strcpy((temp+i),(buf+j));
		}
		
		if((p=strstr(temp,"<")) != NULL)
		{
			t=(p-temp)/sizeof(char);
			for(i=0;i<t;i++)
			{
				strcpy((temp1+i),(temp+i));
			}
			strcpy((temp1+t)," ");
			strcpy((temp1+t+1),(temp+t));
			strcpy((temp1+t+2)," ");
			for(i=t+3,j=t+1;j<sizeof(temp);i++,j++)
			{
				strcpy((temp1+i),(temp+j));
			}
			strcpy(cmd,temp1);
		}
		else
		{
			strcpy(cmd,temp);
		}
	}	
	else
	{
		;
	}
	return cmd;
}


//dispart the command with " "
int my_strstr(char cmd[40], char *arg[10])
{
	char *token;
	int i=0;
	token=strtok(cmd," ");
	while(token != NULL)
	{
		arg[i++]=token;
		token=strtok(NULL," ");
	}
	arg[i]=NULL;
	return i;
}


//check and classify the command

int check()
{
	int i=0;
	int flag=0;
	
	while(i<cn)
	{
		if(strcmp(arg[i],">") == 0)
		{
			if(strcmp(arg[cn-2],"<") == 0)
			{
					flag=2;
					break;
			}
			else
			{
				flag=1;
				f=i;
				break;
			}
		}
		if(strcmp(arg[i],">>") == 0)
		{
			if(strcmp(arg[cn-2],"<") == 0)
			{
				flag=4;
				break;
			}
			else
			{
				flag=3;
				f=i;
				break;
			}
		}
		if(strcmp(arg[i],"|") == 0)
		{
			flag=5;
			f=i;
			break;
		}
		
		i++;
	}
return flag;
}


//command with redirection
//execute command like "cat>a"
int redirect()
{
	 int fd;
	 if((fd=open(arg[f+1],O_RDWR|O_CREAT|O_TRUNC,S_IRUSR)) < 0)
	 {
		 perror("open error!\n");
		 exit (1);
	 }
		arg[f]=NULL;
		arg[f+1]=NULL;
		close(1);
		dup2(fd,1);
		execvp(arg[0],arg);
	 return 0;
}

//execute command like "cat>a<b"
int redirect_a()
{
	int fd;
	if((fd=open(arg[f+1],O_RDWR|O_CREAT|O_APPEND,S_IRUSR)) < 0)
	{
		perror("open error!\n");
		exit (1);
	}
		arg[f]=NULL;
		arg[f+1]=NULL;
		close (1);
		dup2(fd,1);
		execvp(arg[0],arg);
		return 0;
}

//execute command like "cat>>a"
int redirect_cat()
{
	int i=0;
	char buf[1000];
	int f1;
	int f2;
	
	if((f1=open(arg[cn-1],O_RDONLY,S_IRUSR)) == -1)
	{
		perror("open error!\n");
		exit (1);
	}
	if((f2=open(arg[cn-3],O_RDWR|O_CREAT|O_TRUNC,S_IRUSR)) == -1)
	{
		perror("open error\n");
		exit (1);
	}
	i=read(f1,buf,1000);
	buf[i]='\n';
	write(f2,buf,i);
	execlp(NULL,NULL);
	
	return 0;
}

//execute command like "cat>>a<b"
int redirect_cat_a()
{
	int i=0;
	char buf[1000];
	int fd1;
	int fd2;
	
	if((fd1=open(arg[cn-1],O_RDONLY,S_IRUSR)) == -1)
	{
		perror("open error!\n");
		exit (1);
	}
	if((fd2=open(arg[cn-3],O_RDWR|O_CREAT|O_APPEND,S_IRUSR)) == -1)
	{
		perror("open error!\n");
		exit (1);
	}
	i=read(fd1,buf,1000);
	buf[i]='\n';
	write(fd2,buf,i);
	execlp(NULL,NULL);

	return 0;
}




//execute command with pipe
void my_pipe()
{
	pid_t pid;
	int fd[2];

	if(pipe(fd) < 0)
	{
		perror("pipe error!\n");
		exit (1);
	}
	if((pid=fork()) == -1)
	{
		perror("fork error!\n");
		exit (1);
	}
	if(pid == 0)
	{
		close(fd[0]);
		dup2(fd[1],1);
		arg[f]=NULL;
		arg[f+1]=NULL;
		execvp(arg[0],arg);
	}
	else
	{
		close(fd[1]);
		dup2(fd[0],0);
		execvp(arg[f+1],arg+f+1);
	}
	
}


//execute the inner command like exit or cd
int isincmd(char *arg[10])
{
	int i=0;
	char hist[40];
	
	if(strncmp(arg[0],"exit",4) == 0)
	{
		printf("******************************SHELL COMMAND EXPLAINER END******************************\n");
		exit (0);
	}

	if(strncmp(arg[0],"cd",2) == 0)
	{
		strcpy(hist,addr);
		getcwd(addr,40);
		if( (cn == 1) || ( strcmp(arg[1],"~") == 0 ) )
		{
			chdir("/root");
			strcpy(hist,"/root");
		}
		else if( strcmp(arg[1],"-") == 0 )
		{
			strcpy(arg[1],hist);
			chdir(arg[1]);
		}
		else
		{
			chdir(arg[1]);
		}
		i=1;
	}
	return i;

}





int main()
{
	int i=0;
	int j=0;
	int ch=0;
	int flag=0;
	pid_t pid;
	
	getcwd(addr,40);			//get current directory
	
	printf("******************************SHELL COMMAND EXPLAINER START******************************\n");
	
	while(1)
	{
		//char path[300];
        //getcwd(path,300);
        //printf("Cur_dir:%s\n",path);
        
		j=0;
		printf("Your command [%s] $ ",get_current_dir_name());
	
		memset(cmd,0,40);
		
		ch=getchar();
		while(j<BUFSIZ&&ch!='\n')
		{
		    cmd[j++]=ch;
			ch=getchar();
		}
		if(j==BUFSIZ)
	    {
		    printf("Your command is too long\n");
		    break;
	    }
		
		strcpy(cmd,addBlank(cmd));	//add " " before or after “<” “>>” “>”
		
		cn=my_strstr(cmd,arg);		//dispart the command with " "
		
		if(arg[0] != NULL)
		{
			if(isincmd(arg) == 1)			//execute the inner command like exit or cd
			{
				continue;
			}
			else
			{	
				if((pid=fork()) < 0)
				{
					perror("fork error!\n");
					exit (1);
				}
				if(pid == 0)
				{
					i=check();
					switch(i)
					{
						case 1:
							redirect();//execute command like "cat>a"
							break;
						case 2:		   //execute command like "cat>>a"
							redirect_cat();
							break;
						case 3:	       //execute command like "cat>a<b"
							redirect_a();
							break;
						case 4:	       //execute command like "cat>>a<b"
							redirect_cat_a();
							break;
						case 5:	       //execute command with pipe
							my_pipe();
							break;
						default:
							execvp(arg[0],arg);//execute ordinary command
							break;
					}
				}
				else
				{
					PID=pid;
					if(flag == 1)	//no wait for child
					{
						;
					}
					else	        //wait for child
					{
						waitpid(pid,NULL,WUNTRACED);
					}
				}
			}
		}
	}
}

</code></pre>

# Thanks

Thanks for your reading and happy coding
