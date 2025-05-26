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


typedef struct {
<<<<<<< HEAD
	int frameno;
	int isvalid; 
	int count; //for LRU
}ptbentry; //page table entry

typedef struct {
	pid_t pid; // process id
	int m; // no. of pages
	int f_cnt; // no. of frames
	int f_allo; // no. of frames allocated
}pcb; //process control block

typedef struct 
{
	int current; // current index
	int flist[]; // free list
}freelist; //free list

int k,m,f;
int flag = 0;
key_t freekey,pagetbkey;
key_t readykey, msgq2key, msgq3key;
key_t pcbkey;

int ptbid, freelid;
int readyid, msgq2id, msgq3id;
int pcbid;

void print_PCB(pcb p)
{
	printf("PID = %d m = %d f_cnt = %d\n",p.pid,p.m,p.f_cnt);

}
int max(int a, int b)
{
	return (a>b)?a:b;
}

int min(int a,int b)
{
	return (a<b)?a:b;	
}

void myexit(int status);

void createFreeList()
{
	int i;
	freekey = ftok("master.c",56);
	if(freekey == -1)
	{	
		perror("freekey");
		myexit(EXIT_FAILURE);
	}
	freelid = shmget(freekey, sizeof(freelist)+f*sizeof(int), 0666 | IPC_CREAT | IPC_EXCL);
	if(freelid == -1)
	{	
		perror("free-shmget");
		myexit(EXIT_FAILURE);
	}

	freelist *ptr = (freelist*)(shmat(freelid, NULL, 0));
	if(*((int *)ptr) == -1)
	{
		perror("freel-shmat");
		myexit(EXIT_FAILURE);
	}
	for(i=0;i<f;i++)
	{
		ptr->flist[i] = i;
	}
	ptr->current = f-1;

	if(shmdt(ptr) == -1)
	{
		perror("freel-shmdt");
		myexit(EXIT_FAILURE);
	}
}

void createPageTables()
{
	int i;
	pagetbkey = ftok("master.c",100);
	if(pagetbkey == -1)
	{	
		perror("pagetbkey");
		myexit(EXIT_FAILURE);
	}
	ptbid = shmget(pagetbkey, m*sizeof(ptbentry)*k, 0666 | IPC_CREAT | IPC_EXCL);
	if(ptbid == -1)
	{	
		perror("pcb-shmget");
		myexit(EXIT_FAILURE);
	}

	ptbentry *ptr = (ptbentry*)(shmat(ptbid, NULL, 0));
	if(*(int *)ptr == -1)
	{
		perror("pcb-shmat");
		myexit(EXIT_FAILURE);
	}

	for(i=0;i<k*m;i++)
	{
		ptr[i].frameno = -1;
		ptr[i].isvalid = 0;
	}

	if(shmdt(ptr) == -1)
	{
		perror("pcb-shmdt");
		myexit(EXIT_FAILURE);
	}
}


void createMessageQueues()
{
	readykey = ftok("master.c",200);
	if(readykey == -1)
	{	
		perror("readykey");
		myexit(EXIT_FAILURE);
	}
	readyid = msgget(readykey, 0666 | IPC_CREAT| IPC_EXCL);
	if(readyid == -1)
	{
		perror("ready-msgget");
		myexit(EXIT_FAILURE);
	}

	msgq2key = ftok("master.c",300);
	if(msgq2key == -1)
	{	
		perror("msgq2key");
		myexit(EXIT_FAILURE);
	}
	msgq2id = msgget(msgq2key, 0666 | IPC_CREAT| IPC_EXCL );
	if(msgq2id == -1)
	{
		perror("msgq2-msgget");
		myexit(EXIT_FAILURE);
	} 

	msgq3key = ftok("master.c",400);
	if(msgq3key == -1)
	{	
		perror("msgq3key");
		myexit(EXIT_FAILURE);
	}
	msgq3id = msgget(msgq3key, 0666 | IPC_CREAT| IPC_EXCL);
	if(msgq3id == -1)
	{
		perror("msgq3-msgget");
		myexit(EXIT_FAILURE);
	} 
}

