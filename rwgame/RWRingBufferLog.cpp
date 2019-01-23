#include "RWRingBufferLog.hpp"

#include <sstream>

static_assert(static_cast<int>(Logger::MessageSeverity::Verbose) == static_cast<int>(RWRingBufferLog::Message::MessageLevel::VERBOSE));
static_assert(static_cast<int>(Logger::MessageSeverity::Info)    == static_cast<int>(RWRingBufferLog::Message::MessageLevel::INFO));
static_assert(static_cast<int>(Logger::MessageSeverity::Warning) == static_cast<int>(RWRingBufferLog::Message::MessageLevel::WARNING));
static_assert(static_cast<int>(Logger::MessageSeverity::Error)   == static_cast<int>(RWRingBufferLog::Message::MessageLevel::ERROR));

void RWRingBufferLog::messageReceived(const Logger::LogMessage& message) {
    std::ostringstream oss;
    oss << Logger::severityChar[message.severity] << " [" << message.component << "] " << message.message;
    _log.add({oss.str(), RWRingBufferLog::Message::MessageLevel(message.severity)});
    updated = true;
}

void RWRingBufferLog::input(const std::string txt) {
    _log.add({std::move(txt), RWRingBufferLog::Message::MessageLevel::INPUT});
    updated = true;
}

void RWRingBufferLog::toStdOut(const std::string txt) {
    _log.add({std::move(txt), RWRingBufferLog::Message::MessageLevel::STDOUT});
    updated = true;
}

void RWRingBufferLog::toStdErr(const std::string txt) {
    _log.add({std::move(txt), RWRingBufferLog::Message::MessageLevel::STDERR});
    updated = true;
}

const RWRingBuffer<RWRingBufferLog::Message, RWRingBufferLog::N>& RWRingBufferLog::getRingBuffer() const {
    return _log;
}

RWRingBuffer<RWRingBufferLog::Message, RWRingBufferLog::N>& RWRingBufferLog::getRingBuffer() {
    return _log;
}
