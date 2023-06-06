/*
Thang Phu
Particle Measuring System

A simple uberlog logging benchmark module
Example Usage:
    ./main 100 10 n
    arg[1]: time in nanoseconds
    arg[2]: repeat time
    arg[3]: Optional. Configure to initialize the logger. '0', 'n', 'f' are all considered false
            and will not initlize the logger. Otherwise initialize the logger
*/
#define _MULTI_THREADED
#include <stdio.h>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include "uberlog/uberlog.h"
#include <sys/types.h>
#include <unistd.h>
#include "log_wrapper.h"
#include <pthread.h>

#define LOOPS 1000
#define SLEEP_BETWEEN_RUN_MS 1000

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::nanoseconds nanoseconds;

// uberlog::Logger logger;

// void init_logger() {
//     lw.get_logger()->SetArchiveSettings(50 * 1024 * 1024, 3);    // 3 file history, 50 MB each
//     lw.get_logger()->SetLevel(uberlog::Level::Debug);             // emit logs of level Debug or higher
//     lw.get_logger()->Open("tmp/uberlog0");
//     // logger.OpenStdOut();
// }

// sleep_ns is in nanoseconds
// average output is in microseconds (to reduce possibility of overflow)
double average_burst(uint32_t sleep_ns) {
    long times[LOOPS];  // in nanoseconds
    double average = 0; // in microseconds
    Clock::time_point t0 = Clock::now();
    Clock::time_point t1;
    for (int i = 0; i < LOOPS; ++i) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_ns));
        t1 = Clock::now();
        std::chrono::duration<long, std::nano> delta = std::chrono::duration_cast<nanoseconds>(t1 - t0);
        t0 = t1;
        long duration = delta.count();
        average += duration / (1000.0 * LOOPS);
    }
    return average;
}

// sleep_ns is in nanoseconds
// average output is in microseconds (to reduce possibility of overflow)
double average_burst_log(uint32_t sleep_ns) {
    double average = 0; // in microseconds
    Clock::time_point t0 = Clock::now();
    Clock::time_point t1;
    for (int i = 0; i < LOOPS; ++i) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_ns));
        t1 = Clock::now();
        std::chrono::duration<long, std::nano> delta = std::chrono::duration_cast<nanoseconds>(t1 - t0);
        t0 = t1;
        long duration = delta.count();
        average += duration / (1000.0 * LOOPS);
        lw.get_logger()->Info("Time delta was %ld", duration);
    }
    return average;
}

typedef struct {
    uint32_t sleep_ns;
    uint32_t repeat;
} thread_parm_t;

void* run_benchmark_test(void *parm) {
    printf("===== [%u] Running benchmark tests =====\n", std::hash<std::thread::id>{}(std::this_thread::get_id()));
    
    thread_parm_t * param = (thread_parm_t*) parm;
    uint32_t sleep_ns = param->sleep_ns;
    uint32_t repeat = param->repeat;

    double burst_us_results[repeat];
    double burst_log_us_results[repeat];
    double average_burst_us = 0;
    double average_burst_log_us = 0;
    double output_us = 0;

    for (int i = 0; i < repeat; ++i) {
        output_us = average_burst(sleep_ns);
        burst_us_results[i] = output_us; 
        average_burst_us += output_us / repeat;
        output_us = average_burst_log(sleep_ns);
        burst_log_us_results[i] = output_us;
        average_burst_log_us += output_us / repeat;
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_BETWEEN_RUN_MS));
    }

    printf("Operational results: %.3lf", burst_us_results[0]);
    for (int i = 1; i < repeat; ++i) {
        printf(", %.3lf", burst_us_results[i]);
    }
    printf("\n");

    printf("Operational with logging results: %.3lf", burst_log_us_results[0]);
    for (int i = 1; i < repeat; ++i) {
        printf(", %.3lf", burst_log_us_results[i]);
    }
    printf("\n");

    printf("Average operational time: %.3lf us\n", average_burst_us);
    printf("Average operational time with logging: %.3lf us\n", average_burst_log_us);
    printf("Average time difference: %.3lf us\n", average_burst_log_us - average_burst_us);

    pthread_exit(NULL);
}

int main(int argc, char** argv) {  
    uint32_t sleep_ns = 1;
    uint32_t repeat = 1;
    bool initialize_logger = true;

    if (argc == 3 || argc == 4) {
        sleep_ns = (uint32_t) strtoul(argv[1], nullptr, 10);
        repeat = (uint32_t) strtoul(argv[2], nullptr, 10);
        if (argc == 4) {
            std::string config_logger(argv[3]);
            if (config_logger.at(0) == '0' || config_logger.at(0) == 'n' || config_logger.at(0) == 'f') {
                initialize_logger = false;
            }
        }
        printf("Setting sleep_ns = %u ns, repeat = %u\n", sleep_ns, repeat);
    } else {
        printf("Running tests with default sleep time = 1 ns, repeat = 1\n");
    }

    // pid_t pid = getpid();

    // if (initialize_logger) {
    //     // init_logger();
    //     printf("pid %d Logger has been initialized\n", pid);
    // } else {
    //     printf("pid %d Skipping logger initialize\n", pid);
    // }

    // std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_BETWEEN_RUN_MS));
    // lw.get_logger()->Debug("----- pid [%lu] starting up -----", pid);
    // lw.get_logger()->Info("pid [%lu] Setting sleep time = %ld ns, repeat = %ld\n", pid, sleep_ns, repeat);
    
    // run_benchmark_test(sleep_ns, repeat);

    // std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_BETWEEN_RUN_MS * 10));
    int i = 0;
    int err;

    pthread_t tid[10];
    thread_parm_t *parm = (thread_parm_t*) malloc(sizeof(thread_parm_t));
    parm->sleep_ns = sleep_ns;
    parm->repeat = repeat;

    while(i < 10)
    {
        printf("Creating thread: %d", i);
        err = pthread_create(&(tid[i]), NULL, &run_benchmark_test, (void *) parm);
        if (err != 0) {
            printf("Can't create thread\n");
        } else {
            printf("Thread created successfully\n");
        }
        i++;
    }
    free(parm);
} 