void createPCBs()
{
	int i;
	pcbkey = ftok("master.c",500);
	if(pcbkey == -1)
	{	
		perror("pcbkey");
		myexit(EXIT_FAILURE);
	}
	pcbid = shmget(pcbkey, sizeof(pcb)*k, 0666 | IPC_CREAT | IPC_EXCL );
	if(pcbid == -1)
	{	
		perror("pcb-shmget");
		myexit(EXIT_FAILURE);
	}

	pcb *ptr = (pcb*)(shmat(pcbid, NULL, 0));
	if(*(int *)ptr == -1)
	{
		perror("pcb-shmat");
		myexit(EXIT_FAILURE);
	}

	int totpages = 0;
	for(i=0;i<k;i++)
	{
		ptr[i].pid = i;
		ptr[i].m = rand()%m + 1;
		ptr[i].f_allo = 0;
		totpages +=  ptr[i].m;
	}
	int allo_frame = 0;
	printf("tot = %d, k = %d, f=  %d\n",totpages,k,f);
	int max = 0,maxi = 0;
	for(i=0;i<k;i++)
	{
		ptr[i].pid = -1;
		int allo = (int)round(ptr[i].m*(f-k)/(float)totpages) + 1;
		if(ptr[i].m > max)
		{
			max = ptr[i].m;
			maxi = i;
		}
		allo_frame = allo_frame + allo;
		//printf("%d\n",allo);
		ptr[i].f_cnt = allo;
		
	}
	ptr[maxi].f_cnt += f - allo_frame; 

	for(i=0;i<k;i++)
	{
		print_PCB(ptr[i]);
	}

	if(shmdt(ptr) == -1)
	{
		perror("freel-shmdt");
		myexit(EXIT_FAILURE);
	}

}

void clearResources()
{
	if(shmctl(ptbid,IPC_RMID, NULL) == -1)
	{
		perror("shmctl-ptb");
	}
	if(shmctl(freelid,IPC_RMID, NULL) == -1)
	{
		perror("shmctl-freel");
	}
	if(shmctl(pcbid,IPC_RMID, NULL) == -1)
	{
		perror("shmctl-pcb");
	}
	if(msgctl(readyid, IPC_RMID, NULL) == -1)
	{
		perror("msgctl-ready");
	}
	if(msgctl(msgq2id, IPC_RMID, NULL) == -1)
	{
		perror("msgctl-msgq2");
	}
	if(msgctl(msgq3id, IPC_RMID, NULL) == -1)
	{
		perror("msgctl-msgq3");
	}
}

void myexit(int status)
{
	clearResources();
	exit(status);
}


void createProcesses()
{
	pcb *ptr = (pcb*)(shmat(pcbid, NULL, 0));
	/*if(*(int *)ptr == -1)
	{
		perror("pcb-shmat");
		myexit(EXIT_FAILURE);
	}*/

	int i,j;
	for(i=0;i<k;i++)
	{
		int rlen = rand()%(8*ptr[i].m) + 2*ptr[i].m + 1;
		char rstring[m*20*40];
		printf("rlen = %d\n",rlen);
		int l = 0;
		for(j=0;j<rlen;j++)
		{
			int r;
			r = rand()%ptr[i].m;
			float p = (rand()%100)/100.0;
			if(p < 0.2)
			{
				r = rand()%(1000*m) + ptr[i].m;
			}
			l += sprintf(rstring+l,"%d|",r);
		}
		printf("Ref string = %s\n",rstring);
		if(fork() == 0)
		{
			char buf1[20],buf2[20],buf3[20];
			sprintf(buf1,"%d",i);
			sprintf(buf2,"%d",readykey);
			sprintf(buf3,"%d",msgq3key);
			execlp("./process","./process",buf1,buf2,buf3,rstring,(char *)(NULL));
			exit(0);

		}
		//TODO: fork proecess here
		usleep(1*1000);	
	}

}
int pid,spid,mpid;

