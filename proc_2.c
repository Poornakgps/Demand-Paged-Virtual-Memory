<<<<<<< HEAD
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

#define MAX_BUFFER_SIZE 100
#define MAX_PAGES 1000
#define TOSCH 10
#define FROMSCH 20  // FROMSCH+id will be used to read msg from sch
#define TOMMU 10
#define FROMMMU 20 // FROMMMU+id will be used to read msg from MMU

int pg_no[MAX_PAGES] ;
int no_of_pages;

typedef struct mmumsgbuf_send {
	long    mtype;          /* Message type */
	int id;
	int pageno;
} mmumsgbuf_send;

typedef struct mmumsgbuf_recv {
	long    mtype;          /* Message type */
	int frameno;
} mmumsgbuf_recv;

typedef struct mymsgbuf {
	long    mtype;          /* Message type */
	int id;
} mymsgbuf;

void conv_ref_pg_no(char * refs)
{
	const char s[2] = "|";
	char *token;
	token = strtok(refs, s);
	while ( token != NULL )
	{
		pg_no[no_of_pages] = atoi(token);
		no_of_pages++;
		token = strtok(NULL, s);
	}
}

int send_message_mmu( int qid, struct mmumsgbuf_send *qbuf )
{
	int     result, length;

	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(struct mmumsgbuf_send) - sizeof(long);

	if ((result = msgsnd( qid, qbuf, length, 0)) == -1)
	{
		perror("Error in sending message");
		exit(1);
	}

	return (result);
}
int read_message_mmu( int qid, long type, struct mmumsgbuf_recv *qbuf )
{
	int     result, length;

	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(struct mmumsgbuf_recv) - sizeof(long);

	if ((result = msgrcv( qid, qbuf, length, type,  0)) == -1)
	{
		perror("Error in receiving message");
		exit(1);
	}

	return (result);
}

int send_message( int qid, struct mymsgbuf *qbuf )
{
	int     result, length;

	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(struct mymsgbuf) - sizeof(long);

	if ((result = msgsnd( qid, qbuf, length, 0)) == -1)
	{
		perror("Error in sending message");
		exit(1);
	}

	return (result);
}
int read_message( int qid, long type, struct mymsgbuf *qbuf )
{
	int     result, length;

	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(struct mymsgbuf) - sizeof(long);

	if ((result = msgrcv( qid, qbuf, length, type,  0)) == -1)
	{
		perror("Error in receiving message");
		exit(1);
	}

	return (result);
}

int main(int argc, char *argv[]) //argv[] ={id,mq1,mq3,ref_string}
{
	if (argc < 5)
	{
		perror("Please give 5 arguments {id,mq1,mq3,ref_string}\n");
		exit(1);
	}
	int id, mq1_k, mq3_k;
	id = atoi(argv[1]);
	mq1_k = atoi(argv[2]);
	mq3_k = atoi(argv[3]);
	no_of_pages = 0;
	conv_ref_pg_no(argv[4]);
	int mq1, mq3;
	mq1 = msgget(mq1_k, 0666);
	mq3 = msgget(mq3_k, 0666);
	if (mq1 == -1)
	{
		perror("Message Queue1 creation failed");
		exit(1);
	}
	if (mq3 == -1)
	{
		perror("Message Queue3 creation failed");
		exit(1);
	}
	printf("Process id= %d\n", id);

	//sending to scheduler
	mymsgbuf msg_send;
	msg_send.mtype = TOSCH;
	msg_send.id = id;
	send_message(mq1, &msg_send);
	////

	//Wait until msg receive from scheduler
	mymsgbuf msg_recv;
	read_message(mq1, FROMSCH + id, &msg_recv);
	/////////

	mmumsgbuf_send mmu_send;
	mmumsgbuf_recv mmu_recv;
	int cpg = 0; //counter for page number array
	while (cpg < no_of_pages)
	{
		// sending msg to mmu the page number
		printf("Sent request for %d page number\n", pg_no[cpg]);
		mmu_send.mtype = TOMMU;
		mmu_send.id = id;
		mmu_send.pageno = pg_no[cpg];
		send_message_mmu(mq3, &mmu_send);

		read_message_mmu(mq3, FROMMMU + id, &mmu_recv);
		if (mmu_recv.frameno >= 0)
		{
			printf("Frame number from MMU received for process %d: %d\n" , id, mmu_recv.frameno);
			cpg++;
		}
		else if (mmu_recv.frameno == -1) //here cpg will not be incremented
		{
			printf("Page fault occured for process %d\n", id);
			// read_message(mq1, FROMSCH + id, &msg_recv);
		}
		else if (mmu_recv.frameno == -2)
		{
			printf("Invalid page reference for process %d terminating ...\n", id) ;
			exit(1);
		}
	}
	printf("Process %d Terminated successfly\n", id);
	mmu_send.pageno = -9;
	mmu_send.id = id;
	mmu_send.mtype = TOMMU;
	send_message_mmu(mq3, &mmu_send);

	exit(1);
	return 0;
}


=======
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

#define MAX_BUFFER_SIZE 100
#define MAX_PAGES 1000
#define MESSAGE_TO_SCHEDULER 10
#define MESSAGE_FROM_SCHEDULER 20
#define MESSAGE_TO_MMU 10
#define MESSAGE_FROM_MMU 20

