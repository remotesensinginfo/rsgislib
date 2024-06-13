/*
 *  rsgis-tqdm.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 7/4/2020.
 *
 *  This code has been imported and modified from https://github.com/tqdm/tqdm.cpp
 *
 */

#ifndef RSGIS_TQDM_H
#define RSGIS_TQDM_H

#define NOMINMAX

#if _MSC_VER
    #include <io.h>
    #define isatty _isatty
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include <chrono>
#include <ctime>
#include <numeric>
#include <ios>
#include <string>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#ifdef __linux__
    #include <sys/ioctl.h>
#endif

// mark all exported classes/functions with DllExport to have
// them exported by Visual Studio
#undef DllExport
#ifdef _MSC_VER
    #ifdef rsgis_commons_EXPORTS
        #define DllExport __declspec( dllexport )
    #else
        #define DllExport __declspec( dllimport )
    #endif
#else
    #define DllExport
#endif

namespace rsgis
{
    class DllExport rsgis_tqdm 
    {
        public:
            rsgis_tqdm();
            void reset();
            void set_theme_line();
            void set_theme_circle();
            void set_theme_braille();
            void set_theme_braille_spin();
            void set_theme_vertical();
            void set_theme_basic();
            void set_label(std::string label_);
            void enable_colors();
            void finish();
            void progress(int curr, int tot);
            ~rsgis_tqdm();

        private:
            // time, iteration counters and deques for rate calculations
            std::chrono::time_point<std::chrono::system_clock> t_first;
            std::chrono::time_point<std::chrono::system_clock> t_old;
            int n_old = 0;
            std::vector<double> deq_t;
            std::vector<int> deq_n;
            int nupdates = 0;
            int total_ = 0;
            int period = 1;
            unsigned int smoothing = 50;
            bool use_ema = true;
            float alpha_ema = 0.1;
    
            std::vector<const char*> bars;
    
            bool in_screen = false;
            bool in_tmux = false;
            bool is_tty = false;
            bool use_colors = false;
            bool color_transition = false;
            int width = []() {
#ifdef __linux__
              struct winsize win {};
              ioctl(0, TIOCGWINSZ, &win);
              unsigned short width = win.ws_col;
#elif _MSC_VER
              CONSOLE_SCREEN_BUFFER_INFO csbi;
              GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
              unsigned short width = csbi.srWindow.Right - csbi.srWindow.Left;
#else
              unsigned short width = 120;
#endif
              // return the space left for process bar
              // '60' is an experience value to exclude other output info, such as percent, time elapsed, etc.
              return (std::max)((int)width - 60, 1);
            }();
    
            std::string right_pad = "▏";
            std::string label = "";
    
            void hsv_to_rgb(float h, float s, float v, int& r, int& g, int& b);
    };
}
#endif
