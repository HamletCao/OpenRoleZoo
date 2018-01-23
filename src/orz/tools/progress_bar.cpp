//
// Created by lby on 2018/1/23.
//

#include "orz/tools/progress_bar.h"

namespace orz {

    int progress_bar::next() {
        m_value += m_step;
        if (m_value >= m_max && m_autostop) {
            self::stop();
            m_value = m_max;
        }
        return m_value;
    }

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
                m_paused_duration = microseconds(0);
                break;
            case RUNNING:
                break;
            case PAUSED:
                m_paused_duration += duration_cast<microseconds>(system_clock::now() - m_pause_time_point);
                break;
            case STOPPED:
                m_start_time_point = system_clock::now();
                m_paused_duration = microseconds(0);
                break;
        }
        m_stat = RUNNING;
    }

    void progress_bar::stop() {
        using std::chrono::duration_cast;
        switch (m_stat)
        {
            default:
                m_stop_time_point = system_clock::now();
                break;
            case WAITING:
                m_start_time_point = system_clock::now();
                m_stop_time_point = system_clock::now();
                break;
            case RUNNING:
                m_stop_time_point = system_clock::now();
                break;
            case PAUSED:
                m_paused_duration += duration_cast<microseconds>(system_clock::now() - m_pause_time_point);
                m_stop_time_point = system_clock::now();
                break;
            case STOPPED:
                break;
        }
        m_stat = STOPPED;
    }

    void progress_bar::pause() {
        using std::chrono::duration_cast;
        switch (m_stat)
        {
            default:
                m_pause_time_point = system_clock::now();
                break;
            case WAITING:
                m_start_time_point = system_clock::now();
                m_pause_time_point = system_clock::now();
                break;
            case RUNNING:
                m_pause_time_point = system_clock::now();
                break;
            case PAUSED:
                break;
            case STOPPED:
                break;
        }
        m_stat = PAUSED;
    }

    void progress_bar::autostop(bool flag) {
        m_autostop = flag;
    }

    bool progress_bar::autostop() const {
        return m_autostop;
    }

    int progress_bar::value() const {
        return m_value;
    }

    int progress_bar::max() const {
        return m_max;
    }

    int progress_bar::min() const {
        return m_min;
    }

    void progress_bar::set_value(int value) {
        m_value = value;
    }

    void progress_bar::set_min(int min) {
        m_min = min;
    }

    void progress_bar::set_max(int max) {
        m_max = max;
    }

    progress_bar::status progress_bar::stat() const {
        return m_stat;
    }


}
