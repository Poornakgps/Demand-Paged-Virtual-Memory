#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h> 
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/msg.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <ctype.h>
#include <sys/shm.h>

<<<<<<< HEAD

#define MAX_BUFFER_SIZE 100
#define MAX_PAGES 1000
#define MAX_PROCESS 1000
#define FROMPROCESS 10
#define TOPROCESS 20  // TOPROCESS+id will be used to read msg from sch
// #define TOMMU 10
#define FROMMMU 20

#define PAGEFAULT_HANDLED 5
#define TERMINATED 10

int k; //no. of processes
typedef struct _mmutosch {
	long    mtype;
	char mbuf[1];
} mmutosch;

// int running[MAX_PROCESS];
// int runh,runt;

typedef struct mymsgbuf {
	long    mtype;
	int id;
} mymsgbuf;


int send_message( int qid, struct mymsgbuf *qbuf )
{
	int     result, length;

	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(struct mymsgbuf) - sizeof(long);

	if ((result = msgsnd( qid, qbuf, length, 0)) == -1)
=======
#define MAX_BUFFER_SIZE 100
#define MAX_PAGES 1000
#define MAX_PROCESS 1000
#define MESSAGE_FROM_PROCESS 10
#define MESSAGE_TO_PROCESS 20
#define MESSAGE_FROM_MMU 20

#define PAGE_FAULT_RESOLVED 5
#define PROCESS_TERMINATED 10

int total_processes;

typedef struct mmu_to_scheduler_message {
	long    message_type;
	char message_buffer[1];
} mmu_to_scheduler_message;

typedef struct scheduler_process_message {
	long    message_type;
	int process_id;
} scheduler_process_message;

int send_message_to_process(int queue_id, struct scheduler_process_message *message_buffer)
{
	int send_result, message_length;

	message_length = sizeof(struct scheduler_process_message) - sizeof(long);

	if ((send_result = msgsnd(queue_id, message_buffer, message_length, 0)) == -1)
>>>>>>> 92241c1 (Updated implementations)
	{
		perror("Error in sending message");
		exit(1);
	}

<<<<<<< HEAD
	return (result);
}

int read_message( int qid, long type, struct mymsgbuf *qbuf )
{
	int     result, length;

	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(struct mymsgbuf) - sizeof(long);

	if ((result = msgrcv( qid, qbuf, length, type,  0)) == -1)
=======
	return (send_result);
}

int receive_message_from_process(int queue_id, long message_type, struct scheduler_process_message *message_buffer)
{
	int receive_result, message_length;

	message_length = sizeof(struct scheduler_process_message) - sizeof(long);

	if ((receive_result = msgrcv(queue_id, message_buffer, message_length, message_type, 0)) == -1)
>>>>>>> 92241c1 (Updated implementations)
	{
		perror("Error in receiving message");
		exit(1);
	}

<<<<<<< HEAD
	return (result);
}

int read_message_mmu( int qid, long type,mmutosch *qbuf )
{
	int result, length;

	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(mmutosch) - sizeof(long);

	if ((result = msgrcv(qid, qbuf, length, type,  0)) == -1)
=======
	return (receive_result);
}

int receive_message_from_mmu(int queue_id, long message_type, mmu_to_scheduler_message *message_buffer)
{
	int receive_result, message_length;

	message_length = sizeof(mmu_to_scheduler_message) - sizeof(long);

	if ((receive_result = msgrcv(queue_id, message_buffer, message_length, message_type, 0)) == -1)
>>>>>>> 92241c1 (Updated implementations)
	{
		perror("Error in receiving message");
		exit(1);
	}

<<<<<<< HEAD
	return (result);
}
int main(int argc , char * argv[])
{
	int mq1_key, mq2_key, master_pid;
	if (argc < 5) {
		printf("Scheduler rkey q2key k mpid\n");
		exit(EXIT_FAILURE);
	}
	mq1_key = atoi(argv[1]);
	mq2_key = atoi(argv[2]);
	k = atoi(argv[3]);
	master_pid = atoi(argv[4]);

	mymsgbuf msg_send, msg_recv;

	int mq1 = msgget(mq1_key, 0666);
	int mq2 = msgget(mq2_key, 0666);
	if (mq1 == -1)
	{
		perror("Message Queue1 creation failed");
		exit(1);
	}
	if (mq2 == -1)
	{
		perror("Message Queue2 creation failed");
		exit(1);
	}
	printf("Total No. of Process received = %d\n", k);

	//initialize the variables for running array
	int terminated_process = 0; //to keep track of running process to exit at last
	// int terminate[MAX_PROCESS];
	while (1)
	{
		read_message(mq1, FROMPROCESS, &msg_recv);
		int curr_id = msg_recv.id;

		msg_send.mtype = TOPROCESS + curr_id;
		send_message(mq1, &msg_send);

		//recv messages from mmu
		mmutosch mmu_recv;
		read_message_mmu(mq2, 0, &mmu_recv);
		//printf("received %ld\n", mmu_recv.mtype);
		if (mmu_recv.mtype == PAGEFAULT_HANDLED)
		{
			msg_send.mtype = FROMPROCESS;
			msg_send.id=curr_id;
			send_message(mq1, &msg_send);
		}
		else if (mmu_recv.mtype == TERMINATED)
		{
			terminated_process++;
			//printf("Got terminate %d\n", curr_id);
=======
	return (receive_result);
}

int main(int argc, char * argv[])
{
	int ready_queue_key, scheduler_mmu_queue_key, master_process_id;
	if (argc < 5) {
		printf("Scheduler ready_queue_key scheduler_mmu_queue_key total_processes master_process_id\n");
		exit(EXIT_FAILURE);
	}
	ready_queue_key = atoi(argv[1]);
	scheduler_mmu_queue_key = atoi(argv[2]);
	total_processes = atoi(argv[3]);
	master_process_id = atoi(argv[4]);

	scheduler_process_message outgoing_message, incoming_message;

	int ready_queue_id = msgget(ready_queue_key, 0666);
	int scheduler_mmu_queue_id = msgget(scheduler_mmu_queue_key, 0666);
	if (ready_queue_id == -1)
	{
		perror("Ready Queue creation failed");
		exit(1);
	}
	if (scheduler_mmu_queue_id == -1)
	{
		perror("Scheduler MMU Queue creation failed");
		exit(1);
	}
	printf("Total No. of Process received = %d\n", total_processes);

	int completed_processes_count = 0;
	while (1)
	{
		receive_message_from_process(ready_queue_id, MESSAGE_FROM_PROCESS, &incoming_message);
		int current_process_id = incoming_message.process_id;

		outgoing_message.message_type = MESSAGE_TO_PROCESS + current_process_id;
		send_message_to_process(ready_queue_id, &outgoing_message);

		mmu_to_scheduler_message mmu_notification;
		receive_message_from_mmu(scheduler_mmu_queue_id, 0, &mmu_notification);
		if (mmu_notification.message_type == PAGE_FAULT_RESOLVED)
		{
			outgoing_message.message_type = MESSAGE_FROM_PROCESS;
			outgoing_message.process_id = current_process_id;
			send_message_to_process(ready_queue_id, &outgoing_message);
		}
		else if (mmu_notification.message_type == PROCESS_TERMINATED)
		{
			completed_processes_count++;
>>>>>>> 92241c1 (Updated implementations)
		}
		else
		{
			perror("Wrong message from mmu\n");
			exit(1);
		}
<<<<<<< HEAD
		if (terminated_process == k)
			break;
		//printf("====Term %d====\n", terminated_process);
	}
	//printf("Sending sinal\n");
	kill(master_pid, SIGUSR1);
	pause();
	printf("Scheduler terminating ...\n") ;
	exit(1) ;
=======
		if (completed_processes_count == total_processes)
			break;
	}
	kill(master_process_id, SIGUSR1);
	pause();
	printf("Scheduler terminating ...\n");
	exit(1);
>>>>>>> 92241c1 (Updated implementations)
}