#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
/*
 * Structure to store information required for
 * decoding data from source Image
 * Info about output data file is stored here
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
   /* Source Image info */
   char *src_image_fname;
   FILE *fptr_src_image;
   char image_data[MAX_IMAGE_BUF_SIZE];
   char *decoded_magic_string;

   /* Secret File Info */
   char *secret_fname;
   FILE *fptr_secret;
   uint file_ext_size;
   char extn_secret_file[MAX_FILE_SUFFIX];
   uint size_secret_file;

} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/*Skip image header*/
Status header_skiper(FILE *fptr_src);

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/*Decode the size of the extension*/
Status decode_size_of_extension(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_data_from_image(void *data, int size, DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
char decode_byte_from_lsb(char *image_buffer);

/*To decode an int from the LSB of the data*/
Status decode_int_from_lsb(uint *value, DecodeInfo *decInfo);

#endif