#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<error.h>
#include<sys/wait.h>

int main(int argc, char **argv)
{
	int fds[2];
	pid_t pid;

	if(pipe(fds) == -1)
		perror("pipe");

	pid=fork();
	
	if(pid == 0)
	{
		if(dup2(fds[1], STDOUT_FILENO) == -1)
			perror("dup2");
		if(close(fds[0]) == -1)
			perror("close");
		if(close(fds[1]) == -1)
			perror("close");
		execlp("grep", "grep", "-n", argv[1], argv[2], NULL);
		perror("execlp");
	}
	else
	{
		if(dup2(fds[0], STDIN_FILENO) == -1)
			perror("dup2");
		if(close(fds[0]) == -1)
			perror("close");
		if(close(fds[1]) == -1)
			perror("close");
		//if(waitpid(pid, NULL, 0) == -1)
			//perror("waitpid");
		execlp("cut", "cut", "-d:", "-f1", NULL);
		perror("execlp");
	}
	return 0;
}
		
		
		
		
	
