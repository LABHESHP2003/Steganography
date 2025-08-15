#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "types.h"
#include "decode.h"
#include "common.h"

/*
Decoding

./a.out -d stego.bmp [decoded.txt]

1. Open stego.bmp file
2. Move to 54th position in the file.
3. Decode the magic string: Read 8 bytes of stego.bmp, extract all the LSBs and frame a character -> 2 times U get 2 chars i.e #*
4. Decode the secret file extn size : Read 32 bytes of stego.bmp, extract all the LSBs and frame an int . U get int 4
5. Decode the secret file extn : Read 8 bytes of stego.bmp, extract all the LSBs and frame a character -> 4 times U get .txt create a file called output.txt
6. Decode secret file size : Read 32 bytes of stego.bmp, extract all the LSBs and frame an int . U get int 25.
7. Decode secret file data : Read 8 bytes of stego.bmp, extract all the LSBs and frame a character -> 25 times U get the contents of secret.txt These characters shoulld be written to output.txt/decoded.txt file

*/

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check the cla input if given file is .bmp
    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->src_image_fname = argv[2];
    }
    else
    {
        printf("[%s] Invalid source file format please select valid .bmp file\n", argv[2]);
        return e_failure;
    }

    // check if cla is passed for output file
    if (argv[3] != NULL)
    {
        if (strstr(argv[3], ".txt") != NULL)
        {
            decInfo->secret_fname = argv[3];
        }
        else
        {
            printf("[%s] Invalid file format please provide valid [.txt] file name\n", argv[3]);
            return e_failure;
        }
    }
    else
    {
        // if clas not passed the default file name
        decInfo->secret_fname = "output.txt";
    }

    return e_success;
}

Status open_files_decode(DecodeInfo *decInfo)
{
    // open image file in read mode
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");

    // Error Handling
    if ((decInfo->fptr_src_image) == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_image_fname);
        return e_failure;
    }

    // open ouput text file in write mode
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    if (decInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
        return e_failure;
    }
    return e_success;
}

Status header_skiper(FILE *fptr_src)
{
    // move pointer to skip 54 byte bmp header
    if (fseek(fptr_src, 54, SEEK_SET) == -1)
    {
        return e_failure;
    }
    // printf("FILE *pointer is at 55 position");
    return e_success;
}

char decode_byte_from_lsb(char *image_buffer)
{
    // extract LSB of each byte to get 1 char out of 8 bits
    char decoded_char = 0;
    for (int i = 0; i < 8; i++)
    {
        decoded_char = (decoded_char << 1) | (image_buffer[i] & 1);
    }
    return decoded_char;
}

Status decode_data_from_image(void *data, int size, DecodeInfo *decInfo)
{
    char image_buffer[8];
    char *data_char = (char *)data;
    for (int i = 0; i < size; i++)
    {
        // read 8 bytes to decode 1 char
        if (fread(image_buffer, 8, 1, decInfo->fptr_src_image) != 1)
        {
            fprintf(stderr, "Error reading from image file\n");
            return e_failure;
        }
        data_char[i] = decode_byte_from_lsb(image_buffer);
    }
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic_string[3];
    if (decode_data_from_image(magic_string, 2, decInfo) != e_success)
    {
        return e_failure;
    }
    magic_string[2] = '\0';
    if (strcmp(magic_string, MAGIC_STRING) == 0)
    {
        decInfo->decoded_magic_string = strdup(magic_string);
        printf("Magic string matched:%s\n", decInfo->decoded_magic_string);
        return e_success;
    }
    printf("Magic string did NOT match. Decoded: %s Expected: %s\n", magic_string, MAGIC_STRING);
    return e_failure;
}

Status decode_int_from_lsb(uint *value, DecodeInfo *decInfo)
{
    // decode 32 bits one by one and build the int
    char ch;
    uint result = 0;

    for (int i = 0; i < 32; i++)
    {
        if (fread(&ch, 1, 1, decInfo->fptr_src_image) != 1)
        {
            fprintf(stderr, "Error reading image for int decode\n");
            return e_failure;
        }

        result = (result << 1) | (ch & 1);
    }

    *value = result;
    return e_success;
}

Status decode_size_of_extension(DecodeInfo *decInfo)
{
    // get extension length like 4 for ".txt"
    uint size_of_ext = 0;
    if (decode_int_from_lsb(&size_of_ext, decInfo) != e_success)
    {
        return e_failure;
    }
    printf("Decoded extension size: %u Bytes\n", size_of_ext);
    if (size_of_ext != 4)
    {
        printf("Invalid extension size decoded\n");
        return e_failure;
    }

    decInfo->file_ext_size = size_of_ext;
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    // decode extension string like ".txt"
    char file_extn[10];
    if (decode_data_from_image(file_extn, decInfo->file_ext_size, decInfo) != e_success)
    {
        return e_failure;
    }
    file_extn[decInfo->file_ext_size] = '\0';
    // check  if .txt or not
    if (strcmp(file_extn, ".txt") != 0)
    {
        printf("Incorrect file extension found i.e (%s)\n", file_extn);
        return e_failure;
    }
    printf("Correct file extension found i.e %s\n", file_extn);
    strcpy(decInfo->extn_secret_file, file_extn);
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    // get total number of bytes of secret file
    uint file_size;
    if (decode_int_from_lsb(&file_size, decInfo) != e_success)
    {
        return e_failure;
    }
    decInfo->size_secret_file = file_size;
    printf("File size is %u Bytes\n", decInfo->size_secret_file);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    // decode secret file data byte by byte
    char ch;

    for (uint i = 0; i < decInfo->size_secret_file; i++)
    {
        if (decode_data_from_image(&ch, 1, decInfo) == e_failure)
        {
            fprintf(stderr, "Error: Unable to decode byte from image\n");
            return e_failure;
        }

        // write decoded byte to output txt file
        if (fwrite(&ch, 1, 1, decInfo->fptr_secret) != 1)
        {
            fprintf(stderr, "Error: Unable to write to output file\n");
            return e_failure;
        }
    }

    fclose(decInfo->fptr_src_image);
    fclose(decInfo->fptr_secret);
    printf("Successfully extracted data and closed files\n");
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if (open_files_decode(decInfo) == e_success)
    {
        printf("Open file is a success\n");
    }
    else
    {
        printf("Error opening file\n");
        return e_failure;
    }

    if (header_skiper(decInfo->fptr_src_image) == e_success)
    {
        printf("Bypassed Header\n");
    }
    else
    {
        printf("Failed to skip header\n");
        return e_failure;
    }

    if (decode_magic_string(decInfo) == e_success)
    {
        printf("Magic String decoded successfully\n");
    }
    else
    {
        printf("Failed to decode magic string\n");
        return e_failure;
    }

    if (decode_size_of_extension(decInfo) == e_success)
    {
        printf("Size of file extension decoded successfully\n");
    }
    else
    {
        printf("Failed to decode size of file extension\n");
        return e_failure;
    }

    if (decode_secret_file_extn(decInfo) == e_success)
    {
        printf("File extension decoded successfully\n");
    }
    else
    {
        printf("Failed to decode file extension\n");
        return e_failure;
    }

    if (decode_secret_file_size(decInfo) == e_success)
    {
        printf("File size decoded successfully\n");
    }
    else
    {
        printf("Failed to decode file size\n");
        return e_failure;
    }

    if (decode_secret_file_data(decInfo) == e_success)
    {
        printf("Successfully decoded data from image\n");
        printf("Decoded file with name %s\n", decInfo->secret_fname);
    }
    else
    {
        printf("Failed to decode data from image\n");
        return e_failure;
    }
    return e_success;
}
