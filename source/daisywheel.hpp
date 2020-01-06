#pragma once
#include <vector>
#include <tuple>
#include <functional>
#include <utility>
#include <algorithm>
#include <array>
#include <3ds.h>
#include <malloc.h>
#include <citro2d.h>
#include "ui.hpp"
extern uiFuncs_s ui;

class daisywheelkbd
{
    public:
    daisywheelkbd(std::function<void()> &func, std::function<void( char )> cb);
    void async();
    std::string get_input();
    std::string get_input_async();
    bool has_data() { return !input.empty(); };
    void disable_local_echo() { echo = false; };
    
    private:
    std::vector<std::tuple<std::string, float, float>> buttons;
    bool is_capital = false;
    bool is_special = false;
    bool is_lrspecial = false;
    bool done;
    std::string input;
    bool echo = true;
    std::function<void(char )> callback;
    std::array<std::string, 36> current_key_layout;
    static void print(std::vector<std::tuple<std::string, float, float>> *button_vec, int *selected, std::vector<C2D_Image>, LightLock* lock);
    static void cal_buttons(daisywheelkbd *kbd,std::array<std::string, 36> str);
    static bool checkCoordinates(int radius, s16 x, s16 y, float startAngle, float endAngle);
    static int checkForSpecialInput(std::string str, daisywheelkbd *kbd);
    static void hid_input_thread(void *data);
   // std::vector<std::string alphabets = "bac8edf9hgi6kjl3nmo2qpr1tsu4wvx7zy05";
    int selected = 8;
    LightLock lock;
    Thread thread;
};