#include <server/server_config.h>
#include <fstream>
#include <iostream>
#include <base/json.h>

server_config::server_config(const std::string& configPath) {
    _loadFromFile(configPath);
}

server_config::~server_config() = default;

std::string server_config::get_log_type() const {
    return _logType;
}

std::string server_config::get_log_file_path() const {
    return _logFilePath;
}

std::string server_config::get_timestamp_format() const {
    return _timestampFormat;
}

const std::vector<messaging_endpoint>& server_config::get_messaging_endpoints() const {
    return _messagingEndpoints;
}

const messaging_endpoint server_config::get_endpoint_for_topic(const std::string& topic) const{
        for (const auto& ep : _messagingEndpoints) {
            if (ep.topic == topic) {
                return ep;
            }
        }
        return {}; // Return an empty endpoint if not found
    }

void server_config::_loadFromFile(const std::string& path) {
    std::ifstream config_stream(path);
    if (!config_stream.is_open()) {
        throw std::runtime_error("Failed to open config file: " + path);
    }

    nlohmann::json config_json;
    try {
        config_stream >> config_json;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse config JSON: " + std::string(e.what()));
    }

    auto logging = config_json.value("logging", nlohmann::json::object());
    _logType = logging.value("type", "console");
    _logFilePath = logging.value("file_path", "app.log");
    _timestampFormat = logging.value("timestamp_format", "%Y-%m-%d %H:%M:%S");

    auto messaging = config_json.value("messaging", nlohmann::json::object());
    auto endpoints = messaging.value("endpoints", nlohmann::json::array());

    for (const auto& ep : endpoints) {
        messaging_endpoint me;
        me.topic = ep.value("topic", "");
        me.endpoint = ep.value("endpoint", "");
        me.type = EndpointType::UNKNOWN;
        if (ep.contains("type")) {
            std::string typeStr = ep["type"];
            if (typeStr == "TCP") {
                me.type = EndpointType::TCP;
            } else if (typeStr == "IPC") {
                me.type = EndpointType::IPC;
            }
        }
        // Only add valid endpoints
        if (!me.topic.empty() && !me.endpoint.empty()) {
            _messagingEndpoints.push_back(std::move(me));
        }
    }
}
