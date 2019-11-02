#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>

int main()
{
    int n,k,r;
	scanf("%d %d %d", &n, &k, &r);
	k++;
	int count = 1;
	int pid;
	int pfd1[2];
	int pfd2[2];
	pipe(pfd1);
	pipe(pfd2);
	pid = fork();
	if(pid == 0)
	{
        while (1) 
        {
            close(pfd1[0]);
			close(pfd2[1]);
            int fds[2];
            pipe(fds);
            if(count%r==1 && count>1)
            {
                char to_kill[20];
                write(pfd1[1],"#",2);
			    read(pfd2[0],to_kill,sizeof(to_kill));	
			    if(strcmp(to_kill,"-1") != 0)
			    {	
			        int pid3 = fork();
			        if(pid3 == 0)	
			            execlp("kill","kill","-9",to_kill,NULL);
			    }    
            }
            int pid1 = fork();
            if(pid1 == 0)
            {
                close(fds[0]);
                dup2(fds[1], 1);
                close(fds[1]);
                char *ps_args[7] = {"ps", "-eo", "pid,comm,%cpu", "--sort=-%cpu", NULL};
                execvp("ps", ps_args);
                exit(0);
            }
            else
            {
                wait(NULL);
                int pid2 = fork();
                if(pid2 == 0)
                {
                    close(fds[1]);
                    dup2(fds[0], 0);
                    close(fds[0]);
                    char k_string[10];
                    sprintf(k_string, "%d", k);
                    char *head_args[5] = {"head", "-n", k_string, NULL};
                    execvp("head", head_args);
                    exit(0);
                }
                else
                {
                    sleep(n);
                    count++;
                }
            }        
        }
    }
    else
    {
        while(1)
		{
		    char flag[5];
			close(pfd1[1]);
			close(pfd2[0]);
			read(pfd1[0],flag,2);
			if(strcmp(flag,"#")==0)
			{
				char upid[20];
				printf("Pid of process to kill:");
				scanf("%s",upid);		
				write(pfd2[1],upid,sizeof(upid));
			}
		}
    }
    return 0;
}
        
