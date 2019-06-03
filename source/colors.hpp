#include <3ds.h>
#include <citro2d.h>
static u32 colors[] = 
{
    //[TMT_COLOR_DEFAULT] = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF),
    [TMT_COLOR_BLACK]  = 0x000000FF,
    [TMT_COLOR_RED]    = C2D_Color32(0xFF, 0, 0, 0xFF),
    [TMT_COLOR_GREEN]  = C2D_Color32(0, 0xFF, 0, 0xFF),
    [TMT_COLOR_YELLOW] = 0xFF00FFFF,
    [TMT_COLOR_BLUE]   = C2D_Color32(0, 0, 0xFF, 0xFF),
    [TMT_COLOR_MAGENTA] = 0xFFFF00FF,
    [TMT_COLOR_CYAN]   = C2D_Color32(0, 0xFF, 0xFF, 0xFF),
    [TMT_COLOR_WHITE]   = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF)
};