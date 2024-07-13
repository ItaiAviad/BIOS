//  Main Kernel File

//  kernel's main function
//  extern "C" is required inorder for the linker to find the kernel_main
//  function address
extern "C" void kernel_main(void) {
  int i = 1;
  while (true) {
    i++;
  }
}
