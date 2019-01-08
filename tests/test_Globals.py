#!/usr/bin/env python3
import contextlib
import io
import openrw
import unittest
import sys


class OutputRedirector(object):
    def __init__(self):
        self.stdout = io.StringIO()
        self.stderr = io.StringIO()

    def reset(self):
        self.stdout = io.StringIO()
        self.stderr = io.StringIO()

    @contextlib.contextmanager
    def redirect(self):
        stdout = sys.stdout
        stderr = sys.stderr
        sys.stdout = self.stdout
        sys.stderr = self.stderr
        try:
            with openrw.bindutils.ostream_redirect(stdout=True, stderr=True):
                yield
        finally:
            sys.stdout = stdout
            sys.stderr = stderr

    def stdout_text(self):
        self.stdout.seek(0)
        return self.stdout.read()

    def stderr_text(self):
        self.stderr.seek(0)
        return self.stderr.read()