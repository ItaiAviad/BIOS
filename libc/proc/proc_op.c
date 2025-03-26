#include <proc.h>
#include <sys/syscall.h>
#include <process.h>
#include <arch/x86_64/mmu.h>

int exec(char* path){
    #if defined(__is_libk)

        registers* regs_ptr;
        
        asm volatile (
            "mov %[out], [rsp + 8]"  // regs is the 7th argument, stored at RSP + 56
            : [out] "=r" (regs_ptr)   // Output operand
        );

        char* path_copy = malloc(strlen(path)+1);
        memcpy(path_copy, path, strlen(path)+1);

        PCB* pcb = alloc_proc();
        load_proc_mem(pcb, path_copy);
        free(path_copy);
        // if(current_pcb){
        //     pcb->ppid = current_pcb->pid;
        //     pcb->cpu_context.rip += 8;
        //     memcpy(regs_ptr, &(pcb->cpu_context), sizeof(cpu_state));
        //     current_pcb = pcb;
        //     map_memory_range(&kpcb, (void*)pcb->ctx.pml4, ((void*)pcb->ctx.pml4 + PAGE_SIZE - 1), (void*)pcb->ctx.pml4);
        //     map_memory_range(&kpcb, PROC_BIN_ADDR-PROC_STACK_SIZE, PROC_BIN_ADDR-PROC_STACK_SIZE+PROC_MEM_SIZE-1, pcb->real_mem_addr);
        // }else{
        //     pcb->ppid = kpcb.pid;
        //     run_proc(pcb);
        // }
    #else
        return syscall(sys_exec, path);
    #endif
}

int exit(){
    #if defined(__is_libk)
        deallocate_proc_mem(current_pcb->real_mem_addr);
        unmap_memory_range(&kpcb, PROC_BIN_ADDR-PROC_STACK_SIZE, PROC_BIN_ADDR-PROC_STACK_SIZE+PROC_MEM_SIZE-1, false);
        unmap_page(current_pcb->ctx.pml4);

        int parent_pid = current_pcb->ppid;
        free(current_pcb);
        run_proc(find_pcb_by_pid(parent_pid));
        
    #else
        return syscall(sys_exit);
    #endif
}