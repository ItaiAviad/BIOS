#include <sched.h>

uint64_t last_context_switch_time = 0;
PCB* pcb_to_run = NULL;
int skipped = false;
int sched_triggered = false;

void to_cpu_state_from_syscall(cpu_state* cpu_state, registers* syscall_regs) {
    cpu_state->rdi = syscall_regs->rdi;
    cpu_state->rsi = syscall_regs->rsi;
    cpu_state->rdx = syscall_regs->rdx;
    cpu_state->rcx = syscall_regs->rcx;
    cpu_state->rbx = syscall_regs->rbx;
    cpu_state->rax = syscall_regs->rax;

    cpu_state->r15 = syscall_regs->r15;
    cpu_state->r14 = syscall_regs->r14;
    cpu_state->r13 = syscall_regs->r13;
    cpu_state->r12 = syscall_regs->r12;
    cpu_state->r11 = syscall_regs->r11;
    cpu_state->r10 = syscall_regs->r10;
    cpu_state->r9 = syscall_regs->r9;
    cpu_state->r8 = syscall_regs->r8;
    cpu_state->rbp = syscall_regs->rbp;

    cpu_state->rip = syscall_regs->rip;
    cpu_state->rsp = syscall_regs->rsp;
    cpu_state->eflags = syscall_regs->eflags;
}

void run_next_proc(){
    cli();
    run_proc(pcb_to_run);
}

void handle_sched_on_pit_tick(registers* registers){
    // Don't schedule if interrupted from kernel mode (ring 0).
    // The saved CS RPL indicates which ring was interrupted.
    if((registers->cs & 0x3) == 0){
        return;
    }
    if(!pcb_list || ((pit_time_ms - last_context_switch_time) < PROCESS_RUN_TIME && !skipped)){
        return;
    }
    last_context_switch_time = pit_time_ms;
    if(current_pcb){
        to_cpu_state_from_syscall(&(current_pcb->cpu_context), registers);
    }

    pcb_to_run = NULL;

    if(!current_pcb || !(current_pcb->list_node) || !(current_pcb->list_node->next)){
        pcb_to_run = pcb_list->data;
    }else{
        pcb_to_run = current_pcb->list_node->next->data;
    }

    if(pcb_to_run->pid != 0  && pcb_to_run->pid != KERNEL_PID && pcb_to_run->state == READY){
        registers->rip = (uint64_t) run_next_proc;
        registers->cs = KERNEL_CS_SELECTOR_OFFSET_GDT;
        registers->ss = 0x10;
        registers->rsp = (uint64_t) kpcb.stack;
        registers->eflags &= ~(1 << 9);  // Clear IF to prevent re-entry before cli()
        sched_triggered = true;
        skipped = false;
    } else {
        sched_triggered = false;
        skipped = true;
    }
}
