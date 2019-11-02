#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PROCESSES 20

int arrival[NUM_PROCESSES] = {5, 27, 1, 51, 22, 1, 28, 11, 16, 10, 14, 5, 10, 9, 6, 4, 23, 11, 5, 1};
int burst[NUM_PROCESSES] = {10, 17, 2, 22, 15, 33, 3, 7, 3, 2, 1, 4, 6, 0, 35, 6, 15, 26, 1, 17};

typedef struct process
{
    int pid;
    char state;
    int arrival_time;
    int cpu_burst;
    int wait_time;
    char scheduling_policy[100];
    int time_quantum;
    int preemption; // 1 true 0 false
} process;

process table[NUM_PROCESSES];

typedef struct event
{
    enum eventtype
    {
        Arrival,
        CPUburstCompletion,
        TimerExpired
    } name;
    double time;
} event;

event eve[NUM_PROCESSES];

int event_n = 0;

process p;
int cpu = -1; //-1 is for idle

char scheduling_policy[100];

int queue[100];
int front = 0;
int back = 0;
void enqueue(int a)
{
    queue[back] = a;
    back++;
}

int dequeue()
{
    if (front == back)
        return -1;
    int n = queue[front];
    front++;
    return n;
}

int heap[1000000], heapSize;

void Init()
{
    heapSize = 0;
    heap[0] = -INT_MAX;
}

void heap_push(int element)
{
    heapSize++;
    heap[heapSize] = element;
    int now = heapSize;

    while (heap[now / 2] > element)
    {
        heap[now] = heap[now / 2];
        now /= 2;
    }

    heap[now] = element;
}

int heap_pop()
{
    int minElement, lastElement, child, now;
    minElement = heap[1];
    lastElement = heap[heapSize--];
    for (now = 1; now * 2 <= heapSize; now = child)
    {
        child = now * 2;
        if (child != heapSize && heap[child + 1] < heap[child])
            child++;
        if (lastElement > heap[child])
            heap[now] = heap[child];
        else
            break;
    }
    heap[now] = lastElement;
    return minElement;
}

int test(int pid, int n)
{
    for (int i = 0; i <= n; i++)
    {
        if (pid == table[i].pid && table[i].state == 'N')
            return i;
    }
    return -1;
}

int table_n = 0;
int is_complete()
{
    int i;
    int flag = 0;
    for (i = 0; i < table_n; i++)
    {
        if (table[i].state != 'T')
            return 0;
    }
    return 1;
}

void simulate_FCFS()
{
    int currentPid;
    int count = 0;
    int flag = 0;
    int atLeastOneProcess = 0;
    int time;
    int burstLeft = 0;
    for (time = 0; !((atLeastOneProcess == 1) && (dequeue() == -1) && (is_complete())); time++)
    {
        flag = 0;
        if (burstLeft == 0 && time != 0 && atLeastOneProcess == 1)
        {
            cpu = -1;

            eve[event_n].name = CPUburstCompletion;
            int lel = dequeue();
            printf("PID %d COMPLETES at t = %d\n", table[currentPid].pid, time);
            event_n++;
            table[currentPid].state = 'T';
        }
        for (int k = 0; k < NUM_PROCESSES; k++)
        {
            if (arrival[k] == time)
            {
                eve[event_n].name = Arrival;
                enqueue(1);
                event_n++;
                table[table_n].pid = table_n + 1;
                printf("PID %d INSERTED at t = %d \n", table[table_n].pid, time);
                heap_push(table[table_n].pid);
                table[table_n].state = 'N'; //N is for notarrived
                table[table_n].arrival_time = arrival[k];
                table[table_n].cpu_burst = burst[k];
                strcpy(table[table_n].scheduling_policy, "FCFS");
                table[table_n].preemption = 0;
                table_n++;
            }
        }
        if (heapSize == 0)
            flag = 1;
        if ((cpu == -1) && (flag == 0))
        {
            int temp = heap_pop();
            // we have the minimum PID with us now
            int s = test(temp, table_n);
            if (s == -1)
                flag = 1;
            else
            {
                cpu = s + 1;
                count++;
                burstLeft = table[s].cpu_burst;
                table[s].wait_time = time - table[s].arrival_time;
                printf("PID %d SCHEDULED at t = %d ", s + 1, time);
                printf("and will COMPLETE at t = %d\n", table[s].cpu_burst + time);

                currentPid = s;
                atLeastOneProcess = 1;
            }
        }
        if (burstLeft > 0)
            burstLeft--;
        else
            burstLeft = 0;
    }
}

float get_awt()
{
    float awt = 0;
    for (int i = 0; i < NUM_PROCESSES; i++)
        awt = awt + table[i].wait_time;
    awt = awt / NUM_PROCESSES;
    return awt;
}

void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void sort(int arr[], int arr2[], int n)
{
    int i, j;
    for (i = 0; i < n - 1; i++)
    {
        for (j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(&arr[j], &arr[j + 1]);
                swap(&arr2[j], &arr2[j + 1]);
            }
        }
    }
}

int main()
{
    p.pid = -1;
    cpu = -1;
    Init();
    sort(arrival, burst, NUM_PROCESSES);
    printf("FCFS\n");
    
    printf("arrivals: ");
    for (int g = 0; g < NUM_PROCESSES; g++)
        printf("%d ", arrival[g]);
    printf("\n");

    printf("bursts:   ");
    for (int g = 0; g < NUM_PROCESSES; g++)
        printf("%d ", burst[g]);
    printf("\n");

    simulate_FCFS();
    printf("AWT = %.2f s\n", get_awt());
    return 0;
}
