/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --font components/lvgl/scripts/built_in_font/FontAwesome5-Solid+Brands+Regular.woff -r 0xF048,0xF04B,0xF04C,0xF051 --size 16 --bpp 1 --format lvgl --lv-font-name mediaIcons --no-kerning -o main/src/Theme/mediaIcons.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef MEDIAICONS
#define MEDIAICONS 1
#endif

#if MEDIAICONS

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+F048 "" */
    0xc0, 0xf0, 0x7c, 0x3f, 0x1f, 0xdf, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xf7, 0xfc, 0x7f, 0xf, 0xc1,
    0xf0, 0x30,

    /* U+F04B "" */
    0xe0, 0x3, 0xc0, 0xf, 0xc0, 0x3f, 0xc0, 0xff,
    0x83, 0xff, 0x8f, 0xff, 0xbf, 0xff, 0xff, 0xff,
    0xff, 0xef, 0xfe, 0x3f, 0xf0, 0xff, 0x3, 0xf0,
    0xf, 0x0, 0x38, 0x0,

    /* U+F04C "" */
    0xfc, 0xff, 0xf3, 0xff, 0xcf, 0xff, 0x3f, 0xfc,
    0xff, 0xf3, 0xff, 0xcf, 0xff, 0x3f, 0xfc, 0xff,
    0xf3, 0xff, 0xcf, 0xff, 0x3f, 0xfc, 0xff, 0xf3,
    0xf0,

    /* U+F051 "" */
    0xc0, 0xf8, 0x3f, 0xf, 0xe3, 0xfe, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xbf, 0x8f, 0xc3, 0xe0,
    0xf0, 0x30
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 224, .box_w = 10, .box_h = 14, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 18, .adv_w = 224, .box_w = 14, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 46, .adv_w = 224, .box_w = 14, .box_h = 14, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 71, .adv_w = 224, .box_w = 10, .box_h = 14, .ofs_x = 2, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0x3, 0x4, 0x9
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 61512, .range_length = 10, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
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
    .cmap_num = 1,
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
const lv_font_t mediaIcons = {
#else
lv_font_t mediaIcons = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 16,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -6,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if MEDIAICONS*/

