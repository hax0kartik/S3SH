#include "daisywheel.hpp"
#include "kbd_layout.hpp"
#define SCREEN_WIDTH 400
// character
// float x
// float y
void daisywheelkbd::print(std::vector<std::tuple<std::string, float, float>> *buttons_vec, int *selected, std::vector<C2D_Image> images, LightLock *lock)
{
    LightLock_Lock(lock);
    auto button_vec = *buttons_vec;
    C2D_TextBufClear(ui.bot_text_buf);
    static C2D_Text *buttons = (C2D_Text *)malloc(button_vec.size() * sizeof(C2D_Text));
    auto button = button_vec[*selected * 4];
    C2D_DrawImageAt(images[0], std::get<1>(button) + 7.5f + 0.6f, std::get<2>(button) - 7.5f - 0.6f, 0.6f, nullptr, 0.3f, 0.3f);
    button = button_vec[*selected * 4  + 1];
    C2D_DrawImageAt(images[3], std::get<1>(button) + 7.5f + 0.6f, std::get<2>(button) - 7.5f - 0.6f, 0.6f, nullptr, 0.3f, 0.3f);
    button = button_vec[(*selected * 4) + 2];
    C2D_DrawImageAt(images[2], std::get<1>(button) - 36.5f - 0.6f, std::get<2>(button) - 22.0f - 0.6f, 0.6f, nullptr, 0.3f, 0.3f);
    button = button_vec[*selected * 4 + 3];
    C2D_DrawImageAt(images[1], std::get<1>(button) - 7.5f - 0.6f, std::get<2>(button) - 51.0f - 0.6f, 0.6f, nullptr, 0.3f, 0.3f);

    for(int i = 0; i < button_vec.size(); i++)
    {
        std::tuple<std::string, float, float> button = button_vec[i];
        C2D_TextFontParse(&buttons[i], ui.bottom_font, ui.bot_text_buf, std::get<0>(button).c_str());
	    C2D_TextOptimize(&buttons[i]);
        float width, height;
        C2D_TextGetDimensions(&buttons[i], 0.5f, 0.5f, &width, &height);
	    C2D_DrawText(&buttons[i], C2D_WithColor | C2D_AtBaseline, std::get<1>(button) - (width/2), std::get<2>(button) - (height/2), 1.0f, 1.0f, 1.0f, C2D_Color32(0xA0, 0xA0, 0xA0, 0xFF));
    }
    C2D_DrawImageAt(images[0], std::get<1>(button) + 3.75f, std::get<2>(button) + 3.75f, 0.6f, nullptr, 0.3f, 0.3f);
    C2D_DrawCircleSolid(std::get<1>(button), std::get<2>(button) + 15.0f, 1.0f, 30.0f, 0xFF000030);
    LightLock_Unlock(lock);
}

void daisywheelkbd::cal_buttons(daisywheelkbd *kbd, std::array<std::string, 36> str)
{
    kbd->buttons.clear();
    auto circum_coord = [](float angle, float radius, std::pair<float, float> center_coord) -> std::pair<float, float>{
        float x = (radius * sin((angle * 3.14) / 180)) + std::get<0>(center_coord);
        float y = (radius * -cos((angle * 3.14) / 180)) + std::get<1>(center_coord);
        //printf("X: %f Y: %f\n", x, y);
        return std::make_pair(x, y);
    };

    auto center_coords = std::make_pair(160.0f, 120.0f);
    for(int i = 0, angle = 0; i < 9; i++, angle += 45)
    {   
        auto [x, y] = (angle == 360) ? center_coords : circum_coord(angle, 90.0f, center_coords);
        kbd->buttons.push_back(std::make_tuple(str[(i * 4)], x, y));
        kbd->buttons.push_back(std::make_tuple(str[(i * 4) + 1], x - 15, y + 15));
        kbd->buttons.push_back(std::make_tuple(str[(i * 4) + 2], x + 15, y + 15));
        kbd->buttons.push_back(std::make_tuple(str[(i * 4) + 3], x,  y + 30));
    }
    
    //buttons.push_back(std::make_tuple(str.substr((8 * 4) + 1, 1), 200 - 15, 100 + 15));
    //buttons.push_back(std::make_tuple(str.substr((8 * 4) + 2, 1), 200 + 15, 100 + 15));
    //buttons.push_back(std::make_tuple(str.substr((8 * 4) + 3, 1), 200,  100 + 30));
}

bool daisywheelkbd::checkCoordinates(int radius, s16 x, s16 y, float startAngle, float endAngle)
{ 
    // Calculate polar co-ordinates 
    if (x == 0 && y == 0) return false;
    float polarradius = sqrt(x*x+y*y); 
    float Angle = atan2(y, x) * 180/3.14;
    if (Angle>=startAngle && Angle<=endAngle && polarradius<radius) 
        return true;
    else
        return false;
}

