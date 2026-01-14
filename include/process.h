#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_PROCESSES 16
#define MAX_PROCESS_NAME 32
#define PROCESS_STACK_SIZE 4096

typedef enum {
    PROCESS_CREATED = 0,
    PROCESS_READY = 1,
    PROCESS_RUNNING = 2,
    PROCESS_WAITING = 3,
    PROCESS_TERMINATED = 4
} ProcessState;

typedef enum {
    PRIORITY_LOW = 0,
    PRIORITY_NORMAL = 1,
    PRIORITY_HIGH = 2,
    PRIORITY_CRITICAL = 3
} ProcessPriority;

typedef void (*process_entry_t)(void);

typedef struct {
    int pid;
    char name[MAX_PROCESS_NAME];
    ProcessState state;
    ProcessPriority priority;
    uint32_t stack_ptr;
    process_entry_t entry_point;
    uint32_t time_slice;
    uint32_t time_used;
    uint32_t creation_time;
    uint32_t parent_pid;
    int exit_code;
    bool in_use;
} Process;

typedef struct {
    Process processes[MAX_PROCESSES];
    int current_process_index;
    int process_count;
    int next_pid;
    uint32_t total_ticks;
} ProcessManager;

void process_init();
int process_create(const char* name, process_entry_t entry_point, ProcessPriority priority);
int process_destroy(int pid);
int process_get_current_pid();
Process* process_get_current();
Process* process_get_by_pid(int pid);
void process_schedule();
void process_switch_context();
void process_list();
void process_info(int pid);
void process_set_state(int pid, ProcessState state);
void process_terminate_current(int exit_code);
void process_yield();
void process_update_tick();
int process_kill(int pid);

extern ProcessManager g_process_manager;

#endif
