#!/usr/bin/env python3
"""Simple signal generator for testing transmit

Continuously output a carrier with single sideband sinusoid amplitude
modulation.

Terminate with cntl-C.
"""

import argparse
import math
import os
import signal
import time

import numpy as np

from optparse import OptionParser

import SoapySDR
from SoapySDR import * #SOAPY_SDR_ constants

import soapy_log_handle

stop_running = False

def signal_handler(signum, frame):
    global stop_running
    print('Signal handler called with signal {}'.format(signum))
    stop_running = True

def siggen_app(
    args,
    rate,
    ampl=0.7,
    freq=None,
    txBw=None,
    txChan=0,
    txGain=None,
    txAnt=None,
    clockRate=None,
    waveFreq=None
):
    global stop_running

    if waveFreq is None:
        waveFreq = rate / 10

    sdr = SoapySDR.Device(args)
    #set clock rate first
    if clockRate is not None:
        sdr.setMasterClockRate(clockRate)

    #set sample rate
    sdr.setSampleRate(SOAPY_SDR_TX, txChan, rate)
    print("Actual Tx Rate %f Msps"%(sdr.getSampleRate(SOAPY_SDR_TX, txChan) / 1e6))

    #set bandwidth
    if txBw is not None:
        sdr.setBandwidth(SOAPY_SDR_TX, txChan, txBw)

    #set antenna
    print("Set the antenna")
    if txAnt is not None:
        sdr.setAntenna(SOAPY_SDR_TX, txChan, txAnt)

    #set overall gain
    print("Set the gain")
    if txGain is not None:
        sdr.setGain(SOAPY_SDR_TX, txChan, txGain)

    #tune frontends
    print("Tune the frontend")
    if freq is not None:
        sdr.setFrequency(SOAPY_SDR_TX, txChan, freq)


    print("Create Tx stream")
    txStream = sdr.setupStream(SOAPY_SDR_TX, "CF32", [txChan])
    print("Activate Tx Stream")
    sdr.activateStream(txStream)
    phaseAcc = 0
    phaseInc = 2*math.pi*waveFreq/rate
    streamMTU = sdr.getStreamMTU(txStream)
    sampsCh0 = np.array([ampl]*streamMTU, np.complex64)

    timeLastPrint = time.time()
    totalSamps = 0
    while not stop_running:
        phaseAccNext = phaseAcc + streamMTU*phaseInc
        sampsCh0 = ampl*np.exp(1j * np.linspace(phaseAcc, phaseAccNext, streamMTU)).astype(np.complex64)
        phaseAcc = phaseAccNext
        while phaseAcc > math.pi * 2: phaseAcc -= math.pi * 2

        sr = sdr.writeStream(txStream, [sampsCh0], sampsCh0.size, timeoutUs=1000000)
        if sr.ret != sampsCh0.size:
            raise Exception("Expected writeStream() to consume all samples! %d"%sr.ret)
        totalSamps += sr.ret

        if time.time() > timeLastPrint + 5.0:
            print("Python siggen rate: %f Msps"%(totalSamps / (time.time() - timeLastPrint) / 1e6))
            totalSamps = 0
            timeLastPrint = time.time()

    #cleanup streams
    print("Cleanup stream")
    sdr.deactivateStream(txStream)
    sdr.closeStream(txStream)
    print("Done!")

def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument("--args", type=str, help="device factor arguments", default="")
    parser.add_argument("--rate", type=float, help="Tx and Rx sample rate", default=1e6)
    parser.add_argument("--ampl", type=float, help="Tx digital amplitude rate", default=0.7)
    parser.add_argument("--tx-ant", type=str, help="Optional Tx antenna")
    parser.add_argument("--tx-gain", type=float, help="Optional Tx gain (dB)")
    parser.add_argument("--tx-chan", type=int, help="Transmitter channel (def=0)", default=0)
    parser.add_argument("--freq", type=float, help="Optional Tx and Rx freq (Hz)")
    parser.add_argument("--tx-bw", type=float, help="Optional Tx filter bw (Hz)", default=5e6)
    parser.add_argument("--wave-freq", type=float, help="Baseband waveform freq (Hz)")
    parser.add_argument("--clock-rate", type=float, help="Optional clock rate (Hz)")
    parser.add_argument("--debug", action='store_true', help="Output debug messages")
    parser.add_argument("--abort-on-error", action='store_true', help="Halts operations if the SDR logs an error")

    options = parser.parse_args()

    if options.abort_on_error:
        exception_level=SOAPY_SDR_WARNING
    else:
        exception_level=None
    soapy_log_handle.set_python_log_handler(exception_level=exception_level)
    if options.debug:
        SoapySDR.setLogLevel(SOAPY_SDR_DEBUG)

    signal.signal(signal.SIGINT, signal_handler)

    siggen_app(
        args=options.args,
        rate=options.rate,
        ampl=options.ampl,
        freq=options.freq,
        txBw=options.tx_bw,
        txAnt=options.tx_ant,
        txGain=options.tx_gain,
        txChan=options.tx_chan,
        clockRate=options.clock_rate,
        waveFreq=options.wave_freq,
    )

if __name__ == '__main__':
    main()