std::string daisywheelkbd::get_input_async()
{
    done = false;
    return std::move(input);
}

daisywheelkbd::daisywheelkbd(std::function<void()> &func, std::function<void(char)> cb)
{
    current_key_layout = normal;
    std::vector<C2D_Image> images;

    LightLock_Init(&lock);
    C2D_SpriteSheet spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
    if(!spriteSheet) printf("Couldn't locate sprites\n");
    for(int i = 0; i < 4; i++)
        images.push_back(C2D_SpriteSheetGetImage(spriteSheet, i));
    
    cal_buttons(this, normal);
    func = std::bind(daisywheelkbd::print, &buttons, &selected, images, &lock);
    callback = cb;
}


void daisywheelkbd::async()
{
    thread = threadCreate((ThreadFunc)&daisywheelkbd::hid_input_thread, this, 0x1000, 0x29, 1, true);
    svcSleepThread(1e+9);
}

void daisywheelkbd::hid_input_thread(void *data)
{
    daisywheelkbd *keyb = (daisywheelkbd *)data;
    keyb->input.clear();
    circlePosition pos;
    auto tm = osGetTime();
    bool running = true;
    while(running)
    {
        hidScanInput();
        hidCircleRead(&pos);

        if(keysDown() & KEY_START)
        {
            LightLock_Lock(&keyb->lock);
            keyb->current_key_layout = lr_specials;
            cal_buttons(keyb, keyb->current_key_layout);
            LightLock_Unlock(&keyb->lock);
        }
        
        else if(keysDown() & KEY_L)
        {
            LightLock_Lock(&keyb->lock);
            keyb->current_key_layout = capitals;
            cal_buttons(keyb, keyb->current_key_layout);
            LightLock_Unlock(&keyb->lock);
        }

        else if(keysDown() & KEY_R)
        {
            LightLock_Lock(&keyb->lock);
            keyb->current_key_layout = specials;
            cal_buttons(keyb, keyb->current_key_layout);
            LightLock_Unlock(&keyb->lock);
        }

        else if(keysDown() & KEY_X && osGetTime() - tm > 100)
        {
            tm = osGetTime();
            if(keyb->checkForSpecialInput(keyb->current_key_layout[keyb->selected * 4], keyb) == 1)
                keyb->done = true;
        }

        else if(keysDown() & KEY_Y && osGetTime() - tm > 100)
        {
            tm = osGetTime();
            if(keyb->checkForSpecialInput(keyb->current_key_layout[keyb->selected * 4 + 1], keyb) == 1)
                keyb->done = true;
        }
        
        else if(keysDown() & KEY_A && osGetTime() - tm > 100)
        {
            tm = osGetTime();
            if(checkForSpecialInput(keyb->current_key_layout[keyb->selected * 4 + 2], keyb) == 1)
                keyb->done = true;
        }
        
        else if(keysDown() & KEY_B && osGetTime() - tm > 100)
        {
            tm = osGetTime();
            if(checkForSpecialInput(keyb->current_key_layout[keyb->selected * 4 + 3], keyb) == 1)
                keyb->done = true;
        }
        
        else if(keysUp() & KEY_L || keysUp() & KEY_R || keysUp() & KEY_START)
        {
            LightLock_Lock(&keyb->lock);
            if(keyb->current_key_layout != normal)
            {
                keyb->current_key_layout = normal;
                cal_buttons(keyb, normal);
            }
            LightLock_Unlock(&keyb->lock);
        }

        if(abs(pos.dx) < 20 && abs(pos.dy) < 20)
            keyb->selected = 8;
        
        else if (keyb->checkCoordinates(0xFF64, pos.dx, pos.dy, 0 - 27.5f, 0 + 27.5f))           
            keyb->selected = 2;

        else if (keyb->checkCoordinates(0xFF64, pos.dx, pos.dy, 45 - 27.5f, 45 + 27.5f))           
            keyb->selected = 1;
        
        else if (keyb->checkCoordinates(0xFF64, pos.dx, pos.dy, 90 - 27.5f, 90 + 27.5f))           
            keyb->selected = 0;
        
        else if (keyb->checkCoordinates(0xFF64, pos.dx, pos.dy, 135 - 27.5f, 135 + 27.5f))           
            keyb->selected = 7;

        else if (keyb->checkCoordinates(0xFF64, pos.dx, pos.dy, 180 - 27.5f, 180 + 27.5f))           
            keyb->selected = 6;

        else if (keyb->checkCoordinates(0xFF64, pos.dx, pos.dy, -135 - 27.5f, -135 + 27.5f))           
            keyb->selected = 5;

        else if (keyb->checkCoordinates(0xFF64, pos.dx, pos.dy, -90 - 27.5f, -90 + 27.5f))           
            keyb->selected = 4;

        else if (keyb->checkCoordinates(0xFF64, pos.dx, pos.dy, -45 - 27.5f, -45 + 27.5f))           
            keyb->selected = 3;
        
               // printf("pos x: y: %d %d\n", abs(pos.dx),
    }
    
}