void timetoend(int sig)
{
	//printf("Mater: gi=o the signal\n");
	sleep(1);
	kill(spid, SIGTERM);
	kill(mpid, SIGUSR2);
	sleep(2);
	flag = 1;

}
int main(int argc, char const *argv[])
{
	srand(time(NULL));
	signal(SIGUSR1, timetoend);
	signal(SIGINT, myexit);
	if(argc < 4)
	{
		printf("master k m f\n");
		myexit(EXIT_FAILURE);
	}
	k = atoi(argv[1]);
	m = atoi(argv[2]);
	f = atoi(argv[3]);
	pid = getpid();
	if(k <= 0 || m <= 0 || f <=0 || f < k)
	{
		printf("Invalid input\n");
		myexit(EXIT_FAILURE);
	}

	createPageTables();
	createFreeList();
	createPCBs();
	createMessageQueues();

	if((spid = fork()) == 0)
	{
		char buf1[20],buf2[20],buf3[20],buf4[20];
		sprintf(buf1,"%d",readykey);
		sprintf(buf2,"%d",msgq2key);
		sprintf(buf3,"%d",k);
		sprintf(buf4,"%d",pid);
		execlp("./scheduler","./scheduler",buf1,buf2,buf3,buf4,(char *)(NULL));
		exit(0);
	}


	if((mpid = fork()) == 0)
	{
		char buf1[20],buf2[20],buf3[20],buf4[20],buf5[20],buf6[20],buf7[20];
		sprintf(buf1,"%d",msgq2id);
		sprintf(buf2,"%d",msgq3id);
		sprintf(buf3,"%d",ptbid);
		sprintf(buf4,"%d",freelid);
		sprintf(buf5,"%d",pcbid);
		sprintf(buf6,"%d",m);
		sprintf(buf7,"%d",k);
		execlp("./mmu","./mmu",buf1,buf2,buf3,buf4,buf5,buf6,buf7,(char *)(NULL));
		exit(0);
	}
	printf("generating processed\n");
	createProcesses();
	if(flag == 0)
		pause();
	clearResources();
=======
	int frame_number;
	int in_memory; 
	int access_count;
}page_table_entry;

typedef struct {
	pid_t process_id;
	int num_pages;
	int max_frames_allowed;
	int frames_allocated;
}process_control_block;

typedef struct 
{
	int current_index;
	int frame_list[];
}frame_free_list;

int num_processes, pages_per_process, total_frames;
int shutdown_flag = 0;
key_t frame_list_key, page_table_key;
key_t ready_queue_key, scheduler_mmu_queue_key, process_mmu_queue_key;
key_t pcb_shared_memory_key;

int page_table_shared_id, frame_list_shared_id;
int ready_queue_id, scheduler_mmu_queue_id, process_mmu_queue_id;
int pcb_shared_memory_id;

void print_process_control_block(process_control_block process_info)
{
	printf("PID = %d num_pages = %d max_frames = %d\n", process_info.process_id, process_info.num_pages, process_info.max_frames_allowed);
}

int maximum_value(int first_value, int second_value)
{
	return (first_value > second_value) ? first_value : second_value;
}

int minimum_value(int first_value, int second_value)
{
	return (first_value < second_value) ? first_value : second_value;	
}

void cleanup_and_exit(int exit_status);

void create_frame_free_list()
{
	int frame_index;
	frame_list_key = ftok("master.c", 56);
	if(frame_list_key == -1)
	{	
		perror("frame_list_key");
		cleanup_and_exit(EXIT_FAILURE);
	}
	frame_list_shared_id = shmget(frame_list_key, sizeof(frame_free_list) + total_frames * sizeof(int), 0666 | IPC_CREAT | IPC_EXCL);
	if(frame_list_shared_id == -1)
	{	
		perror("frame_list_shared_memory_get");
		cleanup_and_exit(EXIT_FAILURE);
	}

	frame_free_list *frame_list_pointer = (frame_free_list*)(shmat(frame_list_shared_id, NULL, 0));
	if(*((int *)frame_list_pointer) == -1)
	{
		perror("frame_list_shared_memory_attach");
		cleanup_and_exit(EXIT_FAILURE);
	}
	for(frame_index = 0; frame_index < total_frames; frame_index++)
	{
		frame_list_pointer->frame_list[frame_index] = frame_index;
	}
	frame_list_pointer->current_index = total_frames - 1;

	if(shmdt(frame_list_pointer) == -1)
	{
		perror("frame_list_shared_memory_detach");
		cleanup_and_exit(EXIT_FAILURE);
	}
}

