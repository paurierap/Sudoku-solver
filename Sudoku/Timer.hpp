#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <chrono>
#include <string>

/* RAII class for timing events*/

namespace timer{
    class Timer
    {
        private:
            std::string _label;
            std::chrono::high_resolution_clock::time_point _start;
            bool _stopped = false;

        public:
            Timer(const std::string& label = "") : _label(label), _start(std::chrono::high_resolution_clock::now()) {}

            ~Timer()
            {
                if (!_stopped) stop();
            }

            void stop()
            {
                auto end = std::chrono::high_resolution_clock::now();
                auto micros = std::chrono::duration_cast<std::chrono::microseconds>(end - _start).count();

                std::cout << _label << " took ";

                if (micros < 1000) std::cout << micros << " \u03BCs"; 
                else std::cout << micros / 1000.0 << " ms";

                std::cout << "\n";

                _stopped = true;
            }
    };
}
#endif