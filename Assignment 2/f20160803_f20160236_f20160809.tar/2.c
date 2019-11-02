#include<pthread.h>
#include<stdio.h>
#include<unistd.h>
#include<semaphore.h>
#include<stdlib.h>
#include<time.h>

int a, b, c, d;
int k;
sem_t lock;
int running[8];
int number_of_runs[8];

int exit_condition(int pid)
{
   int any_process_running = 0;
    for(int i = 0; i < 8; i++)
    {
        if(running[i] == 1)
        {
            any_process_running = 1;
            return 0;
        }
    }
    switch(pid)
    {
        case 1:
                if((a > 0) && (number_of_runs[4] < k))
                {
                    return 0;
                }
                else if((b > 0) && (number_of_runs[5] < k))
                {
                    return 0;
                }
                else if((c > 0) && (number_of_runs[6] < k))
                {
                    return 0;
                }
                break;
        case 2:
                if((b > 0) && (number_of_runs[5] < k))
                {
                    return 0;
                }
                else if((c > 0) && (number_of_runs[6] < k))
                {
                    return 0;
                }
                else if((d > 0) && (number_of_runs[7] < k))
                {
                    return 0;
                }
                break;
        case 3:
                if((a > 0) && (number_of_runs[4] < k))
                {
                    return 0;
                }
                else if((c > 0) && (number_of_runs[6] < k))
                {
                    return 0;
                }
                else if((d > 0) && (number_of_runs[7] < k))
                {
                    return 0;
                }
                break;
        case 4:
                if((a > 0) && (number_of_runs[4] < k))
                {
                    return 0;
                }
                else if((b > 0) && (number_of_runs[5] < k))
                {
                    return 0;
                }
                else if((d > 0) && (number_of_runs[7] < k))
                {
                    return 0;
                }
                break;
        case 5:
                return 1;
                break;
        case 6:
                return 1;
                break;
        case 7:
                return 1;
                break;
        case 8:
                return 1;
                break;
      }
      return 1;
                       
}    


void release_a(int pid)
{
    double n = (double)rand() / (double)RAND_MAX;
    if (n < 0.66)
    {
        a++;
        printf("Process %d released A\n", pid);
    }
    else
    {
        printf("Process %d did not release A\n", pid);
    }
}

void release_b(int pid)
{
    double n = (double)rand() / (double)RAND_MAX;
    if (n < 0.75)
    {
        b++;
        printf("Process %d released B\n", pid);
    }
    else
    {
        printf("Process %d did not release B\n", pid);
    }
}

void release_c(int pid)
{
    double n = (double)rand() / (double)RAND_MAX;
    if (n < 0.60)
    {
        c++;
        printf("Process %d released C\n", pid);
    }
    else
    {
        printf("Process %d did not release C\n", pid);
    }
}

void release_d(int pid)
{
    double n = (double)rand() / (double)RAND_MAX;
    if (n < 0.66)
    {
        d++;
        printf("Process %d released D\n", pid);
    }
    else
    {
        printf("Process %d did not release D\n", pid);
    }
}