void create_page_tables()
{
	int entry_index;
	page_table_key = ftok("master.c", 100);
	if(page_table_key == -1)
	{	
		perror("page_table_key");
		cleanup_and_exit(EXIT_FAILURE);
	}
	page_table_shared_id = shmget(page_table_key, pages_per_process * sizeof(page_table_entry) * num_processes, 0666 | IPC_CREAT | IPC_EXCL);
	if(page_table_shared_id == -1)
	{	
		perror("page_table_shared_memory_get");
		cleanup_and_exit(EXIT_FAILURE);
	}

	page_table_entry *page_table_pointer = (page_table_entry*)(shmat(page_table_shared_id, NULL, 0));
	if(*(int *)page_table_pointer == -1)
	{
		perror("page_table_shared_memory_attach");
		cleanup_and_exit(EXIT_FAILURE);
	}

	for(entry_index = 0; entry_index < num_processes * pages_per_process; entry_index++)
	{
		page_table_pointer[entry_index].frame_number = -1;
		page_table_pointer[entry_index].in_memory = 0;
	}

	if(shmdt(page_table_pointer) == -1)
	{
		perror("page_table_shared_memory_detach");
		cleanup_and_exit(EXIT_FAILURE);
	}
}

void create_message_queues()
{
	ready_queue_key = ftok("master.c", 200);
	if(ready_queue_key == -1)
	{	
		perror("ready_queue_key");
		cleanup_and_exit(EXIT_FAILURE);
	}
	ready_queue_id = msgget(ready_queue_key, 0666 | IPC_CREAT| IPC_EXCL);
	if(ready_queue_id == -1)
	{
		perror("ready_queue_message_get");
		cleanup_and_exit(EXIT_FAILURE);
	}

	scheduler_mmu_queue_key = ftok("master.c", 300);
	if(scheduler_mmu_queue_key == -1)
	{	
		perror("scheduler_mmu_queue_key");
		cleanup_and_exit(EXIT_FAILURE);
	}
	scheduler_mmu_queue_id = msgget(scheduler_mmu_queue_key, 0666 | IPC_CREAT| IPC_EXCL);
	if(scheduler_mmu_queue_id == -1)
	{
		perror("scheduler_mmu_queue_message_get");
		cleanup_and_exit(EXIT_FAILURE);
	} 

	process_mmu_queue_key = ftok("master.c", 400);
	if(process_mmu_queue_key == -1)
	{	
		perror("process_mmu_queue_key");
		cleanup_and_exit(EXIT_FAILURE);
	}
	process_mmu_queue_id = msgget(process_mmu_queue_key, 0666 | IPC_CREAT| IPC_EXCL);
	if(process_mmu_queue_id == -1)
	{
		perror("process_mmu_queue_message_get");
		cleanup_and_exit(EXIT_FAILURE);
	} 
}

