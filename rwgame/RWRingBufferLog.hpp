#ifndef RWGAME_RWRINGBUFFERLOG_HPP
#define RWGAME_RWRINGBUFFERLOG_HPP

#include "RWRingBuffer.hpp"

#include <core/Logger.hpp>

#include <cstdlib>
#include <string>

class RWRingBufferLog final : public Logger::MessageReceiver, std::enable_shared_from_this<RWRingBufferLog> {
public:
struct Message {
    enum MessageLevel {
        VERBOSE = 0,
        INFO,
        WARNING,
        ERROR,
        INPUT,
        STDOUT,
        STDERR,
        _Count,
    };
    std::string text;
    MessageLevel level;
};
private:
static constexpr size_t N = 1024u;
RWRingBuffer<Message, N> _log;
public:
    bool updated = false;
    void messageReceived(const Logger::LogMessage& message) override;
    void input(const std::string txt);
    void toStdOut(const std::string txt);
    void toStdErr(const std::string txt);
    const RWRingBuffer<Message, N>& getRingBuffer() const;
    RWRingBuffer<Message, N>& getRingBuffer();
};

#endif
