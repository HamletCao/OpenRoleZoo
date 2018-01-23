//
// Created by lby on 2018/1/23.
//

#include "orz/tools/progress_bar.h"

namespace orz {


    progress_bar::microseconds progress_bar::used_time() const {
        using std::chrono::duration_cast;
        switch (m_stat)
        {
            default:
                return microseconds(0);
            case WAITING:
                return microseconds(0);
            case RUNNING:
                return duration_cast<microseconds>(system_clock::now() - m_start_time_point) - m_paused_duration;
            case PAUSED:
                return duration_cast<microseconds>(m_pause_time_point - m_start_time_point) - m_paused_duration;
            case STOPPED:
                return duration_cast<microseconds>(m_stop_time_point - m_start_time_point) - m_paused_duration;
        }
    }

    progress_bar::progress_bar(int min, int max, int value)
        : m_min(min), m_max(max), m_value(value), m_paused_duration(0){}

    progress_bar::progress_bar(int min, int max) : progress_bar(min, max, min){}

    progress_bar::progress_bar(int max) : progress_bar(0, max, 0) {}

    progress_bar::progress_bar() : progress_bar(0, 100, 0) {}

    void progress_bar::start() {
        using std::chrono::duration_cast;
        switch (m_stat)
        {
            default:
                m_start_time_point = system_clock::now();
                break;
            case WAITING:
                m_start_time_point = system_clock::now();
            case RUNNING:
                break;
            case PAUSED:
                m_paused_duration += duration_cast<microseconds>(system_clock::now() - m_pause_time_point);
                break;
            case STOPPED:
                m_start_time_point = system_clock::now();
        }
        m_stat = RUNNING;
    }
}
