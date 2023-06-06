/*
Thang Phu
Particle Measuring System

A simple journald logging benchmark module
Example Usage:
    ./main 100 10
    arg[1]: time in nanoseconds
    arg[2]: repeat time
*/

#include <stdio.h>
#include <string>
#include <chrono>
#include <thread>
#include <vector>
#include <systemd/sd-journal.h>

// #define LOG_ERROR 3
// #define LOG_INFO 6
// #define LOG_DEBUG 7

#define LOOPS 1000
#define SLEEP_BETWEEN_RUN_MS 1000

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::nanoseconds nanoseconds;

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
        times[i] = duration;
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
        sd_journal_print(LOG_INFO, "Time delta was %ld", duration);
    }
    return average;
}

void run_benchmark_test(uint32_t sleep_ns, uint32_t repeat) {
    // printf("===== Running benchmark tests =====\n");

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
}

int main(int argc, char** argv) {
    sd_journal_print(LOG_DEBUG, "----- starting up -----");

    uint32_t sleep_ns = 1;
    uint32_t repeat = 1;

    if (argc == 3) {
        sleep_ns = (uint32_t) strtoul(argv[1], nullptr, 10);
        repeat = (uint32_t) strtoul(argv[2], nullptr, 10);
        sd_journal_print(LOG_INFO, "Setting sleep_ns = %u ns, repeat = %u\n", sleep_ns, repeat);
    } else {
        sd_journal_print(LOG_INFO, "Running default test sleep_us = 1 ns, repeat = 1\n");
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_BETWEEN_RUN_MS));

    run_benchmark_test(sleep_ns, repeat);
}