#pragma once
extern "C"
{
    #include "tsm/libtsm.h"
    #include "tsm/libtsm-int.h"
}

#include <algorithm>
#include <string>
#include <functional>
#include <vector>
#include <array>
#include <tuple>
#include <3ds.h>
#include <citro2d.h>
#include "ui.hpp"
extern uiFuncs_s ui;

static uint8_t color_palette[TSM_COLOR_NUM][3] = {
  	[TSM_COLOR_BLACK]         = { 0x00, 0x00, 0x00 },
  	[TSM_COLOR_RED]           = { 0xFF, 0x00, 0x00 },
  	[TSM_COLOR_GREEN]         = { 0x00, 0xFF, 0x00 },
  	[TSM_COLOR_YELLOW]        = { 0xFF, 0xFF, 0x00 },
  	[TSM_COLOR_BLUE]          = { 0x00, 0x00, 0xFF },
  	[TSM_COLOR_MAGENTA]       = { 0xFF, 0xFF, 0x00 },
  	[TSM_COLOR_CYAN]          = { 0x00, 0xFF, 0xFF },
  	[TSM_COLOR_LIGHT_GREY]    = { 0xD3, 0xD3, 0xD3 },
  	[TSM_COLOR_DARK_GREY]     = { 0xA9, 0xA9, 0xA9 },
  	[TSM_COLOR_LIGHT_RED]     = { 0xab, 0x46, 0x42 },
  	[TSM_COLOR_LIGHT_GREEN]   = { 0xa1, 0xb5, 0x6c },
 	[TSM_COLOR_LIGHT_YELLOW]  = { 0xf7, 0xca, 0x88 },
  	[TSM_COLOR_LIGHT_BLUE]    = { 0x7c, 0xaf, 0xc2 },
  	[TSM_COLOR_LIGHT_MAGENTA] = { 0xba, 0x8b, 0xaf },
  	[TSM_COLOR_LIGHT_CYAN]    = { 0x86, 0xc1, 0xb9 },
  	[TSM_COLOR_WHITE]         = { 0xff, 0xff, 0xff },
 
  	[TSM_COLOR_FOREGROUND]    = { 0xFF, 0xFF, 0xFF },
  	[TSM_COLOR_BACKGROUND]    = { 0x00, 0x00, 0x00 },
  };
typedef struct 
{
    uint32_t c;
    tsm_screen_attr attr;
} cell_s;

typedef struct 
{
    int x;
    int y;
} point_s;

class util
{
    public:
    static void put_char(util *utils, char c);
    void print(std::string str);
    void error(std::string str);
    void set_print_func(std::function<void()>&func);

    LightLock lock;
    point_s cursor;
    //std::vector<std::tuple<vtcell_t, int, int>> cells;
    cell_s *cells;
    //static void print_func(std::vector<std::string> *vec, std::string *input, float &_textscroll, float &_scrollbar);
    struct tsm_screen *console;
    struct tsm_vte *vte;
    static int dummy_draw_cb(struct tsm_screen *con, uint64_t id, const uint32_t *ch, size_t len,
				   unsigned int width, unsigned int posx, unsigned int posy, const struct tsm_screen_attr *attr,
				   tsm_age_t age, void *data);
    private:
    float textscroll = 0;
    float scrollbar = 0;
    static void log_tsm(void *data, const char *file, int line, const char *func,
			   const char *subs, unsigned int sev, const char *format, va_list args);
    static void print_func(void *p);
    static void dummy_writecb(struct tsm_vte *vte, const char *u8,
				  size_t len, void *data);

};