void * thread_function(void *arg)
{
    int pid = *((int *)arg);
    for(int i = 1; i<= k; i++)
    {
        printf("Process %d starting %dth execution \n", pid, i);
        int a_val, b_val, c_val, d_val;
        switch(pid)
        {
            case 1:
                printf("%d : Initial. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                while(1)
                {
                    sem_wait(&lock);
                    if ((a > 0) && (b > 0) && (c > 0))
                    {
                        a--;
                        b--;
                        c--;
                        running[pid-1] = 1;
                        sem_post(&lock);
                        break;
                    }
                    else
                    {
                        sem_post(&lock);
                        if(exit_condition(pid))
                        {
                            printf("INSUFFICIENT RESOURCES !\n");
                            exit(0);
                        }
                        sleep(3);
                        continue;
                    } 
                }
                printf("%d : After collecting. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                sleep(1);
                release_c(pid);
                release_b(pid);
                release_a(pid);
                running[pid-1] = 0;
                printf("%d : After completion, releasing. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                break;
            case 2: 
                printf("%d : Initial. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                while(1)
                {
                    sem_wait(&lock);
                    if ((b > 0) && (c > 0) && (d > 0))
                    {
                        b--;
                        c--;
                        d--;
                        running[pid-1] = 1;
                        sem_post(&lock);
                        break;
                    }
                    else
                    {
                        sem_post(&lock);
                        if(exit_condition(pid))
                        {
                            printf("INSUFFICIENT RESOURCES !\n");
                            exit(0);
                        }
                        sleep(3);
                        continue;
                    } 
                }
                printf("%d : After collecting. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                sleep(1);
                release_d(pid);
                release_c(pid);
                release_b(pid);
                running[pid-1] = 0;
                printf("%d : After completion, releasing. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                break;
            case 3: 
                printf("%d : Initial. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                while(1)
                {
                    sem_wait(&lock);
                    if ((a > 0) && (c > 0) && (d > 0))
                    {
                        a--;
                        c--;
                        d--;
                        running[pid-1] = 1;
                        sem_post(&lock);
                        break;
                    }
                    else
                    {
                        sem_post(&lock);
                        if(exit_condition(pid))
                        {
                            printf("INSUFFICIENT RESOURCES !\n");
                            exit(0);
                        }
                        sleep(3);
                        continue;
                    } 
                }
                printf("%d : After collecting. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                sleep(1);
                release_d(pid);
                release_c(pid);
                release_a(pid);
                running[pid-1] = 0;
                printf("%d : After completion, releasing. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                break;
            case 4:
                printf("%d : Initial. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d); 
                while(1)
                {
                    sem_wait(&lock);
                    if ((a > 0) && (b > 0) && (d > 0))
                    {
                        a--;
                        b--;
                        d--;
                        running[pid-1] = 1;
                        sem_post(&lock);
                        break;
                    }
                    else
                    {
                        sem_post(&lock);
                        if(exit_condition(pid))
                        {
                            printf("INSUFFICIENT RESOURCES !\n");
                            exit(0);
                        }     
                        sleep(3);
                        continue;
                    } 
                }
                printf("%d : After collecting. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                sleep(1);
                release_d(pid);
                release_b(pid);
                release_a(pid);
                running[pid-1] = 0;
                printf("%d : After completion, releasing. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                break;
            case 5:
                printf("%d : Initial. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d); 
                while(1)
                {
                    sem_wait(&lock);
                    if (a > 0)
                    {
                        a--;
                        running[pid-1] = 1;
                        sem_post(&lock);
                        break;
                    }
                    else
                    {
                        sem_post(&lock);
                        if(exit_condition(pid))
                        {
                            printf("INSUFFICIENT RESOURCES !\n");
                            exit(0);
                        }
                        sleep(3);
                        continue;
                    } 
                }
                printf("%d : After collecting. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);  
                sleep(1);
                release_a(pid);
                running[pid-1] = 0;
                printf("%d : After completion, releasing. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                break;
            case 6: 
                printf("%d : Initial. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                while(1)
                {
                    sem_wait(&lock);
                    if (b > 0)
                    {
                        b--;
                        running[pid-1] = 1;
                        sem_post(&lock);
                        break;
                    }
                    else
                    {
                        sem_post(&lock);
                        if(exit_condition(pid))
                        {
                            printf("INSUFFICIENT RESOURCES !\n");
                            exit(0);
                        }
                        sleep(3);
                        continue;
                    } 
                }
                printf("%d : After collecting. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                sleep(1);
                release_b(pid);
                running[pid-1] = 0;
                printf("%d : After completion, releasing. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                break;
            case 7: 
                printf("%d : Initial. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                while(1)
                {
                    sem_wait(&lock);
                    if (c > 0)
                    {
                        c--;
                        running[pid-1] = 1;
                        sem_post(&lock);
                        break;
                    }
                    else
                    {
                        sem_post(&lock);
                        if(exit_condition(pid))
                        {
                            printf("INSUFFICIENT RESOURCES !\n");
                            exit(0);
                        }
                        sleep(3);
                        continue;
                    } 
                }
                printf("%d : After collecting. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                sleep(1);
                release_c(pid);
                running[pid-1] = 0;
                printf("%d : After completion, releasing. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                break;
            case 8: 
                printf("%d : Initial. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                while(1)
                {
                    sem_wait(&lock);
                    if (d > 0)
                    {
                        d--;
                        running[pid-1] = 1;
                        sem_post(&lock);
                        break;
                    }
                    else
                    {
                        sem_post(&lock);
                        if(exit_condition(pid))
                        {
                            printf("INSUFFICIENT RESOURCES !\n");
                            exit(0);
                        }
                        sleep(3);
                        continue;
                    } 
                }
                printf("%d : After collecting. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                sleep(1);
                release_d(pid);
                running[pid-1] = 0;
                printf("%d : After completion, releasing. Number of A: %d Number of B: %d Number of C: %d Number of D: %d\n", pid, a, b, c, d);
                break;
        }
        printf("Process %d finished %dth execution \n", pid, i);
        number_of_runs[pid-1]++;
    }
}

int main()
{
    printf("Enter the value of k: ");
    scanf("%d", &k);
    printf("Enter the value of initial A, B, C, D: ");
    scanf("%d %d %d %d", &a, &b, &c, &d);
    int i;
    pthread_t tid[8];
    sem_init(&lock, 0, 1);
    srand(time(NULL));
    int p1 = 1, p2 = 2, p3 = 3, p4 = 4, p5 = 5, p6 = 6, p7 = 7, p8 = 8;
    pthread_create(&tid[0], NULL, thread_function, &p1);
    pthread_create(&tid[1], NULL, thread_function, &p2);
    pthread_create(&tid[2], NULL, thread_function, &p3);
    pthread_create(&tid[3], NULL, thread_function, &p4);
    pthread_create(&tid[4], NULL, thread_function, &p5);
    pthread_create(&tid[5], NULL, thread_function, &p6);
    pthread_create(&tid[6], NULL, thread_function, &p7);
    pthread_create(&tid[7], NULL, thread_function, &p8);
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);
    pthread_join(tid[4], NULL);
    pthread_join(tid[5], NULL);
    pthread_join(tid[6], NULL);
    pthread_join(tid[7], NULL);
    sem_destroy(&lock);
    printf("ALL PROCESSES HAVE FINISHED %d ITERATIONS !\n", k);
    return 0;
}
