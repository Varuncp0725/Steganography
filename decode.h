#ifndef DECODE_H
#define DECODE_H
#include "encode.h"
#include "types.h"
#include "common.h"

typedef struct _DecodeInfo
{
    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    uint secret_file_size;
    uint secret_file_extn_size;
    char extn_secret_file[5]; // added to store decoded extension

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

} DecodeInfo;
/* Decoding function prototype */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);
Status open_image_file(DecodeInfo *decInfo);
Status skip_bmp_header(FILE *fptr_stego_image);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
char decode_byte_from_lsb(char *image_buffer);
long decode_int_from_lsb(char *image_buffer);

#endif