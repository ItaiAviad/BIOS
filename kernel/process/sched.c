#include <sched.h>

uint64_t last_context_switch_time = 0;
PCB* pcb_to_run = NULL;
int skipped = false;

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
    cpu_state->cs = syscall_regs->cs;
    cpu_state->ss = syscall_regs->ss;
}

void run_next_proc(){
    // printf("Did sched! ");
    run_proc(pcb_to_run);
}

void handle_sched_on_pit_tick(registers* registers, uint64_t cr3){
    if(!pcb_list || ((pit_time_ms - last_context_switch_time) < PROCESS_RUN_TIME)){
        skipped = true;
        return;
    }
    last_context_switch_time = pit_time_ms;
    if(current_pcb){
        to_cpu_state_from_syscall(&(current_pcb->cpu_context), registers);
        current_pcb->ctx.pml4 = (void*)cr3;
    }

    pcb_to_run = NULL;

    if(!current_pcb || !(current_pcb->list_node) || !(current_pcb->list_node->next)){
        pcb_to_run = pcb_list->data;
    }else{
        pcb_to_run = current_pcb->list_node->next->data;
        while (pcb_to_run)
        {
            if(pcb_to_run->state == READY){
                break;
            }
            pcb_to_run = pcb_to_run->list_node->next->data;
        }
        if(!pcb_to_run){
            pcb_to_run = pcb_list->data;
            while (pcb_to_run)
            {
                if(pcb_to_run->state == READY){
                    break;
                }
                pcb_to_run = pcb_to_run->list_node->next->data;
            }
        }
        
    }

    if(!pcb_to_run || !(pcb_to_run->state == READY)){
        return;
    }

    if(pcb_to_run->pid != 0  && pcb_to_run->pid != KERNEL_PID){
        // printf("Ordered sched! ");
        // printf("cs: %x\n", registers->cs);
        // printf("ss: %x\n", registers->ss);
        registers->cs = KERNEL_CS_SELECTOR_OFFSET_GDT;
        registers->ss = KERNEL_SS_SELECTOR_OFFSET_GDT;
        
        skipped = false;

        registers->rip = run_next_proc;
        registers->eflags = 0x000;
    }
}
