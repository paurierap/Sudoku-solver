#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <chrono>
#include <string>

/* RAII class for timing events*/

class Timer
{
public:
    Timer(const std::string& label = "") : _label(label), _start(std::chrono::high_resolution_clock::now()) {}

    ~Timer()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(end - _start).count();

        std::cout << _label << " took ";

        if (micros < 1000) std::cout << micros << " \u03BCs"; // microseconds
        else std::cout << micros / 1000.0 << " ms"; // milliseconds

        std::cout << std::endl;
    }

private:
    std::string _label;
    std::chrono::high_resolution_clock::time_point _start;
};

#endif