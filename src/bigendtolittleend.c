#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// str must be hexadecimal string like "5432" for ex
unsigned int swap_endianess(unsigned char *str)
{
    if(strlen(str) == 4){
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
    if(strlen(hex) == 2){
        unsigned int c = (unsigned int)strtol((char *)hex, NULL, 16);

        return c;
    }
    else
        return 0;
}

//
//
//int main()
//{
//    unsigned char c[] = "5432";
//
//    unsigned char a[] = "54";
//
////    unsigned char esse = conv_hex_to_string(a);
//
//    printf("%c\n", conv_hex_to_string(a));
//
//
//    unsigned int num = (unsigned int)strtol((char *)c, NULL, 16);
//
//    printf("%02X\n", num);
//
////    unsigned int swapped = ((num>>24)&0xff) | // move byte 3 to byte 0
////                    ((num<<8)&0xff0000) | // move byte 1 to byte 2
////                    ((num>>8)&0xff00) | // move byte 2 to byte 1
////                    ((num<<24)&0xff000000); // byte 0 to byte 3
////    swapped = swapped>>16;
////
//
////    unsigned int swapped = (num>>8) | (num<<8);
//
////    printf("%ld", sizeof(c));
//    unsigned int swapped = swap_endianess(c);
//
//    unsigned int desswap = ((swapped>>24)&0xff) | // move byte 3 to byte 0
//                ((swapped<<8)&0xff0000) | // move byte 1 to byte 2
//                ((swapped>>8)&0xff00) | // move byte 2 to byte 1
//                ((swapped<<24)&0xff000000); // byte 0 to byte 3
//    desswap = desswap>>16;
//
//
//    printf("%X\n", swapped);
//    printf("%X", desswap);
//
//    return 0;
//}