void create_process_control_blocks()
{
	int process_index;
	pcb_shared_memory_key = ftok("master.c", 500);
	if(pcb_shared_memory_key == -1)
	{	
		perror("pcb_shared_memory_key");
		cleanup_and_exit(EXIT_FAILURE);
	}
	pcb_shared_memory_id = shmget(pcb_shared_memory_key, sizeof(process_control_block) * num_processes, 0666 | IPC_CREAT | IPC_EXCL);
	if(pcb_shared_memory_id == -1)
	{	
		perror("pcb_shared_memory_get");
		cleanup_and_exit(EXIT_FAILURE);
	}

	process_control_block *pcb_pointer = (process_control_block*)(shmat(pcb_shared_memory_id, NULL, 0));
	if(*(int *)pcb_pointer == -1)
	{
		perror("pcb_shared_memory_attach");
		cleanup_and_exit(EXIT_FAILURE);
	}

	int total_pages_needed = 0;
	for(process_index = 0; process_index < num_processes; process_index++)
	{
		pcb_pointer[process_index].process_id = process_index;
		pcb_pointer[process_index].num_pages = rand() % pages_per_process + 1;
		pcb_pointer[process_index].frames_allocated = 0;
		total_pages_needed += pcb_pointer[process_index].num_pages;
	}
	int allocated_frames_count = 0;
	printf("total_pages = %d, num_processes = %d, total_frames = %d\n", total_pages_needed, num_processes, total_frames);
	int max_pages = 0, max_pages_process_index = 0;
	for(process_index = 0; process_index < num_processes; process_index++)
	{
		pcb_pointer[process_index].process_id = -1;
		int frames_to_allocate = (int)round(pcb_pointer[process_index].num_pages * (total_frames - num_processes) / (float)total_pages_needed) + 1;
		if(pcb_pointer[process_index].num_pages > max_pages)
		{
			max_pages = pcb_pointer[process_index].num_pages;
			max_pages_process_index = process_index;
		}
		allocated_frames_count = allocated_frames_count + frames_to_allocate;
		pcb_pointer[process_index].max_frames_allowed = frames_to_allocate;
	}
	pcb_pointer[max_pages_process_index].max_frames_allowed += total_frames - allocated_frames_count; 

	for(process_index = 0; process_index < num_processes; process_index++)
	{
		print_process_control_block(pcb_pointer[process_index]);
	}

	if(shmdt(pcb_pointer) == -1)
	{
		perror("pcb_shared_memory_detach");
		cleanup_and_exit(EXIT_FAILURE);
	}
}

void clear_system_resources()
{
	if(shmctl(page_table_shared_id, IPC_RMID, NULL) == -1)
	{
		perror("shared_memory_control_page_table");
	}
	if(shmctl(frame_list_shared_id, IPC_RMID, NULL) == -1)
	{
		perror("shared_memory_control_frame_list");
	}
	if(shmctl(pcb_shared_memory_id, IPC_RMID, NULL) == -1)
	{
		perror("shared_memory_control_pcb");
	}
	if(msgctl(ready_queue_id, IPC_RMID, NULL) == -1)
	{
		perror("message_control_ready_queue");
	}
	if(msgctl(scheduler_mmu_queue_id, IPC_RMID, NULL) == -1)
	{
		perror("message_control_scheduler_mmu_queue");
	}
	if(msgctl(process_mmu_queue_id, IPC_RMID, NULL) == -1)
	{
		perror("message_control_process_mmu_queue");
	}
}

void cleanup_and_exit(int exit_status)
{
	clear_system_resources();
	exit(exit_status);
}

void create_user_processes()
{
	process_control_block *pcb_pointer = (process_control_block*)(shmat(pcb_shared_memory_id, NULL, 0));

	int process_index, reference_index;
	for(process_index = 0; process_index < num_processes; process_index++)
	{
		int reference_string_length = rand() % (8 * pcb_pointer[process_index].num_pages) + 2 * pcb_pointer[process_index].num_pages + 1;
		char reference_string[pages_per_process * 20 * 40];
		printf("reference_string_length = %d\n", reference_string_length);
		int string_position = 0;
		for(reference_index = 0; reference_index < reference_string_length; reference_index++)
		{
			int random_page;
			random_page = rand() % pcb_pointer[process_index].num_pages;
			float probability = (rand() % 100) / 100.0;
			if(probability < 0.2)
			{
				random_page = rand() % (1000 * pages_per_process) + pcb_pointer[process_index].num_pages;
			}
			string_position += sprintf(reference_string + string_position, "%d|", random_page);
		}
		printf("Reference string = %s\n", reference_string);
		if(fork() == 0)
		{
			char process_id_buffer[20], ready_queue_key_buffer[20], process_mmu_queue_key_buffer[20];
			sprintf(process_id_buffer, "%d", process_index);
			sprintf(ready_queue_key_buffer, "%d", ready_queue_key);
			sprintf(process_mmu_queue_key_buffer, "%d", process_mmu_queue_key);
			execlp("./process", "./process", process_id_buffer, ready_queue_key_buffer, process_mmu_queue_key_buffer, reference_string, (char *)(NULL));
			exit(0);
		}
		usleep(1 * 1000);	
	}
}

