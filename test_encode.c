#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"

int main(int argc, char **argv)
{
    if (check_operation_type(argv) == e_encode)
    {
        printf("User selected encoding.\n");
        EncodeInfo encoding;
        if (read_and_validate_encode_args(argv, &encoding) == e_success)
        {
            printf("Read and validate encode arguments is a success\n");
            printf("Started encoding..........\n");
            if (do_encoding(&encoding) == e_success)
            {
                printf("Encoding completed successfully\n");
            }
            else
            {
                printf("Failed to perform encoding\n");
                return -1;
            }
        }
        else
        {
            printf("Read and validate encode arguments is a failure\n");
            return 1;
        }
    }
    else if (check_operation_type(argv) == e_decode)
    {
        printf("User selected decoding.\n");
        DecodeInfo decoding;
        if (read_and_validate_decode_args(argv, &decoding) == e_success)
        {
            printf("Read and validate decode arguments is a success\n");
            printf("Started Decoding..........\n");
            if (do_decoding(&decoding) == e_success)
            {
                printf("Decoding completed successfully\n");
            }
            else
            {
                printf("Failed to perform decoding\n");
                return -1;
            }
        }
        else
        {
            printf("Read and validate decode arguments is a failure\n");
            return 1;
        }
    }
    else
    {
        printf("Invalid Input\n");
        printf("For encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]\n");
        printf("For decoding : ./a.out -d stego.bmp [decode.txt]\n");
    }
    return 0;
}

OperationType check_operation_type(char **argv)
{
    if (strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}