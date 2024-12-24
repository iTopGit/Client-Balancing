#ifndef CUSTOM_TIMER_H
#define CUSTOM_TIMER_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

class CustomTimer
{
  public:
    CustomTimer(const std::string& name = "Timer")
        : name(name),
          elapsed_time(0.0)
    {
    }

    void start()
    {
        start_time = std::chrono::steady_clock::now();
    }

    void stop()
    {
        auto end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
        elapsed_time += elapsed.count();
    }

    void reset()
    {
        elapsed_time = 0.0;
    }

    double getElapsedTime() const
    {
        return elapsed_time;
    }

    void logElapsedTime() const
    {
        std::cout << std::fixed << std::setprecision(4) << name << " elapsed time: " << elapsed_time
                  << " ms." << std::endl;
    }

  private:
    std::string name;
    std::chrono::steady_clock::time_point start_time;
    double elapsed_time; // in milliseconds
};

#endif // CUSTOM_TIMER_H
