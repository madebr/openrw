#include <core/Logger.hpp>

#include <pybind11/pybind11.h>

namespace py = pybind11;

struct PyMessageReceiver : public Logger::MessageReceiver {
    void messageReceived(const Logger::LogMessage& message) override {
        PYBIND11_OVERLOAD_PURE_NAME(
                    void,
                    Logger::MessageReceiver,
                    "message_received",
                    messageReceived,
                    message
                    );
    }
};

void register_logging(py::module& parent) {
    py::module logging = parent.def_submodule("logging", "Logging submodule");

    py::class_<Logger::MessageReceiver, PyMessageReceiver> messageReceiver(logging, "MessageReceiver");
    messageReceiver
            .def(py::init<>())
            .def("message_received", &Logger::MessageReceiver::messageReceived, py::arg("message"));

    py::enum_<Logger::MessageSeverity>(logging, "MessageSeverity")
            .value("VERBOSE", Logger::MessageSeverity::Verbose)
            .value("INFO", Logger::MessageSeverity::Info)
            .value("WARNING", Logger::MessageSeverity::Warning)
            .value("ERROR", Logger::MessageSeverity::Error)
            .export_values();

    py::class_<Logger::LogMessage> logMessage(logging, "LogMessage");
    logMessage.def(py::init<std::string, Logger::MessageSeverity, std::string>(), py::arg("component"), py::arg("severity"), py::arg("message"))
            .def_readwrite("component", &Logger::LogMessage::component)
            .def_readwrite("severity", &Logger::LogMessage::severity)
            .def_readwrite("message", &Logger::LogMessage::message)
            .def("__repr__", [](const Logger::LogMessage& msg) {
                return "<LogMessage: component='" + msg.component + "' severity=" + "msg.severity" + " message='" + msg.message + "'>"; // TODO(madebr): replace msg.severity with char (refactor logging)
            }).def("__eq__", [](const Logger::LogMessage& msg1, const Logger::LogMessage& msg2) {
                return msg1.component == msg2.component
                        && msg1.severity == msg2.severity
                        && msg1.message == msg2.message;
            });

    py::class_<Logger> logger(logging, "Logger");
    logger.def(py::init<>())
            .def("add_receiver", &Logger::addReceiver, py::arg("receiver"))
            .def("remove_receiver", &Logger::removeReceiver, py::arg("receiver"))
            .def("log", &Logger::log, py::arg("component"), py::arg("severity"), py::arg("message"))
            .def("verbose", &Logger::verbose, py::arg("component"), py::arg("message"))
            .def("info", &Logger::info, py::arg("component"), py::arg("message"))
            .def("warning", &Logger::warning, py::arg("component"), py::arg("message"))
            .def("error", &Logger::error, py::arg("component"), py::arg("message"));
}
