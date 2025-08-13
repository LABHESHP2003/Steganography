#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>

/* int main()
{
    EncodeInfo encInfo;
    uint img_size;

    // Fill with sample filenames
    encInfo.src_image_fname = "beautiful.bmp";
    encInfo.secret_fname = "secret.txt";
    encInfo.stego_image_fname = "stego_img.bmp";

    // Test open_files
    if (open_files(&encInfo) == e_failure)
    {
    	printf("ERROR: %s function failed\n", "open_files" );
    	return 1;
    }
    else
    {
    	printf("SUCCESS: %s function completed\n", "open_files" );
    }

    // Test get_image_size_for_bmp
    img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);

    return 0;
}  */

int main(int argc,char **argv){
    if(check_operation_type(argv) == e_encode){
        printf("User selected encoding.\n");
        EncodeInfo encoding;
        if(read_and_validate_encode_args(argv,&encoding) == e_success){
            printf("Read and validate encode arguments is a success\n");
            printf("Started encoding..........\n");
            if(do_encoding(&encoding) == e_success){
                printf("Encoding completed successfully\n");
            }
            else{
                printf("Failed to perform encoding\n");
                return -1;
            }
        }
        else{
            printf("Read and validate encode arguments is a failure\n");
            return 1;
        }
    }
    else if (check_operation_type(argv) == e_decode){
        printf("User selected decoding.\n");
    }
    else{
        printf("Invalid Input\n");
        printf("For encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]\n");
        printf("For decoding : ./a.out -d stego.bmp [decode.txt]\n");
    }
    return 0;
}

OperationType check_operation_type(char **argv){
    if(strcmp(argv[1],"-e") == 0){
         return e_encode;
    }
    else if(strcmp(argv[1],"-d") == 0){
        return e_decode;
    }
    else{
        return e_unsupported;
    }
}   
