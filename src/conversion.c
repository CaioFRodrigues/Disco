
#include "../include/conversion.h"
#include <math.h>
// str must be hexadecimal string like "5432" for ex
unsigned int swap_endianess(unsigned char *str)
{
    if(strlen((char *) str) == 4){
        unsigned int num = (unsigned int)strtol((char *)str, NULL, 16);

        unsigned int swapped = ((num>>24)&0xff) | // move byte 3 to byte 0
                               ((num<<8)&0xff0000) | // move byte 1 to byte 2
                               ((num>>8)&0xff00) | // move byte 2 to byte 1
                               ((num<<24)&0xff000000); // byte 0 to byte 3
        swapped = swapped>>16;

//        printf("%ld\n", sizeof(swapped));


        return swapped;
    }
    else
        return 0;

}

// hex must be a simple 2 byte char like "54" for ex
unsigned char conv_hex_to_string(unsigned char *hex)
{
    if(strlen((char *) hex) == 2){
        unsigned int c = (unsigned int)strtol((char *)hex, NULL, 16);

        return c;
    }
    else
        return 0;
}

// Given a buffer string and an start, read 4 bytes as little endian and returns the value as an unsigned int
unsigned int swap_local_endianess(unsigned char *full_str, int start)
{
    
    //String with the 4 bytes to be swapped
    unsigned int big_endian_format = 0;
    //Adds each byte with the desired value
    for (int i = 0; i < 4; i++){
        big_endian_format += ((unsigned int)pow(16,i*2))  * (unsigned int) full_str[start + i];
    }  


    return big_endian_format;
}

// Given a buffer string and an start, read 2 bytes as little endian and returns the value as an unsigned int
unsigned int swap_local_2bit_endianess(unsigned char *full_str, int start)
{
    
    //String with the 4 bytes to be swapped
    unsigned int big_endian_format = 0;
    //Adds each byte with the desired value
    for (int i = 0; i < 2; i++)
        big_endian_format += ((unsigned int)pow(16,i*2))  * (unsigned int) full_str[start + i];
      


    return big_endian_format;
}