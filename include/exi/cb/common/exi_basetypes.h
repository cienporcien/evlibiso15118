/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (C) 2022 - 2023 chargebyte GmbH
 * Copyright (C) 2022 - 2023 Contributors to EVerest
 */

/*****************************************************
 *
 * @author
 * @version
 *
 * The Code is generated! Changes may be overwritten.
 *
 *****************************************************/

/**
  * @file exi_basetypes.h
  * @brief Description goes here
  *
  **/

#ifndef EXI_BASETYPES_H
#define EXI_BASETYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <stdint.h>


// TODO: delete me later! just for compiling the still not reworked parts.
#define EXI_ELEMENT_STACK_SIZE 24
#define EXTRA_CHAR 1
//

#define ASCII_EXTRA_CHAR 1
#define ASCII_CHAR_TERMINATOR '\0'

#define EXI_STRING_MAX_LEN 64
#define EXI_BYTE_ARRAY_MAX_LEN 350

#define EXI_BASETYPES_MAX_OCTETS_SUPPORTED 10

#define EXI_BASETYPES_OCTET_SEQ_FLAG_MASK 0x80
#define EXI_BASETYPES_OCTET_SEQ_VALUE_MASK 0x7F

#define EXI_BASETYPES_UINT8_MAX_OCTETS 2
#define EXI_BASETYPES_UINT16_MAX_OCTETS 3
#define EXI_BASETYPES_UINT32_MAX_OCTETS 5
#define EXI_BASETYPES_UINT64_MAX_OCTETS 10


typedef struct
{
    size_t octets_size;
    uint8_t* octets;
    size_t octets_count;
} exi_binary_t;

typedef struct
{
    uint8_t octets[EXI_BASETYPES_MAX_OCTETS_SUPPORTED];
    size_t octets_count;
} exi_unsigned_t;

typedef char exi_character_t;

int exi_basetypes_convert_to_unsigned(exi_unsigned_t* exi_unsigned, uint32_t value, size_t max_octets);
int exi_basetypes_convert_64_to_unsigned(exi_unsigned_t* exi_unsigned, uint64_t value);

int exi_basetypes_convert_from_unsigned(exi_unsigned_t* exi_unsigned, uint32_t* value, size_t max_octets);
int exi_basetypes_convert_64_from_unsigned(exi_unsigned_t* exi_unsigned, uint64_t* value);


#ifdef __cplusplus
}
#endif

#endif /* EXI_BASETYPES_H */

