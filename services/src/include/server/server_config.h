#pragma once
#include <string>
#include <vector>

enum class EndpointType {
    TCP,
    IPC,
    UNKNOWN
};

struct messaging_endpoint {
    std::string topic;
    std::string endpoint;
    EndpointType type = EndpointType::UNKNOWN;
};

class server_config {
public:
    explicit server_config(const std::string& configPath);
    ~server_config();

    std::string get_log_type() const;
    std::string get_log_file_path() const;
    std::string get_timestamp_format() const;
    const std::vector<messaging_endpoint>& get_messaging_endpoints() const;
    const messaging_endpoint get_endpoint_for_topic(const std::string& topic) const;


private:
    void _loadFromFile(const std::string& path);

    std::string _logType;
    std::string _logFilePath;
    std::string _timestampFormat;
    std::vector<messaging_endpoint> _messagingEndpoints;
};
