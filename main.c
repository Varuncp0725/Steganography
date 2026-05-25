#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    if (check_operation_type(argv[1]) == e_encode)
    {
        printf("Encoding is selected\n");
        EncodeInfo encInfo;
        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Read and Validation success\n");
            if (do_encoding(&encInfo) == e_success)
            {
                printf("\033[32mEncoded Successfully\033[0m\n");
            }
            else
            {
                printf("\033[31mEncoding Failed!\033[0m\n");
            }
        }
        else
        {
            printf("\033[31mInvalid arguments!\033[0m\n");
        }
    }
    else if (check_operation_type(argv[1]) == e_decode)
    {
        printf("Decoding is selected\n");
        DecodeInfo decInfo;
        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("Read and Validation success\n");
            if (do_decoding(&decInfo) == e_success)
            {
                printf("\033[32mDecoded Successfully\033[0m\n");
            }
            else
            {
                printf("\033[31mDecoding Failed!\033[0m\n");
            }
        }
        else
        {
            printf("\033[31mInvalid arguments!\033[0m\n");
        }
    }
    else
    {
        printf("Invalid Operations!\n");
    }

    return 0;

}

OperationType check_operation_type(char *symbol)
{

    if (strcmp(symbol, "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(symbol, "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
