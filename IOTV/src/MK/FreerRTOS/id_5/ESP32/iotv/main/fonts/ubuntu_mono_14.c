/*******************************************************************************
 * Size: 14 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

#include "lvgl.h"

#ifndef UBUNTU_MONO_14
#define UBUNTU_MONO_14 1
#endif

#if UBUNTU_MONO_14

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xaa, 0xa8, 0xf0,

    /* U+0022 "\"" */
    0xb6, 0xd0,

    /* U+0023 "#" */
    0x12, 0x6b, 0xf9, 0x22, 0x44, 0xbf, 0xa4, 0x48,
    0x90,

    /* U+0024 "$" */
    0x23, 0xe1, 0x8, 0x30, 0xc1, 0x8, 0x7c, 0x42,
    0x0,

    /* U+0025 "%" */
    0xe5, 0x4a, 0xa7, 0x41, 0x2, 0x8a, 0x95, 0x4b,
    0x8,

    /* U+0026 "&" */
    0x30, 0x91, 0x22, 0x43, 0xe, 0xa5, 0x44, 0x8c,
    0xe8,

    /* U+0028 "(" */
    0x13, 0x64, 0xc8, 0x88, 0x88, 0xc4, 0x63, 0x10,

    /* U+0029 ")" */
    0x8c, 0x62, 0x31, 0x11, 0x11, 0x32, 0x6c, 0x80,

    /* U+002A "*" */
    0x21, 0x3e, 0x45, 0x28,

    /* U+002B "+" */
    0x10, 0x41, 0x3f, 0x10, 0x41, 0x0,

    /* U+002C "," */
    0x6c, 0xe8,

    /* U+002D "-" */
    0xe0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0xc, 0x20, 0x86, 0x10, 0x41, 0x8, 0x20, 0x84,
    0x10, 0x42, 0x0,

    /* U+0030 "0" */
    0x79, 0x28, 0x6d, 0xb6, 0x18, 0x61, 0x49, 0xc0,

    /* U+0031 "1" */
    0x23, 0x28, 0x42, 0x10, 0x84, 0x27, 0xc0,

    /* U+0032 "2" */
    0x74, 0x42, 0x11, 0x19, 0x88, 0x87, 0xc0,

    /* U+0033 "3" */
    0xf0, 0x20, 0x86, 0x70, 0x20, 0x41, 0xf, 0xe0,

    /* U+0034 "4" */
    0xc, 0x18, 0x51, 0x24, 0x48, 0xbf, 0x82, 0x4,
    0x8,

    /* U+0035 "5" */
    0x7d, 0x4, 0x1c, 0x8, 0x10, 0x41, 0x8f, 0xe0,

    /* U+0036 "6" */
    0x19, 0x84, 0x3e, 0x8e, 0x18, 0x61, 0x49, 0xe0,

    /* U+0037 "7" */
    0xfc, 0x30, 0x84, 0x10, 0xc2, 0x8, 0x21, 0x80,

    /* U+0038 "8" */
    0x7b, 0x38, 0x61, 0x49, 0xe8, 0x61, 0xcd, 0xe0,

    /* U+0039 "9" */
    0x7b, 0x28, 0x61, 0xc5, 0xf0, 0x42, 0x1b, 0x80,

    /* U+003A ":" */
    0xf0, 0xf,

    /* U+003B ";" */
    0x6c, 0x0, 0xd9, 0xd0,

    /* U+003C "<" */
    0x0, 0x33, 0x30, 0x60, 0x60, 0x40,

    /* U+003D "=" */
    0xfc, 0x0, 0x3f,

    /* U+003E ">" */
    0x3, 0x3, 0x83, 0x19, 0x88, 0x0,

    /* U+003F "?" */
    0xe1, 0x11, 0x24, 0x40, 0x66,

    /* U+0040 "@" */
    0x3c, 0x89, 0xc, 0xfb, 0x34, 0x68, 0xd1, 0xb3,
    0xbd, 0x1, 0x1, 0xc0,

    /* U+0041 "A" */
    0x18, 0x18, 0x24, 0x24, 0x24, 0x44, 0x7e, 0x42,
    0x42, 0x83,

    /* U+0042 "B" */
    0xfa, 0x38, 0x63, 0xfa, 0x38, 0x61, 0x8f, 0xe0,

    /* U+0043 "C" */
    0x3d, 0xc, 0x20, 0x82, 0x8, 0x30, 0x40, 0xf0,

    /* U+0044 "D" */
    0xf2, 0x28, 0x61, 0x86, 0x18, 0x61, 0x8b, 0xc0,

    /* U+0045 "E" */
    0xfa, 0x8, 0x20, 0xfa, 0x8, 0x20, 0x83, 0xf0,

    /* U+0046 "F" */
    0xfc, 0x21, 0xf, 0xc2, 0x10, 0x84, 0x0,

    /* U+0047 "G" */
    0x3d, 0x1c, 0x20, 0x82, 0x18, 0x71, 0x44, 0xf0,

    /* U+0048 "H" */
    0x86, 0x18, 0x61, 0xfe, 0x18, 0x61, 0x86, 0x10,

    /* U+0049 "I" */
    0xf9, 0x8, 0x42, 0x10, 0x84, 0x27, 0xc0,

    /* U+004A "J" */
    0x3c, 0x10, 0x41, 0x4, 0x10, 0x41, 0xd, 0xe0,

    /* U+004B "K" */
    0x8d, 0x12, 0x45, 0xc, 0x14, 0x24, 0x44, 0x8d,
    0x8,

    /* U+004C "L" */
    0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x83, 0xf0,

    /* U+004D "M" */
    0x4f, 0x3c, 0xed, 0xb6, 0xd8, 0x61, 0x86, 0x10,

    /* U+004E "N" */
    0x87, 0x1e, 0x69, 0xa6, 0x59, 0x63, 0x8e, 0x10,

    /* U+004F "O" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x44,
    0x70,

    /* U+0050 "P" */
    0xfa, 0x38, 0x61, 0x8f, 0xe8, 0x20, 0x82, 0x0,

    /* U+0051 "Q" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x82,
    0x88, 0xe0, 0x80, 0xc0, 0x0,

    /* U+0052 "R" */
    0xfa, 0x38, 0x61, 0x8f, 0xe9, 0xa2, 0x86, 0x10,

    /* U+0053 "S" */
    0x7f, 0x8, 0x20, 0x60, 0x60, 0x41, 0xf, 0xe0,

    /* U+0054 "T" */
    0xfc, 0x41, 0x4, 0x10, 0x41, 0x4, 0x10, 0x40,

    /* U+0055 "U" */
    0x86, 0x18, 0x61, 0x86, 0x18, 0x61, 0xcd, 0xe0,

    /* U+0056 "V" */
    0xc3, 0x42, 0x42, 0x42, 0x64, 0x24, 0x24, 0x2c,
    0x18, 0x18,

    /* U+0057 "W" */
    0x86, 0x18, 0x61, 0xb6, 0xdb, 0x73, 0xcf, 0x30,

    /* U+0058 "X" */
    0x42, 0xc8, 0x91, 0xc1, 0x83, 0x9, 0x12, 0x46,
    0x84,

    /* U+0059 "Y" */
    0x83, 0x8d, 0x13, 0x62, 0x86, 0x4, 0x8, 0x10,
    0x20,

    /* U+005A "Z" */
    0xfc, 0x30, 0x84, 0x30, 0x84, 0x10, 0x83, 0xf0,

    /* U+005C "\\" */
    0x81, 0x4, 0x10, 0x20, 0x82, 0x4, 0x10, 0x41,
    0x82, 0x8, 0x20,

    /* U+005E "^" */
    0x30, 0xc4, 0x92, 0x86, 0x0,

    /* U+005F "_" */
    0xff,

    /* U+0060 "`" */
    0xa4,

    /* U+0061 "a" */
    0x78, 0x10, 0x5f, 0xc6, 0x18, 0x5f,

    /* U+0062 "b" */
    0x82, 0x8, 0x20, 0xf2, 0x28, 0x61, 0x86, 0x18,
    0xbc,

    /* U+0063 "c" */
    0x3d, 0x8, 0x20, 0x82, 0x4, 0xf,

    /* U+0064 "d" */
    0x4, 0x10, 0x41, 0x3d, 0x18, 0x61, 0x86, 0x14,
    0x4f,

    /* U+0065 "e" */
    0x3c, 0xcd, 0xb, 0xf4, 0x8, 0x18, 0x1f,

    /* U+0066 "f" */
    0x1e, 0x40, 0x87, 0xe2, 0x4, 0x8, 0x10, 0x20,
    0x40, 0x80,

    /* U+0067 "g" */
    0x3d, 0x18, 0x61, 0x86, 0x14, 0x5f, 0x4, 0x3f,
    0x80,

    /* U+0068 "h" */
    0x82, 0x8, 0x20, 0xfa, 0x38, 0x61, 0x86, 0x18,
    0x61,

    /* U+0069 "i" */
    0x61, 0x80, 0x38, 0x20, 0x82, 0x8, 0x20, 0x81,
    0xc0,

    /* U+006A "j" */
    0x18, 0xc0, 0xf0, 0x84, 0x21, 0x8, 0x42, 0x10,
    0xf8,

    /* U+006B "k" */
    0x82, 0x8, 0x20, 0x8e, 0x6b, 0x30, 0xe2, 0x48,
    0xa1,

    /* U+006C "l" */
    0xe0, 0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x81,
    0xc0,

    /* U+006D "m" */
    0xed, 0x26, 0x4c, 0x99, 0x30, 0x60, 0xc1,

    /* U+006E "n" */
    0xfa, 0x38, 0x61, 0x86, 0x18, 0x61,

    /* U+006F "o" */
    0x79, 0x28, 0x61, 0x86, 0x14, 0x9e,

    /* U+0070 "p" */
    0xf2, 0x28, 0x61, 0x86, 0x18, 0xbc, 0x82, 0x8,
    0x0,

    /* U+0071 "q" */
    0x3d, 0x18, 0x61, 0x86, 0x14, 0x4f, 0x4, 0x10,
    0x40,

    /* U+0072 "r" */
    0xfc, 0x21, 0x8, 0x42, 0x10,

    /* U+0073 "s" */
    0x7c, 0x20, 0xc3, 0x4, 0x3e,

    /* U+0074 "t" */
    0x20, 0x8f, 0xc8, 0x20, 0x82, 0x8, 0x20, 0x70,

    /* U+0075 "u" */
    0x86, 0x18, 0x61, 0x86, 0x1c, 0x5f,

    /* U+0076 "v" */
    0x86, 0x18, 0xd2, 0x49, 0x43, 0xc,

    /* U+0077 "w" */
    0x83, 0x6, 0x4c, 0x9a, 0xcd, 0x93, 0x22,

    /* U+0078 "x" */
    0x42, 0x48, 0xe0, 0xc1, 0x84, 0x99, 0xa1,

    /* U+0079 "y" */
    0x86, 0x1c, 0xd2, 0x48, 0xa3, 0xc, 0x10, 0x8c,
    0x0,

    /* U+007A "z" */
    0xf8, 0xc4, 0x46, 0x22, 0x1f,

    /* U+007C "|" */
    0xff, 0xfc,

    /* U+007E "~" */
    0xe6, 0x70,

    /* U+00B0 "°" */
    0x69, 0x96,

    /* U+0401 "Ё" */
    0x50, 0x1, 0xf8, 0x42, 0x1e, 0x84, 0x21, 0xf,
    0x80,

    /* U+0410 "А" */
    0x18, 0x18, 0x24, 0x24, 0x24, 0x44, 0x7e, 0x42,
    0x42, 0x83,

    /* U+0411 "Б" */
    0xfa, 0x8, 0x20, 0xfa, 0x38, 0x61, 0x8f, 0xe0,

    /* U+0412 "В" */
    0xfa, 0x38, 0x63, 0xfa, 0x38, 0x61, 0x8f, 0xe0,

    /* U+0413 "Г" */
    0x7e, 0x81, 0x2, 0x4, 0x8, 0x10, 0x20, 0x41,
    0xf0,

    /* U+0414 "Д" */
    0x3c, 0x48, 0x91, 0x22, 0x44, 0x89, 0x22, 0x45,
    0xfe, 0xc, 0x18, 0x20,

    /* U+0415 "Е" */
    0xfa, 0x8, 0x20, 0xfa, 0x8, 0x20, 0x83, 0xf0,

    /* U+0416 "Ж" */
    0x93, 0xa9, 0x53, 0xe3, 0x8f, 0x15, 0x2a, 0x93,
    0x24,

    /* U+0417 "З" */
    0xf8, 0x30, 0x43, 0x78, 0x30, 0x41, 0x8f, 0xe0,

    /* U+0418 "И" */
    0x8e, 0x39, 0xe5, 0xb6, 0x9a, 0x71, 0xc6, 0x10,

    /* U+0419 "Й" */
    0x48, 0xc0, 0x23, 0x8e, 0x79, 0x6d, 0xa6, 0x9c,
    0x71, 0x84,

    /* U+041A "К" */
    0x8d, 0x12, 0x45, 0xc, 0x14, 0x24, 0x44, 0x8d,
    0x8,

    /* U+041B "Л" */
    0x3e, 0x44, 0x89, 0x12, 0x24, 0x48, 0xa1, 0x43,
    0x4,

    /* U+041C "М" */
    0x4f, 0x3c, 0xed, 0xb6, 0xd8, 0x61, 0x86, 0x10,

    /* U+041D "Н" */
    0x86, 0x18, 0x61, 0xfe, 0x18, 0x61, 0x86, 0x10,

    /* U+041E "О" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x44,
    0x70,

    /* U+041F "П" */
    0xfe, 0x18, 0x61, 0x86, 0x18, 0x61, 0x86, 0x10,

    /* U+0420 "Р" */
    0xfa, 0x38, 0x61, 0x8f, 0xe8, 0x20, 0x82, 0x0,

    /* U+0421 "С" */
    0x3d, 0xc, 0x20, 0x82, 0x8, 0x30, 0x40, 0xf0,

    /* U+0422 "Т" */
    0xfc, 0x41, 0x4, 0x10, 0x41, 0x4, 0x10, 0x40,

    /* U+0423 "У" */
    0xc3, 0x42, 0x42, 0x26, 0x24, 0x14, 0x18, 0x8,
    0x10, 0x60,

    /* U+0424 "Ф" */
    0x10, 0xe5, 0xa5, 0x96, 0x59, 0x65, 0x78, 0xc1,
    0x0,

    /* U+0425 "Х" */
    0x42, 0xc8, 0x91, 0xc1, 0x83, 0x9, 0x12, 0x46,
    0x84,

    /* U+0426 "Ц" */
    0x85, 0xa, 0x14, 0x28, 0x50, 0xa1, 0x42, 0x85,
    0xfc, 0x8, 0x10, 0x20,

    /* U+0427 "Ч" */
    0x86, 0x18, 0x61, 0xc5, 0xf0, 0x41, 0x4, 0x10,

    /* U+0428 "Ш" */
    0x83, 0x6, 0xc, 0x19, 0x32, 0x64, 0xc9, 0x93,
    0xfc,

    /* U+0429 "Щ" */
    0x86, 0x18, 0x61, 0xa6, 0x9a, 0x69, 0xa7, 0xf0,
    0x41, 0x4,

    /* U+042A "Ъ" */
    0xe0, 0x40, 0x81, 0xe2, 0x64, 0x48, 0x91, 0x26,
    0x78,

    /* U+042B "Ы" */
    0x83, 0x6, 0xf, 0x19, 0x32, 0x64, 0xc9, 0x93,
    0xc4,

    /* U+042C "Ь" */
    0x82, 0x8, 0x3e, 0x8e, 0x18, 0x61, 0x8f, 0xe0,

    /* U+042D "Э" */
    0xf0, 0x20, 0x41, 0x7c, 0x10, 0x43, 0xb, 0xc0,

    /* U+042E "Ю" */
    0x9d, 0x6a, 0x8d, 0x1e, 0x34, 0x68, 0xd1, 0xb5,
    0x38,

    /* U+042F "Я" */
    0x7f, 0x18, 0x61, 0xc5, 0xf6, 0x51, 0xc6, 0x10,

    /* U+0430 "а" */
    0x78, 0x10, 0x5f, 0xc6, 0x18, 0x5f,

    /* U+0431 "б" */
    0x1d, 0x84, 0x20, 0xbb, 0x38, 0x61, 0x86, 0x14,
    0x9e,

    /* U+0432 "в" */
    0xfa, 0x18, 0x7e, 0x86, 0x18, 0xfe,

    /* U+0433 "г" */
    0xfc, 0x21, 0x8, 0x42, 0x10,

    /* U+0434 "д" */
    0x3c, 0x48, 0x91, 0x22, 0x48, 0x91, 0x7f, 0x83,
    0x4,

    /* U+0435 "е" */
    0x3c, 0xcd, 0xb, 0xf4, 0x8, 0x18, 0x1f,

    /* U+0436 "ж" */
    0x96, 0xa8, 0xe1, 0xc3, 0x8a, 0xb5, 0xc9,

    /* U+0437 "з" */
    0xf0, 0x42, 0xe0, 0x84, 0x3e,

    /* U+0438 "и" */
    0x86, 0x39, 0x65, 0xa7, 0x1c, 0x61,

    /* U+0439 "й" */
    0x48, 0xe0, 0x21, 0x8e, 0x59, 0x69, 0xc7, 0x18,
    0x40,

    /* U+043A "к" */
    0x8e, 0x6a, 0x30, 0xe2, 0x48, 0xa1,

    /* U+043B "л" */
    0x3c, 0x92, 0x49, 0x25, 0x94, 0x61,

    /* U+043C "м" */
    0xc7, 0x8f, 0x1d, 0x5a, 0xb2, 0x60, 0xc1,

    /* U+043D "н" */
    0x8c, 0x63, 0xf8, 0xc6, 0x31,

    /* U+043E "о" */
    0x79, 0x28, 0x61, 0x86, 0x14, 0x9e,

    /* U+043F "п" */
    0xfc, 0x63, 0x18, 0xc6, 0x31,

    /* U+0440 "р" */
    0xf2, 0x28, 0x61, 0x86, 0x18, 0xbc, 0x82, 0x8,
    0x0,

    /* U+0441 "с" */
    0x3d, 0x8, 0x20, 0x82, 0x4, 0xf,

    /* U+0442 "т" */
    0xfc, 0x82, 0x8, 0x20, 0x82, 0x8,

    /* U+0443 "у" */
    0x86, 0x1c, 0xd2, 0x48, 0xa3, 0xc, 0x10, 0x8c,
    0x0,

    /* U+0444 "ф" */
    0x10, 0x20, 0x40, 0x83, 0xca, 0xa4, 0xc9, 0x93,
    0x25, 0x53, 0xc1, 0x2, 0x4, 0x0,

    /* U+0445 "х" */
    0x42, 0x48, 0xe0, 0xc1, 0x84, 0x99, 0xa1,

    /* U+0446 "ц" */
    0x8a, 0x28, 0xa2, 0x8a, 0x28, 0xbf, 0x4, 0x10,

    /* U+0447 "ч" */
    0x86, 0x18, 0x61, 0x7c, 0x10, 0x41,

    /* U+0448 "ш" */
    0x83, 0x6, 0x4c, 0x99, 0x32, 0x64, 0xff,

    /* U+0449 "щ" */
    0x86, 0x1a, 0x69, 0xa6, 0x9a, 0x7f, 0x4, 0x10,

    /* U+044A "ъ" */
    0xc1, 0x4, 0x1e, 0x45, 0x14, 0x5e,

    /* U+044B "ы" */
    0x86, 0x18, 0x79, 0x96, 0x59, 0x79,

    /* U+044C "ь" */
    0x82, 0x8, 0x3e, 0x86, 0x18, 0x7e,

    /* U+044D "э" */
    0xf0, 0x20, 0x4f, 0x4, 0x10, 0xbc,

    /* U+044E "ю" */
    0x9a, 0x9a, 0x79, 0xa6, 0x9a, 0x66,

    /* U+044F "я" */
    0x7c, 0x63, 0x17, 0xa6, 0x31,

    /* U+0451 "ё" */
    0x24, 0x0, 0xf3, 0x34, 0x2f, 0xd0, 0x20, 0x60,
    0x78,

    /* U+2116 "№" */
    0x97, 0xaf, 0x5e, 0xbd, 0x76, 0x2d, 0xd8, 0x90
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 125, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 125, .box_w = 2, .box_h = 10, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 125, .box_w = 3, .box_h = 4, .ofs_x = 2, .ofs_y = 7},
    {.bitmap_index = 6, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 15, .adv_w = 125, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 24, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 33, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 42, .adv_w = 125, .box_w = 4, .box_h = 15, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 50, .adv_w = 125, .box_w = 4, .box_h = 15, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 58, .adv_w = 125, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 62, .adv_w = 125, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 68, .adv_w = 125, .box_w = 3, .box_h = 5, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 70, .adv_w = 125, .box_w = 3, .box_h = 1, .ofs_x = 2, .ofs_y = 4},
    {.bitmap_index = 71, .adv_w = 125, .box_w = 2, .box_h = 2, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 72, .adv_w = 125, .box_w = 6, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 83, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 91, .adv_w = 125, .box_w = 5, .box_h = 10, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 98, .adv_w = 125, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 105, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 113, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 122, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 130, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 138, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 146, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 154, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 162, .adv_w = 125, .box_w = 2, .box_h = 8, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 164, .adv_w = 125, .box_w = 3, .box_h = 10, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 168, .adv_w = 125, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 174, .adv_w = 125, .box_w = 6, .box_h = 4, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 177, .adv_w = 125, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 183, .adv_w = 125, .box_w = 4, .box_h = 10, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 188, .adv_w = 125, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 200, .adv_w = 125, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 210, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 218, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 226, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 234, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 242, .adv_w = 125, .box_w = 5, .box_h = 10, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 249, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 257, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 265, .adv_w = 125, .box_w = 5, .box_h = 10, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 272, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 280, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 289, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 297, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 305, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 313, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 322, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 330, .adv_w = 125, .box_w = 7, .box_h = 14, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 343, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 351, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 359, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 367, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 375, .adv_w = 125, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 385, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 393, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 402, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 411, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 419, .adv_w = 125, .box_w = 6, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 430, .adv_w = 125, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 435, .adv_w = 125, .box_w = 8, .box_h = 1, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 436, .adv_w = 125, .box_w = 2, .box_h = 3, .ofs_x = 3, .ofs_y = 9},
    {.bitmap_index = 437, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 443, .adv_w = 125, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 452, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 458, .adv_w = 125, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 467, .adv_w = 125, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 474, .adv_w = 125, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 484, .adv_w = 125, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 493, .adv_w = 125, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 502, .adv_w = 125, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 511, .adv_w = 125, .box_w = 5, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 520, .adv_w = 125, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 529, .adv_w = 125, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 538, .adv_w = 125, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 545, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 551, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 557, .adv_w = 125, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 566, .adv_w = 125, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 575, .adv_w = 125, .box_w = 5, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 580, .adv_w = 125, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 585, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 593, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 599, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 605, .adv_w = 125, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 612, .adv_w = 125, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 619, .adv_w = 125, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 628, .adv_w = 125, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 633, .adv_w = 125, .box_w = 1, .box_h = 14, .ofs_x = 3, .ofs_y = -3},
    {.bitmap_index = 635, .adv_w = 125, .box_w = 6, .box_h = 2, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 637, .adv_w = 125, .box_w = 4, .box_h = 4, .ofs_x = 2, .ofs_y = 8},
    {.bitmap_index = 639, .adv_w = 125, .box_w = 5, .box_h = 13, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 648, .adv_w = 125, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 658, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 666, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 674, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 683, .adv_w = 125, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 695, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 703, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 712, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 720, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 728, .adv_w = 125, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 738, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 747, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 756, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 764, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 772, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 781, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 789, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 797, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 805, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 813, .adv_w = 125, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 823, .adv_w = 125, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 832, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 841, .adv_w = 125, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 853, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 861, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 870, .adv_w = 125, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 880, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 889, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 898, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 906, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 914, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 923, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 931, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 937, .adv_w = 125, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 946, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 952, .adv_w = 125, .box_w = 5, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 957, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 966, .adv_w = 125, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 973, .adv_w = 125, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 980, .adv_w = 125, .box_w = 5, .box_h = 8, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 985, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 991, .adv_w = 125, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1000, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1006, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1012, .adv_w = 125, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1019, .adv_w = 125, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1024, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1030, .adv_w = 125, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1035, .adv_w = 125, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1044, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1050, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1056, .adv_w = 125, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1065, .adv_w = 125, .box_w = 7, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1079, .adv_w = 125, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1086, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1094, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1100, .adv_w = 125, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1107, .adv_w = 125, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1115, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1121, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1127, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1133, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1139, .adv_w = 125, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1145, .adv_w = 125, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1150, .adv_w = 125, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1159, .adv_w = 125, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_4[] = {
    0x0, 0x2, 0x34, 0x385
};

static const uint16_t unicode_list_6[] = {
    0x0, 0x1cc5
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 7, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 40, .range_length = 51, .glyph_id_start = 8,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 92, .range_length = 1, .glyph_id_start = 59,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 94, .range_length = 29, .glyph_id_start = 60,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 124, .range_length = 902, .glyph_id_start = 89,
        .unicode_list = unicode_list_4, .glyph_id_ofs_list = NULL, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 1040, .range_length = 64, .glyph_id_start = 93,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 1105, .range_length = 7366, .glyph_id_start = 157,
        .unicode_list = unicode_list_6, .glyph_id_ofs_list = NULL, .list_length = 2, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 7,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t ubuntu_mono_14 = {
#else
lv_font_t ubuntu_mono_14 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 17,          /*The maximum line height required by the font*/
    .base_line = 4,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
    .fallback = NULL,
    .user_data = NULL
};



#endif /*#if UBUNTU_MONO_14*/

