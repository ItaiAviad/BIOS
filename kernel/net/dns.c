#include <net/dns.h>

unsigned char *send_dns(char *name) {
    if (name == NULL)
        printf("DNS: name server\n");

    printf("ARP REQ: \n");
    unsigned char ipv4[IPV4_ADDR_SIZE] = {10, 0, 0, 138};
    unsigned char *mac = get_mac(ipv4);
    if (mac != NULL)
        print_mac(mac);
    else
        printf("MAC not found");
    printf("\n");

    return mac;
}
