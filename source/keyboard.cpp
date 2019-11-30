#include "keyboard.hpp"

void keyboard::print(C2D_Image layout, C2D_Image keypress, LightLock *lock, std::array<key_s, NUM_KEYS> keys, bool *capslock, int *selected)
{
    LightLock_Lock(lock);
    C2D_TextBufClear(ui.bot_text_buf);
    static C2D_Text *buttons = (C2D_Text *)malloc(NUM_KEYS * sizeof(C2D_Text));
    C2D_DrawImageAt(layout, 2.0f, 67.0f, 1.0f, nullptr, 1.0f, 1.0f);
    if(*selected != -1) C2D_DrawImageAt(keypress, 2.0f + keys[*selected].x, 67.0f + keys[*selected].y, 1.0f, nullptr, (keys[*selected].width / 20.0f), 1.0f);
    for(int i = 0; i < NUM_KEYS; i++)
    {
        if(*capslock)
            C2D_TextFontParse(&buttons[i], ui.bottom_font, ui.bot_text_buf, keys[i].special.c_str());
        else
            C2D_TextFontParse(&buttons[i], ui.bottom_font, ui.bot_text_buf, keys[i].normal.c_str()); 
	    C2D_TextOptimize(&buttons[i]);
        float width, height;
        C2D_TextGetDimensions(&buttons[i], 0.5f, 0.5f, &width, &height);
        C2D_DrawText(&buttons[i], C2D_WithColor, 5.0f + keys[i].x, 67.0f + keys[i].y, 1.0f, 1.0f, 1.0f, C2D_Color32(0x0, 0x00, 0x00, 0xFF));
    }
    LightLock_Unlock(lock);
}

int keyboard::append_to_input(keyboard *keyb, int index)
{
    key_s key = keyb->keys[index];
    std::string character = keyb->do_capslock ? key.special : key.normal;

    if(character == "Bsp")
    {
        if(!keyb->input.empty())
        {
            keyb->input.pop_back();
            keyb->callback('\b');
        }
    }
    else if(character == "Tb")
    {
        keyb->callback('\t');
    }

    else if(character == "Entr")
    {   
        
        if(!keyb->echo)
            keyb->input.append("\n");
        return 1;
    }

    else if(character == "Space")
    {
        keyb->callback(' ');
        keyb->input.append(" ");
    }
    else if(character == "Cps" || character == "Shft")
        keyb->do_capslock = !keyb->do_capslock;
    
    else
    {
        
        if(keyb->echo)
            keyb->callback(character[0]);
        
        keyb->input.append(character);
    }
    return 0;  
}

void keyboard::gen_key_table(std::array<key_s, NUM_KEYS> &keys)
{
    keys = {{
        {"`", "~",   1, 1, 20, 20},
        {"1", "!",  22, 1, 20, 20},
        {"2", "@",  43, 1, 20, 20},
        {"3", "#",  64, 1, 20, 20},
        {"4", "$",  85, 1, 20, 20},
        {"5", "%", 106, 1, 20, 20},
        {"6", "^", 127, 1, 20, 20},
        {"7", "&", 148, 1, 20, 20},
        {"8", "*", 169, 1, 20, 20},
        {"9", "(", 190, 1, 20, 20},
        {"0", ")", 211, 1, 20, 20},
        {"-", "_", 232, 1, 20, 20},
        {"+", "=", 253, 1, 20, 20},
        {"Bsp", "Bsp", 274, 1, 40, 20},

        {"Tb", "Tb",  1, 22, 31, 20},
        {"q", "Q",   32, 22, 20, 20},
        {"w", "W",   53, 22, 20, 20},
        {"e", "E",   74, 22, 20, 20},
        {"r", "R",   95, 22, 20, 20},
        {"t", "T",  116, 22, 20, 20},
        {"y", "Y",  137, 22, 20, 20},
        {"u", "U",  158, 22, 20, 20},
        {"i", "I",  179, 22, 20, 20},
        {"o", "O",  200, 22, 20, 20},
        {"p", "P",  220, 22, 20, 20},
        {"[", "{",  242, 22, 20, 20},
        {"]", "}",  263, 22, 20, 20},
        {"\\", "|", 284, 22, 20, 20},

        {"Cps", "Cps", 1, 43, 34, 20},
        {"a", "A", 37, 43, 20, 20},
        {"s", "S", 58, 43, 20, 20},
        {"d", "D", 79, 43, 20, 20},
        {"f", "F", 100, 43, 20, 20},
        {"g", "G", 120, 43, 20, 20},
        {"h", "H", 142, 43, 20, 20},
        {"j", "J", 163, 43, 20, 20},
        {"k", "K", 184, 43, 20, 20},
        {"l", "L", 205, 43, 20, 20},
        {";", ":", 226, 43, 20, 20},
        {"'", "\"",247,43, 20, 20},
        {"Entr", "Entr",  268, 43, 47, 20},

        {"Shft", "Shft", 1, 64, 47, 20},
        {"z", "Z",  48, 64, 20, 20},
        {"x", "X",  69, 64, 20, 20},
        {"c", "C",  90, 64, 20, 20},
        {"v", "V", 111, 64, 20, 20},
        {"b", "B", 132, 64, 20, 20},
        {"n", "N", 153, 64, 20, 20},
        {"m", "M", 174, 64, 20, 20},
        {",", "<", 195, 64, 20, 20},
        {".", ">", 216, 64, 20, 20},
        {"/", "?", 237, 64, 20, 20},
        {"Shft", "Shft", 258, 64, 20, 20},

        {"@", "@",  1, 85, 94, 20},
        {"Space", "Space", 125, 85, 127, 20},
        {"", "", 222, 85, 94, 20}
    }};
}

