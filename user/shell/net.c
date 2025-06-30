// Network Shell Commands
#include <shell.h>

void shcmd_nslookup(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <name server>\n", argv[0]);
        return;
    }

    send_dns_request(argv[1]);
}
