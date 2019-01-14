#include <core/Logger.hpp>

#include <algorithm>
#include <array>
#include <iostream>

void Logger::log(const std::string& component, Logger::MessageSeverity severity,
                 const std::string& message) {
    LogMessage m{component, severity, message};

    for (auto& r : receivers) {
        r->messageReceived(m);
    }
}

void Logger::addReceiver(std::shared_ptr<Logger::MessageReceiver> out) {
    receivers.emplace_back(std::move(out));
}

void Logger::removeReceiver(const std::shared_ptr<Logger::MessageReceiver>& out) {
    receivers.erase(std::remove_if(receivers.begin(), receivers.end(), [&out](auto& v) {
        return out.get() == v.get();
    }), receivers.end());
}

void Logger::error(const std::string& component, const std::string& message) {
    log(component, Logger::Error, message);
}

void Logger::info(const std::string& component, const std::string& message) {
    log(component, Logger::Info, message);
}

void Logger::warning(const std::string& component, const std::string& message) {
    log(component, Logger::Warning, message);
}

void Logger::verbose(const std::string& component, const std::string& message) {
    log(component, Logger::Verbose, message);
}


void StdOutReceiver::messageReceived(const Logger::LogMessage& message) {
    std::cout << Logger::severityChar[message.severity] << " [" << message.component
              << "] " << message.message << std::endl;
}
