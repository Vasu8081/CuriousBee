#include <base/logger.h>

namespace curious::log {

logger_stream::logger_stream(Level level, const char* file, int line, const char* func)
    : _level(level) {
    _stream << "[" << short_filename(file) << ":" << line << " " << func << "] ";
}

void logger_stream::operator<<(Go) {
    const std::string message = _stream.str();
    switch (_level) {
        case Level::Info:  spdlog::info(message); break;
        case Level::Warn:  spdlog::warn(message); break;
        case Level::Error: spdlog::error(message); break;
        case Level::Debug: spdlog::debug(message); break;
    }
}

const char* logger_stream::short_filename(const char* path) {
    const char* file = strrchr(path, '/');
#ifdef _WIN32
    const char* backslash = strrchr(path, '\\');
    if (!file || (backslash && backslash > file))
        file = backslash;
#endif
    return file ? file + 1 : path;
}

} // namespace curious::log