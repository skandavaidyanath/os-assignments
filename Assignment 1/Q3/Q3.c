#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<error.h>
#include<time.h>
#include<sys/wait.h>
#define LEN 10

int isPrime(int a)
{
	int flag=0;
	if(a<=1)
		return 0;
	for(int j=2; j<=a/2; j++)
	{
		if(a%j == 0)
		{
			flag=1;
			break;
		}
	}
	if(flag==0)
		return 1;
	return 0;
}

int allVis(int vis[LEN])
{
	for(int i=0;i<LEN;i++)
	{
		if(vis[i]==0)
			return 0;
	}
	return 1;
}

int main(int argc, char **argv)
{
	int fd1[2];
	pid_t pid;
	int nbytes;
	int a[]={11,12,13,14,15,16,17,18,19,20};
	if(pipe(fd1) == -1)
		perror("pipe");

	pid=fork();
	
	if(pid == 0)
	{
		int sum=0;
		int n;
		if(close(fd1[1]) == -1)
			perror("close");
		while((nbytes = read(fd1[0], &n, sizeof(n))) > 0)
		{	
			if(isPrime(n))
			{
				printf("%d is prime\n", n);
				sum=sum+n;
			}
			else
				printf("%d is not prime\n", n);
			printf("Sum of all primes is %d\n", sum);
			int w=n%3;
			printf("Child sleeping for %d seconds\n", w);
			sleep(w);
		}
		if(nbytes != 0)
			perror("read");
		if(close(fd1[0]) == -1)
			perror("close");
	}
	else
	{
		int x, y, posn;
		int vis[LEN];
		memset(vis, 0, LEN*sizeof(vis[0]));	
		if(close(fd1[0]) == -1)
			perror("close");
		while(allVis(vis)!=1)
		{	
			srand(time(NULL));
			x=rand()%LEN;
			vis[x]=1;
			x=a[x];
			printf("%d\n", x);
			do
			{
				posn=rand()%LEN;
				y=a[posn];
			}while(x==y);
			vis[posn]=1;
			printf("%d\n", y);
			write(fd1[1], &y, sizeof(y));
			int s=x/3;
			printf("Parent sleeping for %d seconds\n", s);
			sleep(s);
			//wait(NULL);
		} 
		if(close(fd1[1]) == -1)
			perror("close");
		exit(0);
	}
	return 0;
}
