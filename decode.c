#include <stdio.h>
#include <string.h>
#include "common.h"
#include "encode.h"
#include "types.h"
#include "decode.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strstr(argv[2], ".bmp") == NULL)
    {
        printf("ERROR:  Source image file must have a .bmp extension!\n");
        return e_failure;
    }
    decInfo->stego_image_fname = argv[2];
    if (argv[3] == NULL)
    {
        decInfo->secret_fname = "output";
    }
    else
    {
        decInfo->secret_fname = argv[3];
    }
    return e_success;
}
Status open_image_file(DecodeInfo *decInfo)
{
    // open stego image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open filr %s\n", decInfo->stego_image_fname);

        return e_failure;
    }
    return e_success;
}
Status skip_bmp_header(FILE *fptr_stego_image)
{
    fseek(fptr_stego_image, HEADER_SIZE, SEEK_SET);
    return e_success;
}
Status decode_magic_string(DecodeInfo *decInfo)
{
    char str[3];
    char buffer[8];
    for (int i = 0; i < strlen(MAGIC_STRING); i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        str[i] = decode_byte_from_lsb(buffer);
    }
    str[strlen(MAGIC_STRING)] = '\0';
    if (strcmp(str, MAGIC_STRING) == 0)
    {
        return e_success;
    }
    return e_failure;
}
char decode_byte_from_lsb(char *image_buffer)
{
    char ch = 0;
    for (int i = 0; i < 8; i++)
    {
        ch = ch << 1;
        ch = ch | (image_buffer[i] & 1);
    }
    return ch;
}
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, decInfo->fptr_stego_image);
    decInfo->secret_file_extn_size = decode_int_from_lsb(buffer);
    return e_success;
}
long decode_int_from_lsb(char *image_buffer)
{
    long num = 0;
    for (int i = 0; i < 32; i++)
    {
        num = num << 1;
        num = num | (image_buffer[i] & 1);
    }
    return num;
}
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[8];
    for (int i = 0; i < decInfo->secret_file_extn_size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        decInfo->extn_secret_file[i] = decode_byte_from_lsb(buffer);
    }
    decInfo->extn_secret_file[decInfo->secret_file_extn_size] = '\0';
    // Concatenate the extension with secret file (nestin.txt)
    // decodeInfo->secret_fname = realloc(decodeInfo->secret_fname, sizeof(extn) + 7);
    char fname[30];
    strcpy(fname, decInfo->secret_fname);
    strcat(fname, decInfo->extn_secret_file);
    decInfo->secret_fname = fname;
    // open secret file in write mode
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "wb");
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, decInfo->fptr_stego_image);
    decInfo->secret_file_size = decode_int_from_lsb(buffer);
    return e_success;
}
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;

    for (int i = 0; i < decInfo->secret_file_size; i++)
    {
       
        fread(buffer, 8, 1, decInfo->fptr_stego_image);

        
        ch = decode_byte_from_lsb(buffer);

       
        fputc(ch, decInfo->fptr_secret);
    }
    if (decInfo->fptr_secret == NULL)
    {
        return e_failure;
    }
    return e_success;
}
Status do_decoding(DecodeInfo *decInfo)
{
    if (open_image_file(decInfo) == e_success)
    {
        printf("INFO: Files opened successfully\n");
    }
    else
    {
        printf("ERROR: Failed to open files\n");
        return e_failure;
    }
    if (skip_bmp_header(decInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: BMP header skipped successfully\n");
    }
    else
    {
        return e_failure;
    }
    if (decode_magic_string(decInfo) == e_success)
    {
        printf("INFO: Magic string Decoded successfully\n");
    }
    else
    {
        return e_failure;
    }
    if (decode_secret_file_extn_size(decInfo) == e_success)
    {
        printf("INFO: Secret file extension size Decoded successfully\n");
    }
    else
    {
        return e_failure;
    }
    if (decode_secret_file_extn(decInfo) == e_success)
    {
        printf("INFO: Secret file extension Decoded successfully\n");
    }
    else
    {
        return e_failure;
    }
    if (decode_secret_file_size(decInfo) == e_success)
    {
        printf("INFO: Secret file size Decoded successfully\n");
    }
    else
    {
        return e_failure;
    }
    if (decode_secret_file_data(decInfo) == e_success)
    {

        printf("INFO: Secret file data Decoded successfully\n");
    }
    else
    {
        return e_failure;
    }
    fclose(decInfo->fptr_stego_image);

    fclose(decInfo->fptr_secret);

    return e_success;
}