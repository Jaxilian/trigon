#pragma once
#include "trigon/core/std/std.h"
#include "vkdef.h"
class window_t {
public:
     window_t();
    ~window_t();

private:

public:
    window_t(const window_t&) = delete;
    window_t& operator=(const window_t&) = delete;

    static window_t& main() {
        static window_t main_win;
        return main_win;
    }
};

class renderer_t {
public:

private:
     renderer_t() {};
    ~renderer_t() {};
public:
    renderer_t(const renderer_t&) = delete;
    renderer_t& operator=(const renderer_t&) = delete;

    static renderer_t& ref() {
        static renderer_t instance;
        return instance;
    }
};