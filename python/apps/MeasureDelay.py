########################################################################
## Measure round trip delay through RF loopback/leakage
########################################################################

import SoapySDR
from SoapySDR import * #SOAPY_SDR_ constants
import numpy as np
from scipy import signal
from optparse import OptionParser
import time
import os

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
    print("Actual Rx Rate %f Msps"%(sdr.getSampleRate(SOAPY_SDR_RX, rxChan)/1e6))
    print("Actual Tx Rate %f Msps"%(sdr.getSampleRate(SOAPY_SDR_TX, txChan)/1e6))

    #set antenna
    if rxAnt is not None: sdr.setAntenna(SOAPY_SDR_RX, rxChan, rxAnt)
    if txAnt is not None: sdr.setAntenna(SOAPY_SDR_TX, txChan, txAnt)

    #set overall gain
    if rxGain is not None: sdr.setGain(SOAPY_SDR_RX, rxChan, rxGain)
    if txGain is not None: sdr.setGain(SOAPY_SDR_TX, txChan, txGain)

    #tune frontends
    if freq is not None: sdr.setFrequency(SOAPY_SDR_RX, rxChan, freq)
    if freq is not None: sdr.setFrequency(SOAPY_SDR_TX, txChan, freq)

    #set bandwidth
    if rxBw is not None: sdr.setBandwidth(SOAPY_SDR_RX, rxChan, rxBw)
    if txBw is not None: sdr.setBandwidth(SOAPY_SDR_TX, txChan, txBw)

    #create rx and tx streams
    print("Create Rx and Tx streams")
    rxStream = sdr.setupStream(SOAPY_SDR_RX, "CF32", [rxChan])
    txStream = sdr.setupStream(SOAPY_SDR_TX, "CF32", [txChan])

    #let things settle
    time.sleep(1)

    #transmit a pulse in the near future
    sdr.activateStream(txStream)
    txPulse = generate_cf32_pulse(numTxSamps)
    txTime0 = sdr.getHardwareTime() + long(0.1e9) #100ms
    txFlags = SOAPY_SDR_HAS_TIME | SOAPY_SDR_END_BURST
    sr = sdr.writeStream(txStream, [txPulse], len(txPulse), txFlags, txTime0)
    if sr.ret != len(txPulse): raise Exception('transmit failed %s'%str(sr))

    #receive slightly before transmit time
    rxBuffs = np.array([], np.complex64)
    rxFlags = SOAPY_SDR_HAS_TIME | SOAPY_SDR_END_BURST
    #half of the samples come before the transmit time
    receiveTime = txTime0 - long((numRxSamps/rate)*1e9)/2
    sdr.activateStream(rxStream, rxFlags, receiveTime, numRxSamps)
    rxTime0 = None

    #accumulate receive buffer into large contiguous buffer
    while True:
        rxBuff = np.array([0]*1024, np.complex64)
        timeoutUs = long(5e5) #500 ms >> stream time
        sr = sdr.readStream(rxStream, [rxBuff], len(rxBuff), timeoutUs=timeoutUs)

        #stash time on first buffer
        if sr.ret > 0 and len(rxBuffs) == 0:
            rxTime0 = sr.timeNs
            if (sr.flags & SOAPY_SDR_HAS_TIME) == 0:
                raise Exception('receive fail - no timestamp on first readStream %s'%(str(sr)))

        #accumulate buffer or exit loop
        if sr.ret > 0: rxBuffs = np.concatenate((rxBuffs, rxBuff[:sr.ret]))
        else: break

    #check resulting buffer
    if len(rxBuffs) != numRxSamps:
        raise Exception('receive fail - captured samples %d out of %d'%(len(rxBuffs), numRxSamps))
    if rxTime0 is None:
        raise Exception('receive fail - no valid timestamp')

    #clear initial samples because transients
    rxMean = np.mean(rxBuffs)
    for i in range(len(rxBuffs)/100): rxBuffs[i] = rxMean

    #normalize the samples
    def normalize(samps):
        samps = samps - np.mean(samps) #remove dc
        samps = np.absolute(samps) #magnitude
        samps = samps/max(samps) #norm ampl to peak
        #print samps[:100]
        return samps

    txPulseNorm = normalize(txPulse)
    rxBuffsNorm = normalize(rxBuffs)

    #dump debug samples
    if dumpDir is not None:
        txPulseNorm.tofile(os.path.join(dumpDir, 'txNorm.dat'))
        rxBuffsNorm.tofile(os.path.join(dumpDir, 'rxNorm.dat'))
        np.real(rxBuffs).tofile(os.path.join(dumpDir, 'rxRawI.dat'))
        np.imag(rxBuffs).tofile(os.path.join(dumpDir, 'rxRawQ.dat'))

    #look for the for peak index for time offsets
    rxArgmaxIndex = np.argmax(rxBuffsNorm)
    txArgmaxIndex = np.argmax(txPulseNorm)

    #check goodness of peak by comparing argmax and correlation
    rxCoorIndex = np.argmax(np.correlate(rxBuffsNorm, txPulseNorm))+len(txPulseNorm)/2
    if abs(rxCoorIndex-rxArgmaxIndex) > len(txPulseNorm)/4:
        raise Exception('correlation(%d) does not match argmax(%d), probably bad data'%(rxCoorIndex, rxArgmaxIndex))

    #calculate time offset
    txPeakTime = txTime0 + long((txArgmaxIndex/rate)*1e9)
    rxPeakTime = rxTime0 + long((rxArgmaxIndex/rate)*1e9)
    timeDelta = rxPeakTime - txPeakTime
    print('>>> Time delta %f us'%(timeDelta/1e3))

    #cleanup streams
    print("Cleanup streams")
    sdr.deactivateStream(rxStream)
    sdr.deactivateStream(txStream)
    sdr.closeStream(rxStream)
    sdr.closeStream(txStream)
    print("Done!")

