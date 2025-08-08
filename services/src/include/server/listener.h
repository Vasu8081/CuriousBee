#pragma once

#include <memory>

/**
 * @brief Base interface for handling network events.
 * 
 * All methods are pure virtual. Classes inheriting from `listener`
 * must override all three methods to handle request/response/message types.
 */
namespace curious::net {
    class network_message; // Forward declaration
}
class listener {
public:
    virtual ~listener() = default;

    /// Called when a request message is received.
    virtual void on_request(std::shared_ptr<curious::net::network_message> req) = 0;

    /// Called when a response to a previous request is received.
    virtual void on_reply(std::shared_ptr<curious::net::network_message> resp) = 0;

    /// Called when a published message is received.
    virtual void on_message(std::shared_ptr<curious::net::network_message> msg) = 0;
};
