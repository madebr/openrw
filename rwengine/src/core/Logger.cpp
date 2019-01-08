#include <core/Logger.hpp>

#include <algorithm>
#include <iostream>
#include <map>

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

void Logger::removeReceiver(std::shared_ptr<Logger::MessageReceiver> out) {
    receivers.erase(std::remove(receivers.begin(), receivers.end(), out),
                    receivers.end());
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

std::map<Logger::MessageSeverity, char> severityStr{{Logger::Error, 'E'},
                                                    {Logger::Warning, 'W'},
                                                    {Logger::Info, 'I'},
                                                    {Logger::Verbose, 'V'}};

void StdOutReceiver::messageReceived(const Logger::LogMessage& message) {
    std::cout << severityStr[message.severity] << " [" << message.component
              << "] " << message.message << std::endl;
}
