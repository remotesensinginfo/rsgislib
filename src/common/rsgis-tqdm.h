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

#if _MSC_VER
    #include <io.h>
    #define isatty _isatty
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
#include <math.h>
#include <algorithm>

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
            int width = 40;
    
            std::string right_pad = "▏";
            std::string label = "";
    
            void hsv_to_rgb(float h, float s, float v, int& r, int& g, int& b);
    };
}
#endif
