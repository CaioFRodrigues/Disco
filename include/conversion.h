#ifndef __CONVERSIONH__
#define __CONVERSIONH__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


unsigned int swap_endianess(unsigned char *str);
unsigned char conv_hex_to_string(unsigned char *hex);


unsigned int swap_local_endianess(unsigned char *full_str, int start);
unsigned int swap_local_2bit_endianess(unsigned char *full_str, int start);
unsigned int swap_4BYTE_endianess(unsigned int hex);


#endif