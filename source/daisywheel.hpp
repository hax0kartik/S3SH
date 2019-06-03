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
    daisywheelkbd(std::function<void()> &func);
    void hid_input();
    void set_input_string(std::string *_input) { input = _input; };
    private:
    std::vector<std::tuple<std::string, float, float>> buttons;
    bool is_capital = false;
    bool is_special = false;
    bool is_lrspecial = false;
    std::string *input;
    std::array<std::string, 36> current_key_layout;
    static void print(std::vector<std::tuple<std::string, float, float>> *button_vec, int *selected, std::vector<C2D_Image>, LightLock* lock);
    void cal_buttons(std::array<std::string, 36> str);
    bool checkCoordinates(int radius, s16 x, s16 y, float startAngle, float endAngle);
    int checkForSpecialInput(std::string str, std::string *input);
   // std::vector<std::string alphabets = "bac8edf9hgi6kjl3nmo2qpr1tsu4wvx7zy05";
    int selected = 8;
    LightLock lock;
};