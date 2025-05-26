#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/msg.h> 
#include <sys/ipc.h>
#include <sys/types.h>
#include <limits.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>

<<<<<<< HEAD

#define PSEND_TYPE 10
#define MMUTOPRO 20
#define INVALID_PAGE_REF -2
#define PAGEFAULT -1
#define PROCESS_OVER -9
#define PAGEFAULT_HANDLED 5
#define TERMINATED 10

int count = 0;
int *pffreq;
FILE *resultf;
int i;

typedef struct {
	int frameno;
	int isvalid;
	int count;
} ptbentry;

typedef struct {
	pid_t pid;
	int m;
	int f_cnt;
	int f_allo;
} pcb;

typedef struct
{
	int current;
	int flist[];
} freelist;

struct msgbuf
{
	long mtype;
	int id;
	int pageno;
};

struct mmutopbuf
{
	long mtype;
	int frameno;
};

struct mmutosch
{
	long mtype;
	char mbuf[1];
};


key_t freekey, pagetbkey;
key_t msgq2key, msgq3key;
key_t pcbkey;

int ptbid, freelid;
int msgq2id, msgq3id;
int pcbid;


int m,k;

int readRequest(int* id)
{
	struct msgbuf mbuf;
	int length;
	/* The length is essentially the size of the structure minus sizeof(mtype) */
	length = sizeof(struct msgbuf) - sizeof(long);
	memset(&mbuf, 0, sizeof(mbuf));

	int rst = msgrcv(msgq3id, &mbuf, length, PSEND_TYPE, 0);
	if (rst == -1)
	{
		if(errno == EINTR)
			return -1;
		perror("msgrcv");
		exit(EXIT_FAILURE);
	}
	*id = mbuf.id;
	return mbuf.pageno;
}

