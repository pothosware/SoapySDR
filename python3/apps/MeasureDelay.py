#!/usr/bin/env python3
"""Measure round trip delay through RF loopback/leakage

"""

import argparse
import os
import time

import numpy as np
from scipy import signal

import SoapySDR
from SoapySDR import * #SOAPY_SDR_ constants

import soapy_log_handle

def generate_cf32_pulse(numSamps, width=5, scaleFactor=0.3):
    x = np.linspace(-width, width, numSamps)
    pulse = np.sinc(x).astype(np.complex64)
    return pulse*scaleFactor

def measure_delay(
    args,
    rate,
    freq=None,
    rxBw=None,
    txBw=None,
    rxChan=0,
    txChan=0,
    rxAnt=None,
    txAnt=None,
    rxGain=None,
    txGain=None,
    clockRate=None,
    numTxSamps=200,
    numRxSamps=10000,
    dumpDir=None,
):
    sdr = SoapySDR.Device(args)
    if not sdr.hasHardwareTime():
        raise Exception('this device does not support timed streaming')

    #set clock rate first
    if clockRate is not None: sdr.setMasterClockRate(clockRate)

    #set sample rate
    sdr.setSampleRate(SOAPY_SDR_RX, rxChan, rate)
    sdr.setSampleRate(SOAPY_SDR_TX, txChan, rate)
    print("Actual Rx Rate %f Msps"%(sdr.getSampleRate(SOAPY_SDR_RX, rxChan) / 1e6))
    print("Actual Tx Rate %f Msps"%(sdr.getSampleRate(SOAPY_SDR_TX, txChan) / 1e6))

    #set antenna
    if rxAnt is not None:
        sdr.setAntenna(SOAPY_SDR_RX, rxChan, rxAnt)
    if txAnt is not None:
        sdr.setAntenna(SOAPY_SDR_TX, txChan, txAnt)

    #set overall gain
    if rxGain is not None:
        sdr.setGain(SOAPY_SDR_RX, rxChan, rxGain)
    if txGain is not None:
        sdr.setGain(SOAPY_SDR_TX, txChan, txGain)

    #tune frontends
    if freq is not None:
        sdr.setFrequency(SOAPY_SDR_RX, rxChan, freq)
    if freq is not None:
        sdr.setFrequency(SOAPY_SDR_TX, txChan, freq)

    #set bandwidth
    if rxBw is not None:
        sdr.setBandwidth(SOAPY_SDR_RX, rxChan, rxBw)
    if txBw is not None:
        sdr.setBandwidth(SOAPY_SDR_TX, txChan, txBw)

    #create rx and tx streams
    print("Create Rx and Tx streams")
    rxStream = sdr.setupStream(SOAPY_SDR_RX, "CF32", [rxChan])
    txStream = sdr.setupStream(SOAPY_SDR_TX, "CF32", [txChan])

    #let things settle
    time.sleep(1)

    #transmit a pulse in the near future
    sdr.activateStream(txStream)
    txPulse = generate_cf32_pulse(numTxSamps)
    txTime0 = int(sdr.getHardwareTime() + 0.1e9) #100ms
    txFlags = SOAPY_SDR_HAS_TIME | SOAPY_SDR_END_BURST
    sr = sdr.writeStream(txStream, [txPulse], len(txPulse), txFlags, txTime0)
    if sr.ret != len(txPulse):
        raise Exception('transmit failed %s'%str(sr))

    #receive slightly before transmit time
    rxBuffs = np.array([], np.complex64)
    rxFlags = SOAPY_SDR_HAS_TIME | SOAPY_SDR_END_BURST
    #half of the samples come before the transmit time
    receiveTime = int(txTime0 - (numRxSamps/rate) * 1e9 / 2)
    sdr.activateStream(rxStream, rxFlags, receiveTime, numRxSamps)
    rxTime0 = None

    #accumulate receive buffer into large contiguous buffer
    while True:
        rxBuff = np.array([0]*1024, np.complex64)
        timeoutUs = int(5e5) #500 ms >> stream time
        sr = sdr.readStream(rxStream, [rxBuff], len(rxBuff), timeoutUs=timeoutUs)

        #stash time on first buffer
        if sr.ret > 0 and len(rxBuffs) == 0:
            rxTime0 = sr.timeNs
            if (sr.flags & SOAPY_SDR_HAS_TIME) == 0:
                raise Exception('receive fail - no timestamp on first readStream %s'%(str(sr)))

        #accumulate buffer or exit loop
        if sr.ret > 0:
            rxBuffs = np.concatenate((rxBuffs, rxBuff[:sr.ret]))
        else:
            break

    #cleanup streams
    print("Cleanup streams")
    sdr.deactivateStream(txStream)
    sdr.closeStream(rxStream)
    sdr.closeStream(txStream)

    #check resulting buffer
    if len(rxBuffs) != numRxSamps:
        raise Exception('receive fail - captured samples %d out of %d'%(len(rxBuffs), numRxSamps))
    if rxTime0 is None:
        raise Exception('receive fail - no valid timestamp')

    #clear initial samples because transients
    rxMean = np.mean(rxBuffs)
    for i in range(len(rxBuffs) // 100): rxBuffs[i] = rxMean

    #normalize the samples
    def normalize(samps):
        samps = samps - np.mean(samps) #remove dc
        samps = np.absolute(samps) #magnitude
        samps = samps / max(samps) #norm ampl to peak
        #print samps[:100]
        return samps

    txPulseNorm = normalize(txPulse)
    rxBuffsNorm = normalize(rxBuffs)

    #dump debug samples
    if dumpDir is not None:
        np.save(os.path.join(dumpDir, 'txNorm.npy'), txPulseNorm)
        np.save(os.path.join(dumpDir, 'rxNorm.npy'), rxBuffsNorm)
        np.save(os.path.join(dumpDir, 'rxRawI.npy'), np.real(rxBuffs))
        np.save(os.path.join(dumpDir, 'rxRawQ.npy'), np.imag(rxBuffs))

    #look for the for peak index for time offsets
    rxArgmaxIndex = np.argmax(rxBuffsNorm)
    txArgmaxIndex = np.argmax(txPulseNorm)

    #check goodness of peak by comparing argmax and correlation
    rxCoorIndex = np.argmax(np.correlate(rxBuffsNorm, txPulseNorm)) + len(txPulseNorm) // 2
    if abs(rxCoorIndex-rxArgmaxIndex) > len(txPulseNorm)/4:
        raise Exception('correlation(%d) does not match argmax(%d), probably bad data'%(rxCoorIndex, rxArgmaxIndex))

    #calculate time offset
    txPeakTime = int(txTime0 + (txArgmaxIndex / rate) * 1e9)
    rxPeakTime = int(rxTime0 + (rxArgmaxIndex / rate) * 1e9)
    timeDelta = rxPeakTime - txPeakTime
    print('>>> Time delta %f us'%(timeDelta / 1e3))
    print("Done!")

def main():
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--args", type=str, help="device factor arguments", default="")
    parser.add_argument("--rate", type=float, help="Tx and Rx sample rate", default=1e6)
    parser.add_argument("--rx-ant", type=str, help="Optional Rx antenna")
    parser.add_argument("--tx-ant", type=str, help="Optional Tx antenna")
    parser.add_argument("--rx-gain", type=float, help="Optional Rx gain (dB)")
    parser.add_argument("--tx-gain", type=float, help="Optional Tx gain (dB)")
    parser.add_argument("--rx-bw", type=float, help="Optional Rx filter bw (Hz)")
    parser.add_argument("--tx-bw", type=float, help="Optional Tx filter bw (Hz)")
    parser.add_argument("--rx-chan", type=int, help="Receiver channel (def=0)", default=0)
    parser.add_argument("--tx-chan", type=int, help="Transmitter channel (def=0)", default=0)
    parser.add_argument("--freq", type=float, help="Optional Tx and Rx freq (Hz)")
    parser.add_argument("--clock-rate", type=float, help="Optional clock rate (Hz)")
    parser.add_argument("--dump-dir", type=str, help="Optional directory to dump debug samples")
    parser.add_argument("--debug", action='store_true', help="Output debug messages")
    parser.add_argument("--abort-on-error", action='store_true', help="Halts operations if the SDR logs an error")

    options = parser.parse_args()

    if options.abort_on_error:
        exception_level=SOAPY_SDR_ERROR
    else:
        exception_level=None
    soapy_log_handle.set_python_log_handler(exception_level=exception_level)
    if options.debug:
        SoapySDR.setLogLevel(SOAPY_SDR_DEBUG)

    measure_delay(
        args=options.args,
        rate=options.rate,
        freq=options.freq,
        rxBw=options.rx_bw,
        txBw=options.tx_bw,
        rxAnt=options.rx_ant,
        txAnt=options.tx_ant,
        rxGain=options.rx_gain,
        txGain=options.tx_gain,
        rxChan=options.rx_chan,
        txChan=options.tx_chan,
        clockRate=options.clock_rate,
        dumpDir=options.dump_dir,
    )

if __name__ == '__main__':
    main()
