#pragma once

#include <stdint.h>


#define IPV4_ADDR_STR_BUFSIZ 16

// NOTE: We assume the out buffer has enough space to print an ipv4.
// The maximum space is:
//	4 x 3 chars = 12
//      3 x 1 dot   =  3
//      1 x nulterm =  1
//      ----------------
//                    16
//
void ipv4_to_string(uint32_t addr_be, char *out);

char *ipv4_proto_to_string(uint8_t proto);
