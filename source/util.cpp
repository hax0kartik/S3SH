#include "util.hpp"
#include <memory>

/*
void util::draw_scrollbar(float &text_scroll, float &_scrollbar, float max_scroll)
{
    float viewableRatio = 24 / max_scroll; 
    float thumbHeight = (viewableRatio * 234);

    float scrollTrackSpace = (max_scroll * 10) - 234; 
    float scrollThumbSpace = scrollTrackSpace - thumbHeight;
    float scrollUnit = scrollTrackSpace / scrollThumbSpace;

    float textScrollSpace = max_scroll;
    float textScrollUnit = textScrollSpace * 10 / 24; 

    hidScanInput();
    if(keysDown() & KEY_DDOWN || keysHeld() & KEY_DDOWN)
    {
        if(_scrollbar + thumbHeight < 236.0f)
        _scrollbar += scrollUnit;
    }
    
    if(keysDown() & KEY_DUP || keysHeld() & KEY_DUP)
    {   
        if(_scrollbar >= 0.0f)
        _scrollbar -= scrollUnit;
    }
    text_scroll = -(_scrollbar * (max_scroll * 10 ) / 234.0);
    C2D_DrawRectSolid(400.0f - 7.0f, _scrollbar + 2.0, 1.0f, 5.0f, thumbHeight, 0x60606060);

}
*/
void util::log_tsm(void *data, const char *file, int line, const char *func,
			   const char *subs, unsigned int sev, const char *format, va_list args)
{
    fprintf(stdout, "%d : %s", sev, subs);
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
}

void util::dummy_writecb(struct tsm_vte *vte,
				  const char *u8,
				  size_t len,
				  void *data)
{
    printf("Write cb: %d:%s", len, u8);
}

void util::print_func(void *p)
{
    util *utils = (util*)p;
    LightLock_Lock(&utils->lock);
    C2D_TextBufClear(ui.top_text_buf);
    static C2D_Text *txt = new C2D_Text[80 * 24];
    for(u32 r = 0; r < 24; r++)
    {
        for(u32 c = 0; c < 80; c++)
        {  
            if(!utils->cells[r * 80 + c].c)
                continue;
            tsm_screen_attr attr = utils->cells[r * 80 + c].attr;
            size_t output_len = 0;
            char *ch = tsm_ucs4_to_utf8_alloc(&utils->cells[r * 80 + c].c, 1, &output_len);
            if (!ch || output_len < 1) continue;
            /*
            if(attr.inverse)
            {
                u8 br = attr.br;
                u8 bg = attr.bg;
                u8 bb = attr.bb;

                attr.br = attr.fr;
                attr.bg = attr.fg;
                attr.bb = attr.fb;

                attr.fr = br;
                attr.fg = bg;
                attr.fb = bb;
            }
            */
            u32 bg = C2D_Color32(attr.br, attr.bg, attr.bb, 0xFF);
            u32 fg = C2D_Color32(attr.fr, attr.fg, attr.fb, 0xFF);
            C2D_TextFontParse(&txt[r * 80 + c], ui.top_font, ui.top_text_buf, ch);
            C2D_TextOptimize(&txt[r * 80 + c]);
            C2D_DrawRectSolid(c * 5, r * 10, 1.0f, 5.0f, 10.0f, bg);
            C2D_DrawText(&txt[r * 80 + c], C2D_WithColor, c * 5, r * 10, 1.0f, 1.0f, 1.0f, fg);
            free(ch);
        }
    }
    C2D_Text input_txt;
    C2D_TextFontParse(&input_txt, ui.top_font, ui.top_text_buf, "_");
    C2D_TextOptimize(&input_txt);
    C2D_DrawText(&input_txt, C2D_WithColor, utils->cursor.x * 5, utils->cursor.y * 10, 1.0f, 1.0f, 1.0f, 0xFFFFFFFF);
    LightLock_Unlock(&utils->lock);
}

/** This dummy function just saves the cell attributes 
 * to private data members for later use*/

int util::dummy_draw_cb(struct tsm_screen *con, uint64_t id, const uint32_t *ch, size_t len,
				   unsigned int width, unsigned int posx, unsigned int posy, const struct tsm_screen_attr *attr,
				   tsm_age_t age, void *data)
{
    //printf("In within dummy_draw\n");
    util *utils = (util *)data;
    for(int i = 0; i < len; i++)
    {
        utils->cells[(posy * 80 + i) + posx].c = ch[i];
        memcpy(&utils->cells[(posy * 80 + i) + posx].attr, attr, sizeof(*attr));
    }
    if(!len)
    {
        memcpy(&utils->cells[(posy * 80) + posx].attr, attr, sizeof(*attr));
        utils->cells[(posy * 80) + posx].c = 32; 
    }

    utils->cursor.x = tsm_screen_get_cursor_x(con);
    utils->cursor.y = tsm_screen_get_cursor_y(con);
    return 0;
}

void util::put_char(util *utils, char c)
{
   // printf("Put char\n");
    LightLock_Lock(&utils->lock);
        tsm_vte_input(utils->vte, &c, 1);
        tsm_screen_draw(utils->console, utils->dummy_draw_cb, utils);
    LightLock_Unlock(&utils->lock);
}

void util::print(std::string str)
{
    printf("Printing function\n");
    LightLock_Lock(&lock);
        tsm_vte_input(vte, str.c_str(), str.length());
        tsm_screen_draw(console, util::dummy_draw_cb, this);
    LightLock_Unlock(&lock);
    printf("Written\n");
}

void util::error(std::string str)
{
    print(str);
    print("\n\rPress Start To continue\n\r");
    while(aptMainLoop())
    {
        hidScanInput();
        if(keysDown() & KEY_START)
            break;
    }
}

void util::set_print_func(std::function<void()>&func)
{
    LightLock_Init(&lock);

    cells = new cell_s[80 * 24];
    int e = tsm_screen_new(&console, util::log_tsm, nullptr);
    e = tsm_screen_resize(console, 80, 24);
    int w = tsm_screen_get_width(console);
    printf("TSM_SCREEN_NEW %d console %d\n", e, w);
    e = tsm_vte_new(&vte, console, util::dummy_writecb, nullptr, util::log_tsm, nullptr);
    printf("TSM_VTE_NEW %d vte %x\n", e, vte);
    e = tsm_vte_set_custom_palette(vte, color_palette);
    printf("TSM_VTE_SET_PALLETE %d\n", e);
    func = std::bind(util::print_func, this);
}