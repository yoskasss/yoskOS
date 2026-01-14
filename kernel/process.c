#include "../include/process.h"
#include "../include/console.h"
#include "../include/string.h"

ProcessManager g_process_manager = {0};

void process_init() {
    g_process_manager.current_process_index = 0;
    g_process_manager.process_count = 0;
    g_process_manager.next_pid = 1;
    g_process_manager.total_ticks = 0;
    

    for (int i = 0; i < MAX_PROCESSES; i++) {
        g_process_manager.processes[i].in_use = false;
        g_process_manager.processes[i].pid = -1;
        g_process_manager.processes[i].state = PROCESS_CREATED;
    }
    
    print_color("[PROCESS] Process management system initialized\n", 0x0A);
}

int process_create(const char* name, process_entry_t entry_point, ProcessPriority priority) {

    int slot = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (!g_process_manager.processes[i].in_use) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        print_color("[PROCESS] ERROR: No free process slots available\n", 0x04);
        return -1;
    }
    

    Process* proc = &g_process_manager.processes[slot];
    proc->pid = g_process_manager.next_pid++;
    strncpy(proc->name, name, MAX_PROCESS_NAME - 1);
    proc->name[MAX_PROCESS_NAME - 1] = '\0';
    proc->state = PROCESS_READY;
    proc->priority = priority;
    proc->stack_ptr = 0;
    proc->entry_point = entry_point;
    proc->time_slice = 10;
    proc->time_used = 0;
    proc->creation_time = g_process_manager.total_ticks;
    proc->parent_pid = (g_process_manager.current_process_index >= 0) ? 
                       g_process_manager.processes[g_process_manager.current_process_index].pid : 0;
    proc->exit_code = 0;
    proc->in_use = true;
    
    g_process_manager.process_count++;
    
    print_color("[PROCESS] Created process '", 0x0A);
    print_color(name, 0x0E);
    print_color("' with PID: ", 0x0A);
    print_int(proc->pid);
    print_color(" (Priority: ", 0x0A);
    print_int(priority);
    print_color(")\n", 0x0A);
    
    return proc->pid;
}

int process_destroy(int pid) {
    Process* proc = process_get_by_pid(pid);
    
    if (!proc) {
        print_color("[PROCESS] ERROR: Process with PID ", 0x04);
        print_int(pid);
        print_color(" not found\n", 0x04);
        return -1;
    }
    

    if (proc->state == PROCESS_RUNNING) {
        print_color("[PROCESS] WARNING: Cannot destroy running process\n", 0x0E);
        proc->state = PROCESS_TERMINATED;
    } else {
        proc->in_use = false;
        g_process_manager.process_count--;
    }
    
    print_color("[PROCESS] Destroyed process '", 0x0A);
    print_color(proc->name, 0x0E);
    print_color("' (PID: ", 0x0A);
    print_int(pid);
    print_color(")\n", 0x0A);
    
    return 0;
}

int process_get_current_pid() {
    if (g_process_manager.current_process_index >= 0 && 
        g_process_manager.current_process_index < MAX_PROCESSES) {
        Process* proc = &g_process_manager.processes[g_process_manager.current_process_index];
        if (proc->in_use) {
            return proc->pid;
        }
    }
    return -1;
}

Process* process_get_current() {
    if (g_process_manager.current_process_index >= 0 && 
        g_process_manager.current_process_index < MAX_PROCESSES) {
        Process* proc = &g_process_manager.processes[g_process_manager.current_process_index];
        if (proc->in_use) {
            return proc;
        }
    }
    return NULL;
}

Process* process_get_by_pid(int pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (g_process_manager.processes[i].in_use && 
            g_process_manager.processes[i].pid == pid) {
            return &g_process_manager.processes[i];
        }
    }
    return NULL;
}

void process_schedule() {
    if (g_process_manager.process_count == 0) {
        return;
    }
    

    int current = g_process_manager.current_process_index;
    int next = current;
    

    if (current >= 0 && current < MAX_PROCESSES) {
        Process* proc = &g_process_manager.processes[current];
        if (proc->in_use && proc->state == PROCESS_RUNNING) {
            proc->time_used++;
            

            if (proc->time_used >= proc->time_slice) {
                proc->state = PROCESS_READY;
                proc->time_used = 0;
            }
        }
    }
    

    int found = false;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        int idx = (current + 1 + i) % MAX_PROCESSES;
        Process* proc = &g_process_manager.processes[idx];
        
        if (proc->in_use && proc->state == PROCESS_READY) {
            next = idx;
            found = true;
            break;
        }
    }
    

    if (!found && current >= 0 && current < MAX_PROCESSES) {
        Process* proc = &g_process_manager.processes[current];
        if (proc->in_use && proc->state != PROCESS_TERMINATED) {
            return;
        }
    }
    

    g_process_manager.current_process_index = next;
    Process* next_proc = &g_process_manager.processes[next];
    next_proc->state = PROCESS_RUNNING;
}

