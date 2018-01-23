//
// Created by lby on 2018/1/23.
//

#ifndef ORZ_TOOLS_PROGRESS_BAR_H
#define ORZ_TOOLS_PROGRESS_BAR_H

#include <chrono>

namespace orz {
    class progress_bar {
    public:
        using self = progress_bar;

        using microseconds = std::chrono::microseconds;
        using system_clock = std::chrono::system_clock;
        using time_point = decltype(system_clock::now());

        enum status
        {
            WAITING,
            RUNNING,
            PAUSED,
            STOPPED,
        };

        progress_bar(int min, int max, int value);
        progress_bar(int min, int max);
        progress_bar(int max);
        progress_bar();

        status stat() const;

        void start();
        void stop();
        void pause();

        void autostop(bool flag);
        bool autostop() const;

        int value() const;
        int max() const;
        int min() const;

        void set_value(int value);
        void set_min(int min);
        void set_max(int max);

        int next();

        microseconds used_time() const;
    private:
        int m_min;
        int m_max;
        int m_value;
        int m_step = 1;

        bool m_autostop = true;

        status m_stat = WAITING;

        time_point m_start_time_point;
        time_point m_stop_time_point;
        time_point m_pause_time_point;
        microseconds m_paused_duration;
    };
}



#endif //ORZ_TOOLS_PROGRESS_BAR_H
