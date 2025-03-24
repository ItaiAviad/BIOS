#include <sched.h>

uint64_t last_context_switch_time = 0;

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

void handle_sched_on_pit_tick(registers* registers){
    if(!pcb_list || (pit_time_ms - last_context_switch_time) < PROCESS_RUN_TIME){
        return;
    }
    if(current_pcb){
        to_cpu_state_from_syscall(&(current_pcb->cpu_context), registers);
    }
}
