#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

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

#define NUM_PROCESSES 20

int arrival[NUM_PROCESSES] = {5, 27, 1, 51, 22, 1, 28, 11, 16, 10, 14, 5, 10, 9, 6, 4, 23, 11, 5, 1};
int burst[NUM_PROCESSES] = {10, 17, 2, 22, 15, 33, 3, 7, 3, 2, 1, 4, 6, 0, 35, 6, 15, 26, 1, 17};

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

int bottomQueue[100];
int bottomFront = 0;
int bottomBack = 0;
void bottomEnqueue(int a)
{
    bottomQueue[bottomBack] = a;
    bottomBack++;
}
int bottomDequeue()
{
    if (bottomFront == bottomBack)
        return -1;
    int n = bottomQueue[bottomFront];
    bottomFront++;
    return n;
}

int topQueue[100];
int topFront = 0;
int topBack = 0;
void topEnqueue(int a)
{
    topQueue[topBack] = a;
    topBack++;
}
int topDequeue()
{
    if (topFront == topBack)
        return -1;
    int n = topQueue[topFront];
    topFront++;
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

int table_n = 0;

int is_complete()
{
    int i;
    int flag = 0;
    for (i = 0; i < table_n; i++)
    {
        if (table[i].state != 'T')
            return 0;
        flag = 1;
    }
    if (flag == 0)
        return 0;
    return 1;
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

float get_awt()
{
    float s = 0;
    for (int i = 0; i < NUM_PROCESSES; i++)
        s = s + table[i].wait_time;
    s = s / NUM_PROCESSES;
    return s;
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
        for (j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1])
            {
                swap(&arr[j], &arr[j + 1]);
                swap(&arr2[j], &arr2[j + 1]);
            }
}

void procWait()
{
    for (int i = 0; i < table_n; i++)
    {
        if (table[i].state == 'N' && cpu != table[i].pid)
            table[i].wait_time++;
    }
}

void multilevel()
{
    int time = 0;
    int currentPid = 0;
    int FCFSburst = -1;
    int shouldNotPreempt = -1;
    int RRburst = -1;
    int prevRRburst = 0;
    int prevFCFSburst = 0;
    for (time = 0; !is_complete(); time++)
    {
        if ((FCFSburst == 0) && (prevFCFSburst > 0)) //this is for process complete
        {
            for (int j = 0; j < table_n; j++)
            {
                if (cpu == table[j].pid)
                {
                    table[j].state = 'T';
                    FCFSburst = -1;
                    printf("PID %d COMPLETED at t = %d\n", table[j].pid, time);
                    cpu = -1;
                }
            }
        }
        if ((RRburst == 0) && (prevRRburst > 0)) //time quantum expires
        {
            for (int j = 0; j < table_n; j++)
            {
                if (cpu == table[j].pid && (strcmp(table[j].scheduling_policy, "RR") == 0))
                {
                    if (table[j].state != 'T')
                        table[j].state = 'N';
                    RRburst = -1;
                    cpu = -1;
                    table[j].cpu_burst = FCFSburst;
                    topEnqueue(table[j].pid);
                }
            }
        }
        for (currentPid = 0; currentPid < NUM_PROCESSES; currentPid++) // this works
        {
            if (arrival[currentPid] == time)
            {
                printf("PID %d ARRIVES at t = %d\n", table_n + 1, time);
                eve[event_n].name = Arrival; // arrival event created
                bottomEnqueue(1);                  // cpu burst completion event added to bottomQueue
                event_n++;
                table[table_n].pid = table_n + 1;
                heap_push(table[table_n].pid);
                table[table_n].state = 'N'; // N is for notscheduled
                // for now  0 time quanta is not even scheduled
                table[table_n].arrival_time = arrival[currentPid];
                table[table_n].cpu_burst = burst[currentPid];
                if (burst[currentPid] > 8)
                    strcpy(table[table_n].scheduling_policy, "FCFS");
                else
                    strcpy(table[table_n].scheduling_policy, "RR");
                table[table_n].preemption = 0;
                if (burst[currentPid] <= 8)
                    topEnqueue(table[table_n].pid);
                table[table_n].wait_time = 0;
                table_n++;
            }
        }
        for (int r = 0; r < table_n; r++)
        {

            if (cpu == table[r].pid)
            {
                if ((strcmp(table[r].scheduling_policy, "FCFS") == 0) && (topFront != topBack)) // check if FCFS is running and preempt if there is process in MLQ
                {
                    shouldNotPreempt = 0;
                }
                else
                {
                    shouldNotPreempt = 1; // RR running or process may not be running
                }
            }
        }
        if ((shouldNotPreempt == 0) && (topFront != topBack)) // preempt if FCFS is running in CPU
        {
            for (int q = 0; q < table_n; q++)
            {
                if (cpu == table[q].pid)
                {
                    printf("PID %d PREEMPTED at t = %d\n", cpu, time);
                    table[q].cpu_burst = FCFSburst;
                    table[q].state = 'N';
                    shouldNotPreempt = 1;
                    cpu = -1;
                }
            }
        }
        // if a RR preocess comes,then preeemt the FCFS process, otherwise just schedule RR
        if ((topFront != topBack) && (cpu == -1))
        {
            int flag = 0;
            while (flag == 0)
            {
                int lel = topDequeue();
                for (int j = 0; j < table_n; j++)
                {
                    if ((table[j].pid == lel) && (strcmp(table[j].scheduling_policy, "RR") == 0) && (table[j].state == 'N'))
                    {
                        printf("PID %d SCHEDULED at t = %d in RR\n", table[j].pid, time);

                        FCFSburst = table[j].cpu_burst;
                        cpu = table[j].pid;
                        RRburst = 4;
                        if (FCFSburst == 0)
                        {
                            flag = 0;
                            table[j].state = 'T';
                            FCFSburst = -1;
                            printf("PID %d COMPLETED at t = %d\n", table[j].pid, time);
                            cpu = -1;
                        }
                        else
                            flag = 1;
                    }
                }
            }
        }
        if ((topFront == topBack) && (cpu == -1)) //this is when top bottomQueue has no entries
        {
            for (int j = 0; j < table_n; j++)
            {
                if (table[j].state == 'N' && strcmp(table[j].scheduling_policy, "FCFS") == 0 && ((FCFSburst == 0) || (FCFSburst == -1)))
                {
                    RRburst = INT_MAX;
                    printf("PID %d SCHEDULED at t = %d in FCFS\n", table[j].pid, time);
                    FCFSburst = table[j].cpu_burst;
                    cpu = table[j].pid;
                }
            }
        }
        shouldNotPreempt = -1;
        prevRRburst = RRburst;
        if (RRburst > 0)
            RRburst--;
        prevFCFSburst = FCFSburst;
        if (FCFSburst > 0)
            FCFSburst--;
        procWait();
    }
}

int main()
{
    Init();

    p.pid = -1;
    sort(arrival, burst, NUM_PROCESSES);
    printf("MultiLevel\n");

    printf("arrivals: ");
    for (int g = 0; g < NUM_PROCESSES; g++)
    {
        printf("%d ", arrival[g]);
    }
    printf("\n");

    printf("bursts:   ");
    for (int g = 0; g < NUM_PROCESSES; g++)
    {
        printf("%d ", burst[g]);
    }
    printf("\n");

    multilevel();
    printf("AWT = %.2f s\n", get_awt());
    return 0;
}