void keyboard::hid_input_thread(void *data)
{
    keyboard *keyb = (keyboard *)data;
    keyb->input.clear();
    touchPosition pos;
    auto tm = osGetTime();
    bool running = true;
    while(running)
    {
        hidScanInput();
        if(osGetTime() - tm > 100)
            keyb->selected_key = -1;
        if(keysDown() & KEY_TOUCH)
        {
            LightLock_Lock(&keyb->lock);
            hidTouchRead(&pos);
            for(int i = 0; i < NUM_KEYS; i++)
            {
                float x1 = keyb->keys[i].x + 2.0f;
                float y1 = keyb->keys[i].y + 67.0f;
                float x2 = x1 + keyb->keys[i].width;
                float y2 = y1 + keyb->keys[i].height;
                
                if(pos.px >= x1 && pos.px <= x2 && pos.py >= y1 && pos.py <= y2)
                {
                    keyb->old_key = i;
                    keyb->selected_key = i;
                    if(keyb->append_to_input(keyb, i) == 1)
                        keyb->done = true;
                }
            }
            tm = osGetTime();
            LightLock_Unlock(&keyb->lock);
        }
    }
}

std::string keyboard::get_input()
{
    input.clear();
    touchPosition pos;
    auto tm = osGetTime();

    while(!done)
    {
        hidScanInput();
        if(osGetTime() - tm > 100)
            selected_key = -1;
        if(keysDown() & KEY_TOUCH)
        {
            hidTouchRead(&pos);
            for(int i = 0; i < NUM_KEYS; i++)
            {
                float x1 = keys[i].x + 2.0f;
                float y1 = keys[i].y + 67.0f;
                float x2 = x1 + keys[i].width;
                float y2 = y1 + keys[i].height;
                
                if(pos.px >= x1 && pos.px <= x2 && pos.py >= y1 && pos.py <= y2)
                {
                    old_key = i;
                    selected_key = i;
                    if(append_to_input(this, i) == 1)
                        done = true;
                }
            }
            tm = osGetTime();
        }
    }
    selected_key = -1;
    done = false;
    return std::move(input);
}

std::string keyboard::get_input_async()
{
    done = false;
    return std::move(input);
}

void keyboard::async()
{
    thread = threadCreate((ThreadFunc)&keyboard::hid_input_thread, this, 0x1000, 0x29, 1, true);
    svcSleepThread(1e+9);
}

keyboard::keyboard(std::function<void()> &func, std::function <void(char)> cb)
{
    LightLock_Init(&lock);
    C2D_SpriteSheet spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
    if(!spriteSheet) printf("Couldn't locate sprites\n");
    gen_key_table(keys);
    C2D_Image layout = C2D_SpriteSheetGetImage(spriteSheet, 4);
    C2D_Image keypress = C2D_SpriteSheetGetImage(spriteSheet, 5);
    func = std::bind(keyboard::print, layout, keypress, &lock, keys, &do_capslock, &selected_key);
    callback = cb;
}

keyboard::~keyboard()
{
    threadJoin(thread, U64_MAX);
}