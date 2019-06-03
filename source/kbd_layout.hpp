#pragma once
#include <array>
#include <string>
/** WARNING: Extra long file ahead **/
std::array<std::string, 4 * 9> normal = {
    "b", "a", "c", "8",
    "e", "d", "f", "9",
    "h", "g", "i", "6",
    "k", "j", "l", "3",
    "n", "m", "o", "2",
    "q", "p", "r", "1",
    "t", "s", "u", "4",
    "w", "v", "x", "7",
    "z", "y", "0", "5",
};

std::array<std::string, 36> capitals = {
    "B", "A", "C", "Up",
    "E", "D", "F", "PUp",
    "H", "G", "I", "Rght",
    "K", "J", "L", "PDown",
    "N", "M", "O", "Down",
    "Q", "P", "R", "End",
    "T", "S", "U", "Left",
    "W", "V", "X", "Home",
    "Z", "Y", "Entr", "Spc"
};

std::array<std::string, 36> specials = {
    "=", "-", "+", "!",
    "*", "(", ")", "@",
    "%", "/", "\\", "#",
    "^", "[", "]", "$",
    "|", "<", ">", "?",
    "_", "{", "}", "&",
    "~", "`", "'", "\"",
    "Tab", "_", ":", ";",
    "Del", "Bsp", ",", "."
};

std::array<std::string, 36> lr_specials = {
    "F2", "F1", "F3", "1",
    "F5", "F4", "F6", "2",
    "F8", "F7", "F9", "3",
    "F11", "F10", "F12", "4",
    "Slk", "Prt", "Pau", "5",
    "Win", "Ins", "Brk", "6",
    "SAp", "App", "nul", "7",
    "STb", "nul", "nul", "8",
    "SDe", "SBk", "0", "9"
};