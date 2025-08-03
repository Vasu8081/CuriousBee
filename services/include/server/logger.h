#pragma once
#include <sstream>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

struct Go {};
constexpr Go go;

namespace curious::log {

class LoggerStream {
public:
    enum class Level { Info, Warn, Error, Debug };

    LoggerStream(Level level, const char* file, int line, const char* func)
        : _level(level) {
        _stream << "[" << short_filename(file) << ":" << line << " " << func << "] ";
    }

    template<typename T>
    LoggerStream& operator<<(const T& val) {
        _stream << val;
        return *this;
    }

    void operator<<(Go) {
        const std::string message = _stream.str();
        switch (_level) {
            case Level::Info:  spdlog::info(message); break;
            case Level::Warn:  spdlog::warn(message); break;
            case Level::Error: spdlog::error(message); break;
            case Level::Debug: spdlog::debug(message); break;
        }
    }

private:
    Level _level;
    std::ostringstream _stream;

    static const char* short_filename(const char* path) {
        const char* file = strrchr(path, '/');
#ifdef _WIN32
        const char* backslash = strrchr(path, '\\');
        if (!file || (backslash && backslash > file))
            file = backslash;
#endif
        return file ? file + 1 : path;
    }
};

// Logging macros with file, line, and function context
#define LOG_INFO  curious::log::LoggerStream(curious::log::LoggerStream::Level::Info,  __FILE__, __LINE__, __func__)
#define LOG_WARN  curious::log::LoggerStream(curious::log::LoggerStream::Level::Warn,  __FILE__, __LINE__, __func__)
#define LOG_ERR   curious::log::LoggerStream(curious::log::LoggerStream::Level::Error, __FILE__, __LINE__, __func__)
#define LOG_DBG   curious::log::LoggerStream(curious::log::LoggerStream::Level::Debug, __FILE__, __LINE__, __func__)

} // namespace curious::log
