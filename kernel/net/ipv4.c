#include <net/ipv4.h>

void print_ipv4(char ipv4[IPV4_ADDR_SIZE]) {
    printf("%x.%x.%x.%x", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
}