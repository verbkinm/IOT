/*******************************************************************************
 * Size: 10 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

#include "lvgl.h"

#ifndef UBUNTU_MONO_10
#define UBUNTU_MONO_10 1
#endif

#if UBUNTU_MONO_10

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xf2,

    /* U+0022 "\"" */
    0xb4,

    /* U+0023 "#" */
    0x29, 0x7e, 0xaf, 0xa9, 0x40,

    /* U+0024 "$" */
    0x23, 0xe0, 0x83, 0x7, 0xe4,

    /* U+0025 "%" */
    0xed, 0xad, 0xf5, 0xae, 0x60,

    /* U+0026 "&" */
    0x74, 0xa8, 0x9b, 0xc9, 0xa0,

    /* U+0028 "(" */
    0x2a, 0x49, 0x12, 0x20,

    /* U+0029 ")" */
    0x88, 0x92, 0x52, 0x80,

    /* U+002A "*" */
    0x4b, 0x6a,

    /* U+002B "+" */
    0x27, 0xc8, 0x40,

    /* U+002C "," */
    0x58,

    /* U+002D "-" */
    0xc0,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x12, 0x22, 0x44, 0x48, 0x80,

    /* U+0030 "0" */
    0x74, 0x63, 0x58, 0xc5, 0xc0,

    /* U+0031 "1" */
    0x2e, 0x22, 0x22, 0xf0,

    /* U+0032 "2" */
    0xf1, 0x12, 0x48, 0xf0,

    /* U+0033 "3" */
    0xe1, 0x16, 0x11, 0xe0,

    /* U+0034 "4" */
    0x32, 0x95, 0x2f, 0x88, 0x40,

    /* U+0035 "5" */
    0x7a, 0x10, 0xe0, 0x87, 0xc0,

    /* U+0036 "6" */
    0x24, 0x8f, 0x99, 0x60,

    /* U+0037 "7" */
    0xf1, 0x22, 0x44, 0x40,

    /* U+0038 "8" */
    0x69, 0x96, 0x99, 0x60,

    /* U+0039 "9" */
    0x69, 0x99, 0x72, 0xc0,

    /* U+003A ":" */
    0x88,

    /* U+003B ";" */
    0x41, 0x60,

    /* U+003C "<" */
    0x3, 0xa0, 0xe0,

    /* U+003D "=" */
    0xf0, 0xf0,

    /* U+003E ">" */
    0x3, 0x82, 0xe0,

    /* U+003F "?" */
    0xf1, 0x12, 0x40, 0x40,

    /* U+0040 "@" */
    0x72, 0x67, 0x5a, 0xd6, 0x68, 0x30,

    /* U+0041 "A" */
    0x31, 0x45, 0x12, 0x7a, 0x28, 0x80,

    /* U+0042 "B" */
    0xe4, 0xa5, 0xc9, 0xc7, 0xc0,

    /* U+0043 "C" */
    0x76, 0x21, 0x8, 0x61, 0xe0,

    /* U+0044 "D" */
    0xf4, 0xe3, 0x18, 0xcf, 0xc0,

    /* U+0045 "E" */
    0xf8, 0x8f, 0x88, 0xf0,

    /* U+0046 "F" */
    0xf8, 0x8f, 0x88, 0x80,

    /* U+0047 "G" */
    0x7e, 0x21, 0x8, 0xe5, 0xe0,

    /* U+0048 "H" */
    0x8c, 0x63, 0xf8, 0xc6, 0x20,

    /* U+0049 "I" */
    0xe9, 0x24, 0xb8,

    /* U+004A "J" */
    0x71, 0x11, 0x11, 0xe0,

    /* U+004B "K" */
    0x94, 0xa9, 0x8a, 0x4a, 0x20,

    /* U+004C "L" */
    0x88, 0x88, 0x88, 0xf0,

    /* U+004D "M" */
    0xde, 0xf7, 0x58, 0xc6, 0x20,

    /* U+004E "N" */
    0x9d, 0xdd, 0xbb, 0x90,

    /* U+004F "O" */
    0x74, 0xe3, 0x18, 0xcd, 0xc0,

    /* U+0050 "P" */
    0xe9, 0x99, 0xe8, 0x80,

    /* U+0051 "Q" */
    0x76, 0xe3, 0x18, 0xc5, 0xc4, 0x10,

    /* U+0052 "R" */
    0xe9, 0x99, 0xea, 0x90,

    /* U+0053 "S" */
    0xf8, 0x86, 0x11, 0xe0,

    /* U+0054 "T" */
    0xf9, 0x8, 0x42, 0x10, 0x80,

    /* U+0055 "U" */
    0x8c, 0x63, 0x18, 0xc5, 0xc0,

    /* U+0056 "V" */
    0x8c, 0x52, 0xa5, 0x18, 0xc0,

    /* U+0057 "W" */
    0x8c, 0x63, 0xde, 0xef, 0x60,

    /* U+0058 "X" */
    0x4a, 0x8c, 0x45, 0x26, 0x20,

    /* U+0059 "Y" */
    0x8c, 0x54, 0xc2, 0x10, 0x80,

    /* U+005A "Z" */
    0xf1, 0x24, 0x88, 0xf0,

    /* U+005C "\\" */
    0x88, 0x44, 0x42, 0x22, 0x10,

    /* U+005E "^" */
    0x4a, 0x90,

    /* U+005F "_" */
    0xfc,

    /* U+0060 "`" */
    0x80,

    /* U+0061 "a" */
    0x71, 0xf9, 0xf0,

    /* U+0062 "b" */
    0x88, 0xe9, 0x99, 0xe0,

    /* U+0063 "c" */
    0x7c, 0x21, 0x7, 0x80,

    /* U+0064 "d" */
    0x11, 0x79, 0x99, 0x70,

    /* U+0065 "e" */
    0x74, 0xbd, 0x7, 0x0,

    /* U+0066 "f" */
    0x7a, 0x3c, 0x84, 0x21, 0x0,

    /* U+0067 "g" */
    0x79, 0x99, 0x71, 0xe0,

    /* U+0068 "h" */
    0x88, 0xe9, 0x99, 0x90,

    /* U+0069 "i" */
    0x20, 0x38, 0x42, 0x10, 0xe0,

    /* U+006A "j" */
    0x10, 0x71, 0x11, 0x11, 0xe0,

    /* U+006B "k" */
    0x84, 0x25, 0x4c, 0x52, 0x40,

    /* U+006C "l" */
    0xe1, 0x8, 0x42, 0x10, 0xe0,

    /* U+006D "m" */
    0xdd, 0x6b, 0x18, 0x80,

    /* U+006E "n" */
    0xe9, 0x99, 0x90,

    /* U+006F "o" */
    0x74, 0x63, 0x17, 0x0,

    /* U+0070 "p" */
    0xe9, 0x99, 0xe8, 0x80,

    /* U+0071 "q" */
    0x79, 0x99, 0x71, 0x10,

    /* U+0072 "r" */
    0xf8, 0x88, 0x80,

    /* U+0073 "s" */
    0xf8, 0x43, 0xf0,

    /* U+0074 "t" */
    0x4, 0xf4, 0x44, 0x70,

    /* U+0075 "u" */
    0x99, 0x99, 0x70,

    /* U+0076 "v" */
    0x8a, 0x54, 0x62, 0x0,

    /* U+0077 "w" */
    0x8d, 0x6f, 0xb4, 0x80,

    /* U+0078 "x" */
    0x49, 0x8c, 0xa4, 0x80,

    /* U+0079 "y" */
    0x4a, 0x54, 0x63, 0x13, 0x80,

    /* U+007A "z" */
    0xf2, 0x48, 0xf0,

    /* U+007C "|" */
    0xff, 0x80,

    /* U+007E "~" */
    0x6c, 0xc0,

    /* U+00B0 "°" */
    0xf7, 0x80,

    /* U+0401 "Ё" */
    0x6f, 0x88, 0xf8, 0x8f,

    /* U+0410 "А" */
    0x31, 0x45, 0x12, 0x7a, 0x28, 0x80,

    /* U+0411 "Б" */
    0xf4, 0x21, 0xe8, 0xc7, 0xc0,

    /* U+0412 "В" */
    0xe4, 0xa5, 0xc9, 0xc7, 0xc0,

    /* U+0413 "Г" */
    0x7a, 0x10, 0x84, 0x23, 0xc0,

    /* U+0414 "Д" */
    0x7a, 0x52, 0x94, 0xc7, 0xf1, 0x88,

    /* U+0415 "Е" */
    0xf8, 0x8f, 0x88, 0xf0,

    /* U+0416 "Ж" */
    0xad, 0x5c, 0xeb, 0x56, 0xa0,

    /* U+0417 "З" */
    0x70, 0x42, 0x60, 0x85, 0xc0,

    /* U+0418 "И" */
    0x9b, 0xbd, 0xdd, 0x90,

    /* U+0419 "Й" */
    0x66, 0x9b, 0xbd, 0xdd, 0x90,

    /* U+041A "К" */
    0x94, 0xa9, 0x8a, 0x4a, 0x20,

    /* U+041B "Л" */
    0x39, 0x4a, 0x52, 0xa7, 0x20,

    /* U+041C "М" */
    0xde, 0xf7, 0x58, 0xc6, 0x20,

    /* U+041D "Н" */
    0x8c, 0x63, 0xf8, 0xc6, 0x20,

    /* U+041E "О" */
    0x74, 0xe3, 0x18, 0xcd, 0xc0,

    /* U+041F "П" */
    0xfc, 0x63, 0x18, 0xc6, 0x20,

    /* U+0420 "Р" */
    0xe9, 0x99, 0xe8, 0x80,

    /* U+0421 "С" */
    0x76, 0x21, 0x8, 0x61, 0xe0,

    /* U+0422 "Т" */
    0xf9, 0x8, 0x42, 0x10, 0x80,

    /* U+0423 "У" */
    0x89, 0x24, 0x8c, 0x10, 0x8e, 0x0,

    /* U+0424 "Ф" */
    0x23, 0xab, 0x5a, 0xb8, 0x80,

    /* U+0425 "Х" */
    0x4a, 0x8c, 0x45, 0x26, 0x20,

    /* U+0426 "Ц" */
    0x94, 0xa5, 0x29, 0x4b, 0xe1, 0x8,

    /* U+0427 "Ч" */
    0x99, 0x99, 0x71, 0x10,

    /* U+0428 "Ш" */
    0x8c, 0x63, 0x5a, 0xd7, 0xe0,

    /* U+0429 "Щ" */
    0x94, 0xa5, 0x6b, 0x5b, 0xe1, 0x8,

    /* U+042A "Ъ" */
    0xc2, 0x10, 0xe4, 0xa5, 0xc0,

    /* U+042B "Ы" */
    0x8c, 0x63, 0xda, 0xd7, 0xa0,

    /* U+042C "Ь" */
    0x84, 0x21, 0xe8, 0xc7, 0xc0,

    /* U+042D "Э" */
    0xf0, 0xc2, 0xf0, 0x8f, 0xc0,

    /* U+042E "Ю" */
    0xb6, 0x73, 0x9c, 0xe6, 0xc0,

    /* U+042F "Я" */
    0x79, 0x99, 0x75, 0x90,

    /* U+0430 "а" */
    0x71, 0xf9, 0xf0,

    /* U+0431 "б" */
    0x74, 0x3d, 0x18, 0xc5, 0xc0,

    /* U+0432 "в" */
    0xf9, 0xe9, 0xf0,

    /* U+0433 "г" */
    0xf8, 0x88, 0x80,

    /* U+0434 "д" */
    0x7a, 0x53, 0x1f, 0xc4,

    /* U+0435 "е" */
    0x74, 0xbd, 0x7, 0x0,

    /* U+0436 "ж" */
    0xab, 0x9d, 0x6a, 0x80,

    /* U+0437 "з" */
    0xf1, 0x61, 0xe0,

    /* U+0438 "и" */
    0x9b, 0xdd, 0x90,

    /* U+0439 "й" */
    0x46, 0x9b, 0xdd, 0x90,

    /* U+043A "к" */
    0x95, 0x31, 0x49, 0x0,

    /* U+043B "л" */
    0x39, 0x4a, 0x54, 0x80,

    /* U+043C "м" */
    0x8e, 0xfb, 0x58, 0x80,

    /* U+043D "н" */
    0x99, 0xf9, 0x90,

    /* U+043E "о" */
    0x74, 0x63, 0x17, 0x0,

    /* U+043F "п" */
    0xf9, 0x99, 0x90,

    /* U+0440 "р" */
    0xe9, 0x99, 0xe8, 0x80,

    /* U+0441 "с" */
    0x7c, 0x21, 0x7, 0x80,

    /* U+0442 "т" */
    0xf9, 0x8, 0x42, 0x0,

    /* U+0443 "у" */
    0x4a, 0x54, 0x63, 0x13, 0x80,

    /* U+0444 "ф" */
    0x21, 0x1d, 0x5a, 0xd5, 0xc4, 0x20,

    /* U+0445 "х" */
    0x49, 0x8c, 0xa4, 0x80,

    /* U+0446 "ц" */
    0x94, 0xa5, 0x2f, 0x84,

    /* U+0447 "ч" */
    0x99, 0xf1, 0x10,

    /* U+0448 "ш" */
    0x8d, 0x6b, 0x5f, 0x80,

    /* U+0449 "щ" */
    0x95, 0xad, 0x6f, 0x84,

    /* U+044A "ъ" */
    0xc2, 0x1e, 0x97, 0x80,

    /* U+044B "ы" */
    0x8c, 0x7b, 0x5e, 0x80,

    /* U+044C "ь" */
    0x88, 0xf9, 0xe0,

    /* U+044D "э" */
    0xe1, 0x71, 0xe0,

    /* U+044E "ю" */
    0xb6, 0x73, 0x9b, 0x0,

    /* U+044F "я" */
    0xf9, 0x79, 0x90,

    /* U+0451 "ё" */
    0x30, 0x1d, 0x2f, 0x41, 0xc0,

    /* U+2116 "№" */
    0xad, 0xff, 0xde, 0x56, 0x80
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 90, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 90, .box_w = 1, .box_h = 7, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 90, .box_w = 3, .box_h = 2, .ofs_x = 2, .ofs_y = 5},
    {.bitmap_index = 3, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 8, .adv_w = 90, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 13, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 18, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 23, .adv_w = 90, .box_w = 3, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 27, .adv_w = 90, .box_w = 3, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 31, .adv_w = 90, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 33, .adv_w = 90, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 36, .adv_w = 90, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 37, .adv_w = 90, .box_w = 2, .box_h = 1, .ofs_x = 2, .ofs_y = 2},
    {.bitmap_index = 38, .adv_w = 90, .box_w = 1, .box_h = 1, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 39, .adv_w = 90, .box_w = 4, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 44, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 49, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 53, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 57, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 61, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 66, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 71, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 75, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 79, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 83, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 87, .adv_w = 90, .box_w = 1, .box_h = 5, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 88, .adv_w = 90, .box_w = 2, .box_h = 6, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 90, .adv_w = 90, .box_w = 5, .box_h = 4, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 93, .adv_w = 90, .box_w = 4, .box_h = 3, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 95, .adv_w = 90, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 98, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 102, .adv_w = 90, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 108, .adv_w = 90, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 114, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 119, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 124, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 129, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 133, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 137, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 142, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 147, .adv_w = 90, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 150, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 154, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 168, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 177, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 181, .adv_w = 90, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 187, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 191, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 195, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 200, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 205, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 210, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 215, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 220, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 225, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 229, .adv_w = 90, .box_w = 4, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 234, .adv_w = 90, .box_w = 4, .box_h = 3, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 236, .adv_w = 90, .box_w = 6, .box_h = 1, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 237, .adv_w = 90, .box_w = 2, .box_h = 2, .ofs_x = 2, .ofs_y = 5},
    {.bitmap_index = 238, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 241, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 245, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 249, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 253, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 257, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 262, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 266, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 270, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 275, .adv_w = 90, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 280, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 285, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 290, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 294, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 297, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 301, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 305, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 309, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 312, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 315, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 319, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 322, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 326, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 330, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 334, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 339, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 342, .adv_w = 90, .box_w = 1, .box_h = 9, .ofs_x = 2, .ofs_y = -2},
    {.bitmap_index = 344, .adv_w = 90, .box_w = 5, .box_h = 2, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 346, .adv_w = 90, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 348, .adv_w = 90, .box_w = 4, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 352, .adv_w = 90, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 358, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 363, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 368, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 373, .adv_w = 90, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 379, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 383, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 388, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 393, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 397, .adv_w = 90, .box_w = 4, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 402, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 407, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 412, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 417, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 422, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 427, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 432, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 436, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 441, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 446, .adv_w = 90, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 452, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 457, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 462, .adv_w = 90, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 468, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 472, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 477, .adv_w = 90, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 483, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 488, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 493, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 498, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 503, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 508, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 512, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 515, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 520, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 523, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 526, .adv_w = 90, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 530, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 534, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 538, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 541, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 544, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 548, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 552, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 556, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 560, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 563, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 567, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 570, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 574, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 578, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 582, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 587, .adv_w = 90, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 593, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 597, .adv_w = 90, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 601, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 604, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 608, .adv_w = 90, .box_w = 5, .box_h = 6, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 612, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 616, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 620, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 623, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 626, .adv_w = 90, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 630, .adv_w = 90, .box_w = 4, .box_h = 5, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 633, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 638, .adv_w = 90, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0}
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
const lv_font_t ubuntu_mono_10 = {
#else
lv_font_t ubuntu_mono_10 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 11,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
    .fallback = NULL,
    .user_data = NULL
};



#endif /*#if UBUNTU_MONO_10*/