int master_process_id, scheduler_process_id, mmu_process_id;

void handle_termination_signal(int signal_number)
{
	sleep(1);
	kill(scheduler_process_id, SIGTERM);
	kill(mmu_process_id, SIGUSR2);
	sleep(2);
	shutdown_flag = 1;
}

int main(int argc, char const *argv[])
{
	srand(time(NULL));
	signal(SIGUSR1, handle_termination_signal);
	signal(SIGINT, cleanup_and_exit);
	if(argc < 4)
	{
		printf("master num_processes pages_per_process total_frames\n");
		cleanup_and_exit(EXIT_FAILURE);
	}
	num_processes = atoi(argv[1]);
	pages_per_process = atoi(argv[2]);
	total_frames = atoi(argv[3]);
	master_process_id = getpid();
	if(num_processes <= 0 || pages_per_process <= 0 || total_frames <= 0 || total_frames < num_processes)
	{
		printf("Invalid input\n");
		cleanup_and_exit(EXIT_FAILURE);
	}

	create_page_tables();
	create_frame_free_list();
	create_process_control_blocks();
	create_message_queues();

	if((scheduler_process_id = fork()) == 0)
	{
		char ready_queue_key_buffer[20], scheduler_mmu_queue_key_buffer[20], num_processes_buffer[20], master_process_id_buffer[20];
		sprintf(ready_queue_key_buffer, "%d", ready_queue_key);
		sprintf(scheduler_mmu_queue_key_buffer, "%d", scheduler_mmu_queue_key);
		sprintf(num_processes_buffer, "%d", num_processes);
		sprintf(master_process_id_buffer, "%d", master_process_id);
		execlp("./scheduler", "./scheduler", ready_queue_key_buffer, scheduler_mmu_queue_key_buffer, num_processes_buffer, master_process_id_buffer, (char *)(NULL));
		exit(0);
	}

	if((mmu_process_id = fork()) == 0)
	{
		char scheduler_mmu_queue_id_buffer[20], process_mmu_queue_id_buffer[20], page_table_shared_id_buffer[20], frame_list_shared_id_buffer[20], pcb_shared_memory_id_buffer[20], pages_per_process_buffer[20], num_processes_buffer[20];
		sprintf(scheduler_mmu_queue_id_buffer, "%d", scheduler_mmu_queue_id);
		sprintf(process_mmu_queue_id_buffer, "%d", process_mmu_queue_id);
		sprintf(page_table_shared_id_buffer, "%d", page_table_shared_id);
		sprintf(frame_list_shared_id_buffer, "%d", frame_list_shared_id);
		sprintf(pcb_shared_memory_id_buffer, "%d", pcb_shared_memory_id);
		sprintf(pages_per_process_buffer, "%d", pages_per_process);
		sprintf(num_processes_buffer, "%d", num_processes);
		execlp("./mmu", "./mmu", scheduler_mmu_queue_id_buffer, process_mmu_queue_id_buffer, page_table_shared_id_buffer, frame_list_shared_id_buffer, pcb_shared_memory_id_buffer, pages_per_process_buffer, num_processes_buffer, (char *)(NULL));
		exit(0);
	}
	printf("generating processes\n");
	create_user_processes();
	if(shutdown_flag == 0)
		pause();
	clear_system_resources();
>>>>>>> 92241c1 (Updated implementations)

	return 0;
}