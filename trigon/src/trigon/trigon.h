#pragma once
#include "core/std/std.h"


struct app_info_t {
	cstr_t		name;
	version_t	version;
};
extern app_info_t app_main();


class trigon_t{
public:


    int     __trigon_main();
    void    quit();
private:
    trigon_t();
    ~trigon_t();
    bool running = true;


public:
    trigon_t(const trigon_t&) = delete;
    trigon_t& operator=(const trigon_t&) = delete;
    trigon_t(trigon_t&&) = delete;
    trigon_t& operator=(trigon_t&&) = delete;

    static trigon_t& ref() {
        static trigon_t instance;
        return instance;
    }
};