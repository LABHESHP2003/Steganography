#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "string.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(char **argv,EncodeInfo *encInfo){
    if(strstr(argv[2],".bmp") != NULL){
        encInfo->src_image_fname = argv[2];
    }
    else{
        return e_failure;
    }

    if(strstr(argv[3],".txt") != NULL){
        encInfo->secret_fname = argv[3];
    }
    else{
        return e_failure;
    }

    if (argv[4] != NULL)
    {
        if (strstr(argv[4], ".bmp") != NULL)
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            return e_failure;
        }
    }
    else{
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
}

uint get_file_size(FILE *fptr){
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

Status check_capacity(EncodeInfo *encInfo){
    //sizeof beautiful.bmp > (16+32+ 32+ 32+ (sizeof secret.txt *8 ))
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    if(encInfo->image_capacity > (16 + 32 + 32 + 32 + (encInfo->size_secret_file *8))){
        return e_success;
    }
    else{
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){
    char str[54];
    fseek(fptr_src_image,0,SEEK_SET);
    int byteRead = fread(str,1,54,fptr_src_image);
    if(byteRead != 54){
        printf("Error: only read %d bytes\n", byteRead);
        return e_failure;
    }
    fseek(fptr_dest_image,0,SEEK_SET);
    fwrite(str,1,54,fptr_dest_image);
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo){
    if(open_files(encInfo) == e_success){
        printf("Open file is a success\n");
    }
    else{
        printf("Error opening file\n");
        return e_failure;
    }

    if(check_capacity(encInfo) == e_success){
        printf("%s file can be used to perform encoding\n",encInfo->src_image_fname);
    }
    else{
        printf("%s file cannot be used to perform encoding\n",encInfo->src_image_fname);
        return e_failure;
    }

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success){
        printf("Copied bmp header of %s to %s\n", encInfo -> src_image_fname, encInfo -> stego_image_fname);
    }
    else{
        printf("Failed to copy bmp header of %s to %s\n", encInfo -> src_image_fname, encInfo -> stego_image_fname);
        return e_failure;
    }
    return e_success;
}