int page_reference_array[MAX_PAGES];
int total_page_references;

typedef struct process_to_mmu_message {
	long    message_type;
	int process_id;
	int page_number;
} process_to_mmu_message;

typedef struct mmu_to_process_message {
	long    message_type;
	int frame_number;
} mmu_to_process_message;

typedef struct process_scheduler_message {
	long    message_type;
	int process_id;
} process_scheduler_message;

void convert_reference_string_to_page_numbers(char * reference_string)
{
	const char delimiter[2] = "|";
	char *token;
	token = strtok(reference_string, delimiter);
	while (token != NULL)
	{
		page_reference_array[total_page_references] = atoi(token);
		total_page_references++;
		token = strtok(NULL, delimiter);
	}
}

int send_message_to_mmu(int queue_id, struct process_to_mmu_message *message_buffer)
{
	int send_result, message_length;

	message_length = sizeof(struct process_to_mmu_message) - sizeof(long);

	if ((send_result = msgsnd(queue_id, message_buffer, message_length, 0)) == -1)
	{
		perror("Error in sending message");
		exit(1);
	}

	return (send_result);
}

int receive_message_from_mmu(int queue_id, long message_type, struct mmu_to_process_message *message_buffer)
{
	int receive_result, message_length;

	message_length = sizeof(struct mmu_to_process_message) - sizeof(long);

	if ((receive_result = msgrcv(queue_id, message_buffer, message_length, message_type, 0)) == -1)
	{
		perror("Error in receiving message");
		exit(1);
	}

	return (receive_result);
}

int send_message_to_scheduler(int queue_id, struct process_scheduler_message *message_buffer)
{
	int send_result, message_length;

	message_length = sizeof(struct process_scheduler_message) - sizeof(long);

	if ((send_result = msgsnd(queue_id, message_buffer, message_length, 0)) == -1)
	{
		perror("Error in sending message");
		exit(1);
	}

	return (send_result);
}

int receive_message_from_scheduler(int queue_id, long message_type, struct process_scheduler_message *message_buffer)
{
	int receive_result, message_length;

	message_length = sizeof(struct process_scheduler_message) - sizeof(long);

	if ((receive_result = msgrcv(queue_id, message_buffer, message_length, message_type, 0)) == -1)
	{
		perror("Error in receiving message");
		exit(1);
	}

	return (receive_result);
}

int main(int argc, char *argv[])
{
	if (argc < 5)
	{
		perror("Please give 5 arguments {process_id, ready_queue_key, process_mmu_queue_key, reference_string}\n");
		exit(1);
	}
	int process_id, ready_queue_key, process_mmu_queue_key;
	process_id = atoi(argv[1]);
	ready_queue_key = atoi(argv[2]);
	process_mmu_queue_key = atoi(argv[3]);
	total_page_references = 0;
	convert_reference_string_to_page_numbers(argv[4]);
	int ready_queue_id, process_mmu_queue_id;
	ready_queue_id = msgget(ready_queue_key, 0666);
	process_mmu_queue_id = msgget(process_mmu_queue_key, 0666);
	if (ready_queue_id == -1)
	{
		perror("Ready Queue creation failed");
		exit(1);
	}
	if (process_mmu_queue_id == -1)
	{
		perror("Process MMU Queue creation failed");
		exit(1);
	}
	printf("Process id= %d\n", process_id);

	process_scheduler_message scheduler_message;
	scheduler_message.message_type = MESSAGE_TO_SCHEDULER;
	scheduler_message.process_id = process_id;
	send_message_to_scheduler(ready_queue_id, &scheduler_message);

	process_scheduler_message scheduler_response;
	receive_message_from_scheduler(ready_queue_id, MESSAGE_FROM_SCHEDULER + process_id, &scheduler_response);

	process_to_mmu_message mmu_request;
	mmu_to_process_message mmu_response;
	int current_page_index = 0;
	while (current_page_index < total_page_references)
	{
		printf("Sent request for %d page number\n", page_reference_array[current_page_index]);
		mmu_request.message_type = MESSAGE_TO_MMU;
		mmu_request.process_id = process_id;
		mmu_request.page_number = page_reference_array[current_page_index];
		send_message_to_mmu(process_mmu_queue_id, &mmu_request);

		receive_message_from_mmu(process_mmu_queue_id, MESSAGE_FROM_MMU + process_id, &mmu_response);
		if (mmu_response.frame_number >= 0)
		{
			printf("Frame number from MMU received for process %d: %d\n", process_id, mmu_response.frame_number);
			current_page_index++;
		}
		else if (mmu_response.frame_number == -1)
		{
			printf("Page fault occured for process %d\n", process_id);
		}
		else if (mmu_response.frame_number == -2)
		{
			printf("Invalid page reference for process %d terminating ...\n", process_id);
			exit(1);
		}
	}
	printf("Process %d Terminated successfully\n", process_id);
	mmu_request.page_number = -9;
	mmu_request.process_id = process_id;
	mmu_request.message_type = MESSAGE_TO_MMU;
	send_message_to_mmu(process_mmu_queue_id, &mmu_request);

	exit(1);
	return 0;
}
>>>>>>> 92241c1 (Updated implementations)
