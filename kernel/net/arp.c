#include <net/arp.h>
#include <net/rtl8139.h>

#define ARP_CACHE_SIZE 32

struct arp_entry arp_cache[ARP_CACHE_SIZE];

// Update `cache_arp_entry` to store the timestamp
void cache_arp_entry(unsigned char ip[IPV4_ADDR_SIZE], unsigned char mac[MAC_ADDR_SIZE]) {
    uint64_t now = time(); // Function to get the current time in milliseconds
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (!memcmp(arp_cache[i].ip, ip, IPV4_ADDR_SIZE) || arp_cache[i].ip[0] == 0) {
            memcpy(arp_cache[i].ip, ip, IPV4_ADDR_SIZE);
            memcpy(arp_cache[i].mac, mac, MAC_ADDR_SIZE);
            arp_cache[i].timestamp = now;
            return;
        }
    }
    printf("ARP cache full, unable to store entry for IP ");
    print_ipv4(ip);
    printf("\n");
}

// Periodically clean up expired entries
void cleanup_arp_cache() {
    uint64_t now = time();
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (arp_cache[i].ip[0] != 0 && now - arp_cache[i].timestamp > ARP_CACHE_TTL) {
            memset(&arp_cache[i], 0, sizeof(struct arp_entry)); // Clear entry
        }
    }
}

// Function to call periodically (e.g., in a timer or main loop)
void arp_cache_maintenance() {
    static uint64_t last_cleanup = 0;
    uint64_t now = time();
    if (now - last_cleanup > ARP_CACHE_CLEANUP_INTERVAL) {
        cleanup_arp_cache();
        last_cleanup = now;
    }
}

unsigned char *wait_for_arp_entry(unsigned char ip[IPV4_ADDR_SIZE]) {
    __asm__ volatile("sti");

    int elapsed_time = 0;
    while (elapsed_time < ARP_WAIT_TIMEOUT) {
        unsigned char *mac = find_mac_in_cache(ip);
        if (mac != NULL) {
            printf("IPV4 in wait: %d, %u\n", ip[3], ip[3]);
            printf("INPUT IP: ");
            print_ipv4(ip);
            printf("  ");
            printf("GOT MAC: %p  ", mac);
            print_mac(mac);
            printf("\n\n");
            return mac; // Entry found
        }

        // Sleep for a short duration to avoid busy-waiting
        // sleep(10); // Sleep for 10 ms (implement `sleep` for your environment)
        // elapsed_time += 10;
    }
    return NULL; // Timeout, entry not found
}

unsigned char *find_mac_in_cache(unsigned char ip[IPV4_ADDR_SIZE]) {
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (memcmp(arp_cache[i].ip, ip, IPV4_ADDR_SIZE) == 0) {
            return arp_cache[i].mac;
        }
    }
    return NULL;
}

void send_arp(uint16_t oper, unsigned char sha[MAC_ADDR_SIZE], unsigned char spa[IPV4_ADDR_SIZE], unsigned char tha[MAC_ADDR_SIZE], unsigned char tpa[IPV4_ADDR_SIZE]) {
    void *packet = malloc(PACKET_MAX_SIZE);
    int packet_len = 0;

    struct arp_header arp = {
        .htype = htobe16(ARP_HTYPE_ETH),
        .ptype = htobe16(PTYPE_IPV4),

        .hlen = ARP_HLEN_ETH,
        .plen = ARP_PLEN_IPV4,
        .oper = htobe16(oper),
    };

    // Addresses
    memcpy(arp.sha, sha, MAC_ADDR_SIZE);
    memcpy(arp.spa, spa, IPV4_ADDR_SIZE);
    memcpy(arp.tha, tha, MAC_ADDR_SIZE);
    memcpy(arp.tpa, tpa, IPV4_ADDR_SIZE);

    packet_len += sizeof(struct arp_header);
    memcpy(packet, &arp, packet_len);

    // Ethernet encapsulation
    unsigned char eth_broadcast[MAC_ADDR_SIZE] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    if (oper == ARP_OPER_REQUEST) // broadcast
        encapsulate_ethernet(packet, &packet_len, eth_broadcast, sha, PTYPE_ARP);
    else if (oper == ARP_OPER_REPLY)
        encapsulate_ethernet(packet, &packet_len, tha, sha, PTYPE_ARP);

    send_packet(packet, packet_len);

    free(packet);
}

unsigned char *get_mac(unsigned char ip[IPV4_ADDR_SIZE]) {
    // Send ARP Request
    unsigned char sha[MAC_ADDR_SIZE];
    memcpy(sha, g_nic.mac, MAC_ADDR_SIZE);
    unsigned char tha[MAC_ADDR_SIZE] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    send_arp(ARP_OPER_REQUEST, sha, g_nic.ipv4, tha, ip);

    // Wait for ARP Reply
    return wait_for_arp_entry(ip);
}
