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