void process_switch_context() {
    process_schedule();

}

void process_list() {
    print_color("\n=== PROCESS LIST ===\n", 0x0E);
    print_color("PID\tName\t\tState\t\tPriority\tTime Used\n", 0x0B);
    print_color("---\t----\t\t-----\t\t--------\t---------\n", 0x0B);
    
    int listed = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        Process* proc = &g_process_manager.processes[i];
        if (proc->in_use) {
            print_int(proc->pid);
            print("\t");
            print(proc->name);
            print("\t");
            

            switch (proc->state) {
                case PROCESS_CREATED: print("CREATED"); break;
                case PROCESS_READY: print("READY"); break;
                case PROCESS_RUNNING: print("RUNNING"); break;
                case PROCESS_WAITING: print("WAITING"); break;
                case PROCESS_TERMINATED: print("TERMINATED"); break;
                default: print("UNKNOWN"); break;
            }
            print("\t");
            
            print_int(proc->priority);
            print("\t");
            print_int(proc->time_used);
            print("ms\n");
            
            listed++;
        }
    }
    
    if (listed == 0) {
        print_color("No processes found\n", 0x08);
    }
    
    print_color("\nTotal: ", 0x0E);
    print_int(g_process_manager.process_count);
    print_color(" processes\n\n", 0x0E);
}

void process_info(int pid) {
    Process* proc = process_get_by_pid(pid);
    
    if (!proc) {
        print_color("Process with PID ", 0x04);
        print_int(pid);
        print_color(" not found\n", 0x04);
        return;
    }
    
    print_color("\n=== PROCESS INFORMATION ===\n", 0x0E);
    print_color("PID: ", 0x0B);
    print_int(proc->pid);
    print("\n");
    
    print_color("Name: ", 0x0B);
    print(proc->name);
    print("\n");
    
    print_color("State: ", 0x0B);
    switch (proc->state) {
        case PROCESS_CREATED: print("CREATED"); break;
        case PROCESS_READY: print("READY"); break;
        case PROCESS_RUNNING: print("RUNNING"); break;
        case PROCESS_WAITING: print("WAITING"); break;
        case PROCESS_TERMINATED: print("TERMINATED"); break;
        default: print("UNKNOWN"); break;
    }
    print("\n");
    
    print_color("Priority: ", 0x0B);
    print_int(proc->priority);
    print("\n");
    
    print_color("Time Slice: ", 0x0B);
    print_int(proc->time_slice);
    print("ms\n");
    
    print_color("Time Used: ", 0x0B);
    print_int(proc->time_used);
    print("ms\n");
    
    print_color("Creation Time: ", 0x0B);
    print_int(proc->creation_time);
    print("\n");
    
    print_color("Parent PID: ", 0x0B);
    print_int(proc->parent_pid);
    print("\n");
    
    print_color("Exit Code: ", 0x0B);
    print_int(proc->exit_code);
    print("\n\n");
}

void process_set_state(int pid, ProcessState state) {
    Process* proc = process_get_by_pid(pid);
    if (proc) {
        proc->state = state;
    }
}

void process_terminate_current(int exit_code) {
    Process* proc = process_get_current();
    if (proc) {
        proc->exit_code = exit_code;
        proc->state = PROCESS_TERMINATED;
        print_color("[PROCESS] Process '", 0x0E);
        print(proc->name);
        print_color("' terminated with exit code: ", 0x0E);
        print_int(exit_code);
        print_color("\n", 0x0E);
        

        process_schedule();
    }
}

void process_yield() {
    Process* proc = process_get_current();
    if (proc) {
        proc->state = PROCESS_READY;
        proc->time_used = proc->time_slice;
        process_schedule();
    }
}

void process_update_tick() {
    g_process_manager.total_ticks++;
    

    Process* proc = process_get_current();
    if (proc && proc->state == PROCESS_RUNNING) {
        proc->time_used++;
        

        if (proc->time_used >= proc->time_slice) {
            proc->state = PROCESS_READY;
            proc->time_used = 0;
            process_schedule();
        }
    }
}

int process_kill(int pid) {
    Process* proc = process_get_by_pid(pid);
    
    if (!proc) {
        return -1;
    }
    
    if (proc->pid == 1) {
        return -2;
    }
    
    proc->state = PROCESS_TERMINATED;
    proc->in_use = false;
    g_process_manager.process_count--;
    
    print_color("Process '", 0x0A);
    print_color(proc->name, 0x0E);
    print_color("' (PID: ", 0x0A);
    print_int(pid);
    print_color(") has been killed\n", 0x0A);
    
    return 0;
}
