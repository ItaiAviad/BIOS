#include <net/ipv4.h>

void print_ipv4(char ipv4[IPV4_ADDR_SIZE]) {
    char *ip = (char *)ipv4;
    printf("%x.%x.%x.%x", ip[0], ip[1], ip[2], ip[3]);
}