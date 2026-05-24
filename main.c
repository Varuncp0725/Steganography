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
            printf("Invalid arguments!\n");
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
            printf("Invalid arguments!\n");
        }
    }
    else
    {
        printf("Invalid Operations!\n");
    }

    return 0;
    // step1 -> call check_operation_type(argv[1]);
    //  print msg
    //-e -> encode
    // declare structure variable EncodeInfo encInfo;
    // check read_and_validate_encode_args(argv, &encInfo) is e_success or not
    // no -> print error msg and return e_failure;
    // yes -> check do_encoding(&encInfo) returning e_success or not
    // no -> print error msg and stop
    // yes -> print success msg and stop
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
    // step1 -> check it is -e or -d
    //-e ->return e_encode
    //-d -> return e_decode

    // return e_unsupported
}
