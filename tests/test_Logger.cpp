#include <boost/test/unit_test.hpp>
#include <core/Logger.hpp>

class CallbackReceiver : public Logger::MessageReceiver, public std::enable_shared_from_this<CallbackReceiver> {
public:
    std::function<void(const Logger::LogMessage&)> func;

    CallbackReceiver(std::function<void(const Logger::LogMessage&)> func)
        : func(func) {
    }

    virtual void messageReceived(const Logger::LogMessage& message) {
        func(message);
    }
};

BOOST_AUTO_TEST_SUITE(LoggerTests)

BOOST_AUTO_TEST_CASE(test_receiver) {
    Logger log;

    Logger::LogMessage lastMessage("", Logger::Error, "");

    auto receiver = std::make_shared<CallbackReceiver>(
        [&](const Logger::LogMessage& m) { lastMessage = m; });

    log.addReceiver(receiver);

    log.info("Tests", "Test");

    BOOST_CHECK_EQUAL(lastMessage.component, "Tests");
    BOOST_CHECK_EQUAL(lastMessage.severity, Logger::Info);
    BOOST_CHECK_EQUAL(lastMessage.message, "Test");
}

BOOST_AUTO_TEST_CASE(test_remove_receiver) {
    Logger log;

    Logger::LogMessage lastMessage("", Logger::Error, "");

    auto receiver = std::make_shared<CallbackReceiver>(
        [&](const Logger::LogMessage& m) { lastMessage = m; });

    log.addReceiver(receiver);
    log.removeReceiver(receiver);

    log.info("Tests", "Test");

    BOOST_CHECK_EQUAL(lastMessage.component, "");
    BOOST_CHECK_EQUAL(lastMessage.severity, Logger::Error);
    BOOST_CHECK_EQUAL(lastMessage.message, "");
}

BOOST_AUTO_TEST_SUITE_END()
