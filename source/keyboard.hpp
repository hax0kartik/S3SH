#pragma once
#include <string>
#include <functional>
#include <string>
#include <array>
#include <3ds.h>
#include <citro2d.h>
#include "ui.hpp"
extern uiFuncs_s ui;
typedef struct 
{
    std::string normal;
    std::string special;

    float x;
    float y;
    float width;
    float height;
}key_s;

constexpr int NUM_KEYS = 56;

class keyboard
{
    public:
        keyboard(std::function<void()> &func, std::function<void(char)> cb);
        ~keyboard();
        std::string get_input();
        bool has_data() { return done; };
    private:
        Thread thread;
        LightLock lock;
        std::array<key_s, NUM_KEYS> keys;
        std::string input;
        std::function<void(char )> callback;
        bool do_capslock;
        bool do_special;
        bool done;
        int selected_key = -1, old_key;
        
        static void print(C2D_Image layout, C2D_Image keypress, LightLock *lock, std::array<key_s, NUM_KEYS> keys, bool *capslock, int *selected);
        void gen_key_table(std::array<key_s, NUM_KEYS> &keys);
        static int append_to_input(keyboard *keyb, int index);
        static void hid_input_thread(void *data);
};