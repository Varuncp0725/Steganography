#include <stdio.h>
#include <string.h>
#include "common.h"
#include "encode.h"
#include "types.h"

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
    
    fseek(fptr_image, 18, SEEK_SET);

    
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{

    fseek(fptr, 0, SEEK_END);
    int size = ftell(fptr);
    return size;
}


//| Magic String | Extn Size | Extn | Secret File Size | Secret File Data |
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strstr(argv[2], ".bmp") == NULL)
    {
        printf("ERROR:  Source image file must have a .bmp extension!\n");
        return e_failure;
    }
    encInfo->src_image_fname = argv[2];
    if (strchr(argv[3], '.') == NULL)
    {
        printf("ERROR:  Secret file must be . file!\n");
        return e_failure;
    }
    encInfo->secret_fname = argv[3];
    strcpy(encInfo->extn_secret_file, strchr(argv[3], '.'));
    if (argv[4] == NULL)
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    else if (strstr(argv[4], ".bmp") == NULL)
    {
        printf("INFO:  Output File not mentioned. Creating stego.bmp as default\n");
        return e_failure;
    }
    else
    {
        encInfo->stego_image_fname = argv[4];
    }
    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
   
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");

    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
   
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

   
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    uint max = 0;
    max = HEADER_SIZE + (8 * (strlen(MAGIC_STRING) + 4 + strlen(encInfo->extn_secret_file) + encInfo->size_secret_file + 4)); // first 4 is ".txt", last 4 is data size of secret file

    if (encInfo->image_capacity > max)
    {
        return e_success;
    }
    printf("ERROR:  Image Doesn't have enough capacity for encoding!!!\n");
    return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[HEADER_SIZE];
    rewind(fptr_src_image);
    if (fread(buffer, HEADER_SIZE, 1, fptr_src_image) != 1)
    {
        printf("ERROR:  Couldn't read BMP header properly!!!\n");
        return e_failure;
    }
    rewind(fptr_src_image);
    if (fwrite(buffer, HEADER_SIZE, 1, fptr_dest_image) != 1)
    {
        printf("ERROR:  Couldn't write BMP header properly!!!\n");
        return e_failure;
    }
    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    for (int i = 0; i < strlen(magic_string); i++)
    {
        char buffer[8];
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char buffer[8];
    for (int i = 0; i < strlen(file_extn); i++)
    {

        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    char ch;
    char buffer[8];
    while (fread(&ch, 1, 1, encInfo->fptr_secret) == 1)
    {

        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src))
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int bit = 0;
    for (int i = 7; i >= 0; i--)
    {
        bit = (data >> i) & 1;
        image_buffer[7 - i] = (image_buffer[7 - i] & 0xFE) | bit;
    }
    return e_success;
    
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for (int i = 31; i >= 0; i--)
    {
        int bit = (size >> i) & 1;
        imageBuffer[31 - i] = (imageBuffer[31 - i] & 0xFE) | bit;
    }
    return e_success;
    
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
        printf("INFO: Files opened successfully\n");
    }
    else
    {
        printf("ERROR: Failed to open files\n");
        return e_failure;
    }
    if (check_capacity(encInfo) == e_success)
    {
        printf("INFO: Image has sufficient capacity\n");
    }
    else
    {
        printf("ERROR: Image doesn't have enough capcity\n");
        return e_failure;
    }
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: BMP header copied successfully\n");
    }
    else
    {
        return e_failure;
    }
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("INFO: Magic string encoded successfully\n");
    }
    else
    {
        return e_failure;
    }
    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
    {
        printf("INFO: Secret file extension size encoded successfully\n");
    }
    else
    {
        return e_failure;
    }
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        printf("INFO: Secret file extension encoded successfully\n");
    }
    else
    {
        return e_failure;
    }
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
    {
        printf("INFO: Secret file size encoded successfully\n");
    }
    else
    {
        return e_failure;
    }
    if (encode_secret_file_data(encInfo) == e_success)
    {

        printf("INFO: Secret file data encoded successfully\n");
    }
    else
    {
        return e_failure;
    }
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: Remaining image data copied successfully\n");
    }
    else
    {
        return e_failure;
    }
    fclose(encInfo->fptr_src_image);

    fclose(encInfo->fptr_secret);

    fclose(encInfo->fptr_stego_image);
    return e_success;
}
