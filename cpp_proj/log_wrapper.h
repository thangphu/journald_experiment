#include "uberlog/uberlog.h"

class Log_Wrapper {
    uberlog::Logger logger;
    uberlog::Logger *logger_ptr;

public: 
    Log_Wrapper() {
        logger.SetArchiveSettings(50 * 1024 * 1024, 3);    // 3 file history, 50 MB each
        logger.SetLevel(uberlog::Level::Debug);             // emit logs of level Debug or higher
        logger.Open("tmp/uberlog0");
        logger_ptr = &logger;
    }

    ~Log_Wrapper() = default;

    Log_Wrapper(Log_Wrapper &other) {
        logger_ptr=other.logger_ptr;
    }

    uberlog::Logger* get_logger() {
        return logger_ptr;
    }
};

extern Log_Wrapper lw;