void sendreply(int id, int frameno)
{
	struct mmutopbuf mbuf;
	mbuf.mtype = id + MMUTOPRO;
	mbuf.frameno = frameno;
	int length = sizeof(struct msgbuf) - sizeof(long);
	int rst = msgsnd(msgq3id, &mbuf, length, 0);
	if (rst == -1)
	{
		perror("msgsnd");
=======
#define PROCESS_REQUEST_TYPE 10
#define MMU_TO_PROCESS_TYPE 20
#define INVALID_PAGE_REFERENCE -2
#define PAGE_FAULT_OCCURRED -1
#define PROCESS_TERMINATION_SIGNAL -9
#define PAGE_FAULT_RESOLVED 5
#define PROCESS_TERMINATED 10

int global_access_counter = 0;
int *page_fault_frequency;
FILE *result_file;
int loop_index;

typedef struct {
	int frame_number;
	int in_memory;
	int access_count;
} page_table_entry;

typedef struct {
	pid_t process_id;
	int num_pages;
	int max_frames_allowed;
	int frames_allocated;
} process_control_block;

typedef struct
{
	int current_index;
	int frame_list[];
} frame_free_list;

struct process_request_message
{
	long message_type;
	int process_id;
	int page_number;
};

struct mmu_response_message
{
	long message_type;
	int frame_number;
};

struct mmu_scheduler_notification
{
	long message_type;
	char message_buffer[1];
};

key_t frame_list_key, page_table_key;
key_t scheduler_mmu_queue_key, process_mmu_queue_key;
key_t pcb_shared_memory_key;

int page_table_shared_id, frame_list_shared_id;
int scheduler_mmu_queue_id, process_mmu_queue_id;
int pcb_shared_memory_id;

int pages_per_process, total_processes;

int read_page_request(int* requesting_process_id)
{
	struct process_request_message incoming_message;
	int message_length;
	message_length = sizeof(struct process_request_message) - sizeof(long);
	memset(&incoming_message, 0, sizeof(incoming_message));

	int receive_result = msgrcv(process_mmu_queue_id, &incoming_message, message_length, PROCESS_REQUEST_TYPE, 0);
	if (receive_result == -1)
	{
		if(errno == EINTR)
			return -1;
		perror("message_receive");
		exit(EXIT_FAILURE);
	}
	*requesting_process_id = incoming_message.process_id;
	return incoming_message.page_number;
}

void send_response_to_process(int process_id, int frame_number)
{
	struct mmu_response_message response_message;
	response_message.message_type = process_id + MMU_TO_PROCESS_TYPE;
	response_message.frame_number = frame_number;
	int message_length = sizeof(struct process_request_message) - sizeof(long);
	int send_result = msgsnd(process_mmu_queue_id, &response_message, message_length, 0);
	if (send_result == -1)
	{
		perror("message_send");
>>>>>>> 92241c1 (Updated implementations)
		exit(EXIT_FAILURE);
	}
}

<<<<<<< HEAD
void notifySched(int type)
{
	struct mmutosch mbuf;
	mbuf.mtype = type;
	int length = sizeof(struct msgbuf) - sizeof(long);
	int rst = msgsnd(msgq2id, &mbuf, length, 0);
	if (rst == -1)
	{
		perror("msgsnd");
		exit(EXIT_FAILURE);
	}
	//printf("Sent signal to sched = %d\n",type);

}
pcb *pcbptr;
ptbentry *ptbptr;
freelist *freeptr;

int handlePageFault(int id, int pageno)
{
	int i;
	if (freeptr->current == -1 || pcbptr[i].f_cnt <= pcbptr[i].f_allo)
	{
		int min = INT_MAX, mini = -1;
		int victim = 0;
		for (i = 0; i < pcbptr[i].m; i++)
		{
			if (ptbptr[id * m + i].isvalid == 1)
			{
				if (ptbptr[id * m + i].count < min)
				{
					min = ptbptr[id * m + i].count;
					victim = ptbptr[id * m + i].frameno;
					mini = i;
				}
			}
		}
		ptbptr[id * m + mini].isvalid = 0;
		return victim;
	}
	else
	{
		int fn = freeptr->flist[freeptr->current];
		freeptr->current -= 1;
		return fn;
	}
}

void freepages(int i)
{

	int k = 0;
	for (k = 0; k < pcbptr[i].m; i++)
	{
		if (ptbptr[i * m + k].isvalid == 1)
		{
			freeptr->flist[freeptr->current + 1] = ptbptr[i * m + k].frameno;
			freeptr->current += 1;
		}
	}
	//TODO: Think shuld change the allocation
}

int serviceMRequest()
{
	pcbptr = (pcb*)(shmat(pcbid, NULL, 0));
	/*if (*(int *)pcbptr == -1)
	{
		perror("pcb-shmat");
		exit(EXIT_FAILURE);
	}*/
	ptbptr = (ptbentry*)(shmat(ptbid, NULL, 0));
	/*if (*(int *)ptbptr == -1)
	{
		perror("pcb-shmat");
		exit(EXIT_FAILURE);
	}*/
	freeptr = (freelist*)(shmat(freelid, NULL, 0));
	/*if (*((int *)freeptr) == -1)
	{
		perror("freel-shmat");
		exit(EXIT_FAILURE);
	}*/

	int id = -1, pageno;
	pageno = readRequest(&id);
	if(pageno == -1 && id == -1)
	{
		return 0;
	}
	int i = id;
	if (pageno == PROCESS_OVER)
	{
		freepages(id);
		notifySched(TERMINATED);
		//printf("Notifying notifySched\n");
		return;
	}
	count ++;
	printf("Page reference : (%d,%d,%d)\n",count,id,pageno);
	fprintf(resultf,"Page reference : (%d,%d,%d)\n",count,id,pageno);
	if (pcbptr[id].m < pageno || pageno < 0)
	{
		printf("Invalid Page Reference : (%d %d)\n",id,pageno);
		fprintf(resultf,"Invalid Page Reference : (%d %d)\n",id,pageno);
		sendreply(id, INVALID_PAGE_REF);
		printf("Process %d: TRYING TO ACCESS INVALID PAGE REFERENCE %d\n", id, pageno);
		freepages(id);
		notifySched(TERMINATED);
		//Invalid reference

	}
	else
	{
		if (ptbptr[i * m + pageno].isvalid == 0)
		{
			//PAGE FAULT
			printf("Page Fault : (%d, %d)\n",id,pageno);
			fprintf(resultf,"Page Fault : (%d, %d)\n",id,pageno);
			pffreq[id] += 1;
			sendreply(id, -1);
			int fno = handlePageFault(id, pageno);
			ptbptr[i * m + pageno].isvalid = 1;
			ptbptr[i * m + pageno].count = count;
			ptbptr[i * m + pageno].frameno = fno;
			
			notifySched(PAGEFAULT_HANDLED);
		}
		else
		{
			sendreply(id, ptbptr[i * m + pageno].frameno);
			ptbptr[i * m + pageno].count = count;
			//FRAME FOUND
		}
	}
	if(shmdt(pcbptr) == -1)
	{
		perror("pcbptr-shmdt");
		exit(EXIT_FAILURE);
	}
	if(shmdt(ptbptr) == -1)
	{
		perror("ptbptr-shmdt");
		exit(EXIT_FAILURE);
	}
	if(shmdt(freeptr) == -1)
	{
		perror("freel-shmdt");
		exit(EXIT_FAILURE);
	}
}
int flag = 1;
void handletgerm(int sig)
{
	//printf("I am a god kill me\n");
	flag = 0;
=======
void notify_scheduler(int notification_type)
{
	struct mmu_scheduler_notification notification_message;
	notification_message.message_type = notification_type;
	int message_length = sizeof(struct process_request_message) - sizeof(long);
	int send_result = msgsnd(scheduler_mmu_queue_id, &notification_message, message_length, 0);
	if (send_result == -1)
	{
		perror("message_send");
		exit(EXIT_FAILURE);
	}
}

process_control_block *pcb_pointer;
page_table_entry *page_table_pointer;
frame_free_list *frame_list_pointer;

int handle_page_fault(int process_id, int page_number)
{
	int page_index;
	if (frame_list_pointer->current_index == -1 || pcb_pointer[process_id].max_frames_allowed <= pcb_pointer[process_id].frames_allocated)
	{
		int minimum_access_count = INT_MAX, minimum_page_index = -1;
		int victim_frame = 0;
		for (page_index = 0; page_index < pcb_pointer[process_id].num_pages; page_index++)
		{
			if (page_table_pointer[process_id * pages_per_process + page_index].in_memory == 1)
			{
				if (page_table_pointer[process_id * pages_per_process + page_index].access_count < minimum_access_count)
				{
					minimum_access_count = page_table_pointer[process_id * pages_per_process + page_index].access_count;
					victim_frame = page_table_pointer[process_id * pages_per_process + page_index].frame_number;
					minimum_page_index = page_index;
				}
			}
		}
		page_table_pointer[process_id * pages_per_process + minimum_page_index].in_memory = 0;
		return victim_frame;
	}
	else
	{
		int available_frame = frame_list_pointer->frame_list[frame_list_pointer->current_index];
		frame_list_pointer->current_index -= 1;
		return available_frame;
	}
}

void free_process_pages(int process_id)
{
	int page_index = 0;
	for (page_index = 0; page_index < pcb_pointer[process_id].num_pages; page_index++)
	{
		if (page_table_pointer[process_id * pages_per_process + page_index].in_memory == 1)
		{
			frame_list_pointer->frame_list[frame_list_pointer->current_index + 1] = page_table_pointer[process_id * pages_per_process + page_index].frame_number;
			frame_list_pointer->current_index += 1;
		}
	}
}

int service_memory_request()
{
	pcb_pointer = (process_control_block*)(shmat(pcb_shared_memory_id, NULL, 0));
	page_table_pointer = (page_table_entry*)(shmat(page_table_shared_id, NULL, 0));
	frame_list_pointer = (frame_free_list*)(shmat(frame_list_shared_id, NULL, 0));

	int requesting_process_id = -1, requested_page_number;
	requested_page_number = read_page_request(&requesting_process_id);
	if(requested_page_number == -1 && requesting_process_id == -1)
	{
		return 0;
	}
	int current_process_index = requesting_process_id;
	if (requested_page_number == PROCESS_TERMINATION_SIGNAL)
	{
		free_process_pages(requesting_process_id);
		notify_scheduler(PROCESS_TERMINATED);
		return;
	}
	global_access_counter++;
	printf("Page reference : (%d,%d,%d)\n", global_access_counter, requesting_process_id, requested_page_number);
	fprintf(result_file, "Page reference : (%d,%d,%d)\n", global_access_counter, requesting_process_id, requested_page_number);
	if (pcb_pointer[requesting_process_id].num_pages < requested_page_number || requested_page_number < 0)
	{
		printf("Invalid Page Reference : (%d %d)\n", requesting_process_id, requested_page_number);
		fprintf(result_file, "Invalid Page Reference : (%d %d)\n", requesting_process_id, requested_page_number);
		send_response_to_process(requesting_process_id, INVALID_PAGE_REFERENCE);
		printf("Process %d: TRYING TO ACCESS INVALID PAGE REFERENCE %d\n", requesting_process_id, requested_page_number);
		free_process_pages(requesting_process_id);
		notify_scheduler(PROCESS_TERMINATED);
	}
	else
	{
		if (page_table_pointer[current_process_index * pages_per_process + requested_page_number].in_memory == 0)
		{
			printf("Page Fault : (%d, %d)\n", requesting_process_id, requested_page_number);
			fprintf(result_file, "Page Fault : (%d, %d)\n", requesting_process_id, requested_page_number);
			page_fault_frequency[requesting_process_id] += 1;
			send_response_to_process(requesting_process_id, -1);
			int allocated_frame_number = handle_page_fault(requesting_process_id, requested_page_number);
			page_table_pointer[current_process_index * pages_per_process + requested_page_number].in_memory = 1;
			page_table_pointer[current_process_index * pages_per_process + requested_page_number].access_count = global_access_counter;
			page_table_pointer[current_process_index * pages_per_process + requested_page_number].frame_number = allocated_frame_number;
			
			notify_scheduler(PAGE_FAULT_RESOLVED);
		}
		else
		{
			send_response_to_process(requesting_process_id, page_table_pointer[current_process_index * pages_per_process + requested_page_number].frame_number);
			page_table_pointer[current_process_index * pages_per_process + requested_page_number].access_count = global_access_counter;
		}
	}
	if(shmdt(pcb_pointer) == -1)
	{
		perror("pcb_shared_memory_detach");
		exit(EXIT_FAILURE);
	}
	if(shmdt(page_table_pointer) == -1)
	{
		perror("page_table_shared_memory_detach");
		exit(EXIT_FAILURE);
	}
	if(shmdt(frame_list_pointer) == -1)
	{
		perror("frame_list_shared_memory_detach");
		exit(EXIT_FAILURE);
	}
	return 1;
}

int continue_execution = 1;

void handle_termination_signal(int signal_number)
{
	continue_execution = 0;
>>>>>>> 92241c1 (Updated implementations)
}

int main(int argc, char const *argv[])
{
<<<<<<< HEAD
	if (argc < 4)
	{
		printf("mmu m2key m3key ptbkey fkey pcbkey m k\n");
		exit(EXIT_FAILURE);
	}
	msgq2id = atoi(argv[1]);
	msgq3id = atoi(argv[2]);
	ptbid = atoi(argv[3]);
	freelid = atoi(argv[4]);
	pcbid = atoi(argv[5]);
	m = atoi(argv[6]);
	k = atoi(argv[7]);
	signal(SIGUSR2, handletgerm);
	pffreq = (int *)malloc(k*sizeof(int));
	for(i=0;i<k;i++)
	{
		pffreq[i] = 0;
	} 
	resultf = fopen("result.txt","w");
	while(flag)
	{
		serviceMRequest();
	}
	printf("Page fault Count for each Process:\n");	
	fprintf(resultf,"Page fault Count for each Process:\n");
	printf("Process Id\tFreq\n");
	fprintf(resultf,"Process Id\tFreq\n");
	for(i = 0;i<k;i++)
	{
		printf("%d\t%d\n",i,pffreq[i]);
		fprintf(resultf,"%d\t%d\n",i,pffreq[i]);
	}
	fclose(resultf);
=======
	if (argc < 8)
	{
		printf("mmu scheduler_mmu_queue_id process_mmu_queue_id page_table_shared_id frame_list_shared_id pcb_shared_memory_id pages_per_process total_processes\n");
		exit(EXIT_FAILURE);
	}
	scheduler_mmu_queue_id = atoi(argv[1]);
	process_mmu_queue_id = atoi(argv[2]);
	page_table_shared_id = atoi(argv[3]);
	frame_list_shared_id = atoi(argv[4]);
	pcb_shared_memory_id = atoi(argv[5]);
	pages_per_process = atoi(argv[6]);
	total_processes = atoi(argv[7]);
	signal(SIGUSR2, handle_termination_signal);
	page_fault_frequency = (int *)malloc(total_processes * sizeof(int));
	for(loop_index = 0; loop_index < total_processes; loop_index++)
	{
		page_fault_frequency[loop_index] = 0;
	} 
	result_file = fopen("result.txt", "w");
	while(continue_execution)
	{
		service_memory_request();
	}
	printf("Page fault Count for each Process:\n");	
	fprintf(result_file, "Page fault Count for each Process:\n");
	printf("Process Id\tFreq\n");
	fprintf(result_file, "Process Id\tFreq\n");
	for(loop_index = 0; loop_index < total_processes; loop_index++)
	{
		printf("%d\t%d\n", loop_index, page_fault_frequency[loop_index]);
		fprintf(result_file, "%d\t%d\n", loop_index, page_fault_frequency[loop_index]);
	}
	fclose(result_file);
>>>>>>> 92241c1 (Updated implementations)
	return 0;
}