std::string daisywheelkbd::get_input()
{
    input.clear();
    circlePosition pos;
    auto tm = osGetTime();

    while(!done)
    {
        hidScanInput();
        hidCircleRead(&pos);

        if(keysDown() & KEY_START)
        {
            LightLock_Lock(&lock);
            current_key_layout = lr_specials;
            cal_buttons(this, current_key_layout);
            LightLock_Unlock(&lock);
        }
        
        else if(keysDown() & KEY_L)
        {
            LightLock_Lock(&lock);
            current_key_layout = capitals;
            cal_buttons(this, current_key_layout);
            LightLock_Unlock(&lock);
        }

        else if(keysDown() & KEY_R)
        {
            LightLock_Lock(&lock);
            current_key_layout = specials;
            cal_buttons(this, current_key_layout);
            LightLock_Unlock(&lock);
        }

        else if(keysDown() & KEY_X && osGetTime() - tm > 100)
        {
            tm = osGetTime();
            if(checkForSpecialInput(current_key_layout[selected * 4], this) == 1)
                done = true;
        }

        else if(keysDown() & KEY_Y && osGetTime() - tm > 100)
        {
            tm = osGetTime();
            if(checkForSpecialInput(current_key_layout[selected * 4 + 1], this) == 1)
                done = true;
        }
        
        else if(keysDown() & KEY_A && osGetTime() - tm > 100)
        {
            tm = osGetTime();
            if(checkForSpecialInput(current_key_layout[selected * 4 + 2], this) == 1)
                done = true;
        }
        
        else if(keysDown() & KEY_B && osGetTime() - tm > 100)
        {
            tm = osGetTime();
            if(checkForSpecialInput(current_key_layout[selected * 4 + 3], this) == 1)
                done = true;
        }
        
        else if(keysUp() & KEY_L || keysUp() & KEY_R || keysUp() & KEY_START)
        {
            LightLock_Lock(&lock);
            if(current_key_layout != normal)
            {
                current_key_layout = normal;
                cal_buttons(this, normal);
            }
            LightLock_Unlock(&lock);
        }

        if(abs(pos.dx) < 20 && abs(pos.dy) < 20)
            selected = 8;
        
        else if (checkCoordinates(0xFF64, pos.dx, pos.dy, 0 - 27.5f, 0 + 27.5f))           
            selected = 2;

        else if (checkCoordinates(0xFF64, pos.dx, pos.dy, 45 - 27.5f, 45 + 27.5f))           
            selected = 1;
        
        else if (checkCoordinates(0xFF64, pos.dx, pos.dy, 90 - 27.5f, 90 + 27.5f))           
            selected = 0;
        
        else if (checkCoordinates(0xFF64, pos.dx, pos.dy, 135 - 27.5f, 135 + 27.5f))           
            selected = 7;

        else if (checkCoordinates(0xFF64, pos.dx, pos.dy, 180 - 27.5f, 180 + 27.5f))           
            selected = 6;

        else if (checkCoordinates(0xFF64, pos.dx, pos.dy, -135 - 27.5f, -135 + 27.5f))           
            selected = 5;

        else if (checkCoordinates(0xFF64, pos.dx, pos.dy, -90 - 27.5f, -90 + 27.5f))           
            selected = 4;

        else if (checkCoordinates(0xFF64, pos.dx, pos.dy, -45 - 27.5f, -45 + 27.5f))           
            selected = 3;
        
               // printf("pos x: y: %d %d\n", abs(pos.dx),
    }
        /******
         * Input stuff
         ******/             // printf("keysDown(): %x\n", keysHeld());(
    done = false;
    return std::move(input);
}

int daisywheelkbd::checkForSpecialInput(std::string character, daisywheelkbd *keyb)
{
    if(character == "Bsp")
    {
        if(!keyb->input.empty())
        {
            if(!keyb->echo)

            keyb->input.pop_back();
            keyb->callback('\b');
        }
    }
    else if(character == "Tab")
    {
        //keyb->callback('\t');
        char ch = '\t';
        keyb->input.append(1, '\x09');
    }

    else if(character == "Entr")
    {       
        if(!keyb->echo)
            keyb->input.append("\n");
        return 1;
    }

    else if(character == "Spc")
    {
        keyb->callback(' ');
        keyb->input.append(" ");
    }

    else if(character == "Esc")
    {
        //keyb->callback(0x1B); // Escape 
        keyb->input.append(1, '\x1b');
    }
    else
    {
        if(keyb->echo)
            keyb->callback(character[0]);
        keyb->input.append(character);
    }
    return 0;
}