#include "util.hpp"

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

void util::print_func(std::array<TMTCHAR, 80 * 24> *_cells, TMTPOINT *cursor, LightLock *lock)
{
    LightLock_Lock(lock);
    std::array<TMTCHAR, 80 * 24> chars = *_cells;
    C2D_TextBufClear(ui.top_text_buf);
    static C2D_Text *txt = (C2D_Text *)malloc(80 * 24 * sizeof(C2D_Text));
    for(u32 r = 0; r < 24; r++)
    {
        for(u32 c = 0; c < 80; c++)
        {
            C2D_TextFontParse(&txt[r * 80 + c], ui.top_font, ui.top_text_buf, &chars[r * 80 + c].c);
            C2D_TextOptimize(&txt[r * 80 + c]);
            C2D_DrawRectSolid(c * 5, r * 10, 1.0f, 5.0f, 10.0f, colors[chars[r * 80 + c].a.bg]);
            C2D_DrawText(&txt[r * 80 + c], C2D_WithColor, c * 5, r * 10, 1.0f, 1.0f, 1.0f, colors[chars[r * 80 + c].a.fg]);
        }
    }
    C2D_Text input_txt;
    C2D_TextFontParse(&input_txt, ui.top_font, ui.top_text_buf, "_");
    C2D_TextOptimize(&input_txt);
    C2D_DrawText(&input_txt, C2D_WithColor, cursor->c * 5, cursor->r * 10, 1.0f, 1.0f, 1.0f, 0xFFFFFFFF);
    LightLock_Unlock(lock);
}

void util::paint_callback(tmt_msg_t m, struct TMT *v, const void *r, void *p)
{
    util *utils = (util *)p;
    const TMTSCREEN *s = tmt_screen(v);
    const TMTPOINT *cur = tmt_cursor(v);

    switch (m){
        case TMT_MSG_UPDATE:
            /* the screen image changed; a is a pointer to the TMTSCREEN */
            for (size_t r = 0; r < s->nline; r++)
            {
                if (s->lines[r]->dirty)
                {
                    for (size_t c = 0; c < s->ncol; c++)
                    {
                        utils->cells[r * s->ncol + c] = s->lines[r]->chars[c];
                    }
                }
            }
            /* let tmt know we've redrawn the screen */
            tmt_clean(v);
            break;

        case TMT_MSG_MOVED:
                /* the cursor moved; a is a pointer to the cursor's TMTPOINT */
            utils->cursor_pos = *cur;
            break;
        case TMT_MSG_ANSWER:
        case TMT_MSG_BELL:
            break;
    }
}

void util::put_char(LightLock lock, TMT *vt, char c)
{
   // printf("Put char\n");
    LightLock_Lock(&lock);
        tmt_write(vt, &c, 1);
    LightLock_Unlock(&lock);
}

void util::print(std::string str)
{
    printf("Printing function\n");
    LightLock_Lock(&lock);
        printf(str.c_str());
        tmt_write(vt, str.c_str(), str.length());
    LightLock_Unlock(&lock);
    printf("Written\n");
}

void util::set_print_func(std::function<void()>&func)
{
    LightLock_Init(&lock);
    vt = tmt_open(24, 80, util::paint_callback, this, nullptr);
    func = std::bind(util::print_func, &cells, &cursor_pos, &lock);
}