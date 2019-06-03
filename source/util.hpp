#pragma once
extern "C"
{
    #include "tmt.h"
    #include "colors.hpp"
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
class util
{
    public:
    static void put_char(LightLock lock, TMT *vt, char c);
    void print(std::string str);
    void set_print_func(std::function<void()>&func);

    TMT *vt;
    LightLock lock;
    
    private:
    float textscroll = 0;
    float scrollbar = 0;
    TMTPOINT cursor_pos;
    //std::vector<std::tuple<vtcell_t, int, int>> cells;
    std::array<TMTCHAR, 80 * 24> cells;
    //static void print_func(std::vector<std::string> *vec, std::string *input, float &_textscroll, float &_scrollbar);
    static void print_func(std::array<TMTCHAR, 80 * 24> *_cells, TMTPOINT *cursor, LightLock *lock);
    static void draw_scrollbar(float &textscroll, float &scrollbar, float max_scroll);
    static void paint_callback(tmt_msg_t m, struct TMT *v, const void *r, void *p);
};