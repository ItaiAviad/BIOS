#include <endian.h>

// Detect system endianness at runtime (portable)
static inline int is_little_endian() {
    uint32_t num = 1;
    return (*(uint8_t *)&num) == 1;
}

// Byte-swapping utility functions
static inline uint16_t swap16(uint16_t value) {
    return (value << 8) | (value >> 8);
}

static inline uint32_t swap32(uint32_t value) {
    return ((value << 24) & 0xFF000000) |
           ((value << 8)  & 0x00FF0000) |
           ((value >> 8)  & 0x0000FF00) |
           ((value >> 24) & 0x000000FF);
}

static inline uint64_t swap64(uint64_t value) {
    return ((value << 56) & 0xFF00000000000000ULL) |
           ((value << 40) & 0x00FF000000000000ULL) |
           ((value << 24) & 0x0000FF0000000000ULL) |
           ((value << 8)  & 0x000000FF00000000ULL) |
           ((value >> 8)  & 0x00000000FF000000ULL) |
           ((value >> 24) & 0x0000000000FF0000ULL) |
           ((value >> 40) & 0x000000000000FF00ULL) |
           ((value >> 56) & 0x00000000000000FFULL);
}

// Host to big-endian
uint16_t htobe16(uint16_t host_16bits) { return is_little_endian() ? swap16(host_16bits) : host_16bits; }
uint32_t htobe32(uint32_t host_32bits) { return is_little_endian() ? swap32(host_32bits) : host_32bits; }
uint64_t htobe64(uint64_t host_64bits) { return is_little_endian() ? swap64(host_64bits) : host_64bits; }

// Host to little-endian
uint16_t htole16(uint16_t host_16bits) { return is_little_endian() ? host_16bits : swap16(host_16bits); }
uint32_t htole32(uint32_t host_32bits) { return is_little_endian() ? host_32bits : swap32(host_32bits); }
uint64_t htole64(uint64_t host_64bits) { return is_little_endian() ? host_64bits : swap64(host_64bits); }

// Big-endian to host
uint16_t be16toh(uint16_t big_endian_16bits) { return htobe16(big_endian_16bits); }
uint32_t be32toh(uint32_t big_endian_32bits) { return htobe32(big_endian_32bits); }
uint64_t be64toh(uint64_t big_endian_64bits) { return htobe64(big_endian_64bits); }

// Little-endian to host
uint16_t le16toh(uint16_t little_endian_16bits) { return htole16(little_endian_16bits); }
uint32_t le32toh(uint32_t little_endian_32bits) { return htole32(little_endian_32bits); }
uint64_t le64toh(uint64_t little_endian_64bits) { return htole64(little_endian_64bits); }