def main():
    parser = OptionParser()
    parser.add_option("--args", type="string", dest="args", help="device factor arguments", default="")
    parser.add_option("--rate", type="float", dest="rate", help="Tx and Rx sample rate", default=1e6)
    parser.add_option("--rxAnt", type="string", dest="rxAnt", help="Optional Rx antenna", default=None)
    parser.add_option("--txAnt", type="string", dest="txAnt", help="Optional Tx antenna", default=None)
    parser.add_option("--rxGain", type="float", dest="rxGain", help="Optional Rx gain (dB)", default=None)
    parser.add_option("--txGain", type="float", dest="txGain", help="Optional Tx gain (dB)", default=None)
    parser.add_option("--rxBw", type="float", dest="rxBw", help="Optional Rx filter bw (Hz)", default=None)
    parser.add_option("--txBw", type="float", dest="txBw", help="Optional Tx filter bw (Hz)", default=None)
    parser.add_option("--rxChan", type="int", dest="rxChan", help="Receiver channel (def=0)", default=0)
    parser.add_option("--txChan", type="int", dest="txChan", help="Transmitter channel (def=0)", default=0)
    parser.add_option("--freq", type="float", dest="freq", help="Optional Tx and Rx freq (Hz)", default=None)
    parser.add_option("--clockRate", type="float", dest="clockRate", help="Optional clock rate (Hz)", default=None)
    parser.add_option("--dumpDir", type="string", dest="dumpDir", help="Optional directory to dump debug samples", default=None)
    (options, args) = parser.parse_args()
    measure_delay(
        args=options.args,
        rate=options.rate,
        freq=options.freq,
        rxBw=options.rxBw,
        txBw=options.txBw,
        rxAnt=options.rxAnt,
        txAnt=options.txAnt,
        rxGain=options.rxGain,
        txGain=options.txGain,
        rxChan=options.rxChan,
        txChan=options.txChan,
        clockRate=options.clockRate,
        dumpDir=options.dumpDir,
    )

if __name__ == '__main__': main()
