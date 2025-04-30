//
// Created by fogoz on 29/04/2025.
//

#ifndef UTILS_H
#define UTILS_H
#include "Arduino.h"


// Define if missing
inline uint16_t htons(uint16_t x) {
    return (x << 8) | (x >> 8);
}

inline uint32_t htonl(uint32_t x) {
    return ((x << 24) & 0xFF000000) |
           ((x << 8)  & 0x00FF0000) |
           ((x >> 8)  & 0x0000FF00) |
           ((x >> 24) & 0x000000FF);
}

// ntohs and ntohl are same as htons and htonl respectively
#define ntohs(x) htons(x)
#define ntohl(x) htonl(x)


#endif //UTILS_H
