#pragma once
#include <sstream>
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

struct Go {};
constexpr Go go;

namespace curious::log {

class logger_stream {
public:
    enum class Level { Info, Warn, Error, Debug };
    logger_stream(Level level, const char* file, int line, const char* func);

    template<typename T>
    logger_stream& operator<<(const T& val) {
        _stream << val;
        return *this;
    }

    template<typename T>
    logger_stream& operator<<(const std::optional<T>& opt) {
        if (opt.has_value())
            _stream << *opt;
        else
            _stream << "(nullopt)";
        return *this;
    }

    void operator<<(Go);

private:
    Level _level;
    std::ostringstream _stream;
    static const char* short_filename(const char* path);
};

// Logging macros with file, line, and function context
#define LOG_INFO  curious::log::logger_stream(curious::log::logger_stream::Level::Info,  __FILE__, __LINE__, __func__)
#define LOG_WARN  curious::log::logger_stream(curious::log::logger_stream::Level::Warn,  __FILE__, __LINE__, __func__)
#define LOG_ERR   curious::log::logger_stream(curious::log::logger_stream::Level::Error, __FILE__, __LINE__, __func__)
#define LOG_DBG   curious::log::logger_stream(curious::log::logger_stream::Level::Debug, __FILE__, __LINE__, __func__)

} // namespace curious::log