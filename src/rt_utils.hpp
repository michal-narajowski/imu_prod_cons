#pragma once
#include <string>
#include <stdexcept>
#include <sched.h>
#include <pthread.h>
#include <sys/mman.h>
#include <boost/log/trivial.hpp>

class RtUtils {
public:
    static void set_rt_priority(int priority) {
        struct sched_param param;
        param.sched_priority = priority;
        
        if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
            throw std::runtime_error("Failed to set RT scheduler: " + std::string(strerror(errno)));
        }

        // Lock memory to prevent paging
        if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
            BOOST_LOG_TRIVIAL(warning) << "Failed to lock memory: " << strerror(errno);
        }
    }

    static void verify_rt_privileges() {
        int min_priority = sched_get_priority_min(SCHED_FIFO);
        int max_priority = sched_get_priority_max(SCHED_FIFO);
        
        if (min_priority == -1 || max_priority == -1) {
            throw std::runtime_error("Failed to get RT priority range");
        }

        BOOST_LOG_TRIVIAL(info) << "RT priority range: " << min_priority << " - " << max_priority;
    }
};