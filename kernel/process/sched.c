#include <sched.h>

uint64_t last_context_switch_time = 0;
PCB* pcb_to_run = NULL;
int can_sched = false;

void to_cpu_state_from_syscall(cpu_state* cpu_state, registers* syscall_regs) {
    // Save the registers from the syscall into the cpu_state structure
    cpu_state->rdi = syscall_regs->rdi;
    cpu_state->rsi = syscall_regs->rsi;
    cpu_state->rdx = syscall_regs->rdx;
    cpu_state->rcx = syscall_regs->rcx;
    cpu_state->rbx = syscall_regs->rbx;
    cpu_state->rax = syscall_regs->rax;

    // Optionally save the other registers (callee-saved registers)
    cpu_state->r15 = syscall_regs->r15;
    cpu_state->r14 = syscall_regs->r14;
    cpu_state->r13 = syscall_regs->r13;
    cpu_state->r12 = syscall_regs->r12;
    cpu_state->rbp = syscall_regs->rbp;

    // Save other state information, such as the return address (rip), stack pointer (rsp), etc.
    cpu_state->rip = syscall_regs->rip;  // Return address
    cpu_state->rsp = syscall_regs->rsp;  // Stack pointer
    cpu_state->eflags = syscall_regs->eflags;
}

void run_next_proc(){
    run_proc(pcb_to_run);
}

void handle_sched_on_pit_tick(registers* registers){
    if(!can_sched){
        return;
    }
    if(!pcb_list || (pit_time_ms - last_context_switch_time) < PROCESS_RUN_TIME || pit_time_ms == 0){
        return;
    }
    last_context_switch_time = pit_time_ms;
    // if(registers->rip >= KERNEL_VBASE && KERNEL_VBASE + KERNEL_END - KERNEL_LOAD_ADDR){
    //     return;
    // }
    if(current_pcb && !(registers->rip >= KERNEL_VBASE && KERNEL_VBASE + KERNEL_END - KERNEL_LOAD_ADDR)){
        to_cpu_state_from_syscall(&(current_pcb->cpu_context), registers);
    }

    pcb_to_run = NULL;

    if(!current_pcb || !(current_pcb->list_node) || !(current_pcb->list_node->next)){
        pcb_to_run = pcb_list->data;
    }else{
        pcb_to_run = current_pcb->list_node->next->data;
    }

    if(pcb_to_run->pid != 0  && pcb_to_run->pid != KERNEL_PID){
        registers->rip = run_next_proc;
    }
}
