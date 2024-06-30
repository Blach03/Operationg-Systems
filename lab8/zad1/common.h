#define QUEUE_SIZE 10
#define JOB_SIZE 10

typedef struct {
    char jobs[QUEUE_SIZE][JOB_SIZE];
    int front;
    int rear;
    int count;
} job_queue_t;
