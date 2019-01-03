#!/usr/bin/env python3

import openrw
import unittest


class TestReceiver(openrw.logging.MessageReceiver):
    def __init__(self):
        openrw.logging.MessageReceiver.__init__(self)
        self.messages = []

    def message_received(self, message: openrw.logging.LogMessage):
        self.messages.append(message)


class TestLoggerMethods(unittest.TestCase):
    def setUp(self):
        self.logger = openrw.logging.Logger()
        self.receiver = TestReceiver()
        self.logger.add_receiver(self.receiver)

    def tearDown(self):
        self.logger.remove_receiver(self.receiver)
        del self.receiver
        del self.logger

    def test_initial(self):
        self.assertEqual(len(self.receiver.messages), 0)

    def test_LogMessage(self):
        msg = openrw.logging.LogMessage('component_text', openrw.logging.VERBOSE, 'verbose_text')
        self.assertEqual(msg.component, 'component_text')
        self.assertEqual(msg.severity, openrw.logging.VERBOSE)
        self.assertEqual(msg.message, 'verbose_text')
        self.assertEqual(msg, msg)
        msg2 = openrw.logging.LogMessage(msg.component, msg.severity, msg.message)
        self.assertTrue(msg, msg2)
        self.assertTrue(msg.component in repr(msg))
        self.assertTrue(msg.message in repr(msg))

    def test_log_verbose(self):
        msg = openrw.logging.LogMessage('component_text', openrw.logging.VERBOSE, 'verbose_text')
        self.logger.log(msg.component, msg.severity, msg.message)
        self.assertEqual(self.receiver.messages, [msg])
        msg2 = openrw.logging.LogMessage('component_text2', openrw.logging.VERBOSE, 'verbose_text2')
        self.logger.verbose(msg2.component, msg2.message)
        self.assertEqual(self.receiver.messages, [msg, msg2])

    def test_log_info(self):
        msg = openrw.logging.LogMessage('component_text', openrw.logging.INFO, 'info_text')
        self.logger.log(msg.component, msg.severity, msg.message)
        self.assertEqual(self.receiver.messages, [msg])
        msg2 = openrw.logging.LogMessage('component_text2', openrw.logging.INFO, 'info_text')
        self.logger.info(msg2.component, msg2.message)
        self.assertEqual(self.receiver.messages, [msg, msg2])

    def test_log_warning(self):
        msg = openrw.logging.LogMessage('component_text', openrw.logging.WARNING, 'warning_text')
        self.logger.log(msg.component, msg.severity, msg.message)
        self.assertEqual(self.receiver.messages, [msg])
        msg2 = openrw.logging.LogMessage('component_text2', openrw.logging.WARNING, '')
        self.logger.warning(msg2.component, msg2.message)
        self.assertEqual(self.receiver.messages, [msg, msg2])

    def test_log_error(self):
        msg = openrw.logging.LogMessage('component_text', openrw.logging.ERROR, 'warning_text')
        self.logger.log(msg.component, msg.severity, msg.message)
        self.assertEqual(self.receiver.messages, [msg])
        msg2 = openrw.logging.LogMessage('component_text2', openrw.logging.ERROR, '')
        self.logger.error(msg2.component, msg2.message)
        self.assertEqual(self.receiver.messages, [msg, msg2])


if __name__ == '__main__':
    unittest.main()
