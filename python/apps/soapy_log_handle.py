"""Example Python basing logging for SoapySDR.

This permits exceptions to be thrown if the log message is severe enough.
"""
from __future__ import print_function

import sys
import SoapySDR


class SoapyException(Exception):
    """SoapySDR has logged an error message (or worse)."""

def set_python_log_handler(exception_level=None):
    """Replace the default SoapySDR log handler with a Python one.

    The python handler sends the log text to stderr.

    If the log_level is at exception_level or worse then a SoapyException
    is thrown.
    """

    log_level_text = {
        SoapySDR.SOAPY_SDR_FATAL: "FATAL",
        SoapySDR.SOAPY_SDR_CRITICAL: "CRITICAL",
        SoapySDR.SOAPY_SDR_ERROR: "ERROR",
        SoapySDR.SOAPY_SDR_WARNING: "WARNING",
        SoapySDR.SOAPY_SDR_NOTICE: "NOTICE",
        SoapySDR.SOAPY_SDR_INFO: "INFO",
        SoapySDR.SOAPY_SDR_DEBUG: "DEBUG",
        SoapySDR.SOAPY_SDR_TRACE: "TRACE",
        SoapySDR.SOAPY_SDR_SSI: "SSI"}

    def log_handler(log_level, message):
        level_text = log_level_text[log_level]
        log_text = "[{}] {}".format(level_text, message)
        print(log_text, file=sys.stderr)
        if exception_level is not None and log_level <= exception_level:
            raise SoapyException(log_text)

    SoapySDR.registerLogHandler(log_handler)
