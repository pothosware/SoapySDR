#!/usr/bin/env python
"""Measure round trip delay through RF loopback/leakage

"""

import argparse
import os
import time

import numpy as np

import SoapySDR
from SoapySDR import * #SOAPY_SDR_ constants
import soapy_log_handle

def generate_cf32_pulse(num_samps, width=5, scale_factor=0.3):
    """Create a sinc pulse."""
    rel_time = np.linspace(-width, width, num_samps)
    pulse = np.sinc(rel_time).astype(np.complex64)
    return pulse * scale_factor

def measure_delay(
        args,
        rate,
        freq=None,
        rx_bw=None,
        tx_bw=None,
        rx_chan=0,
        tx_chan=0,
        rx_ant=None,
        tx_ant=None,
        rx_gain=None,
        tx_gain=None,
        clock_rate=None,
        num_tx_samps=200,
        num_rx_samps=10000,
        dump_dir=None,
):
    """Transmit a bandlimited pulse, receive it and find the delay."""

    sdr = SoapySDR.Device(args)
    if not sdr.hasHardwareTime():
        raise Exception('this device does not support timed streaming')

    #set clock rate first
    if clock_rate is not None:
        sdr.setMasterClockRate(clock_rate)

    #set sample rate
    sdr.setSampleRate(SOAPY_SDR_RX, rx_chan, rate)
    sdr.setSampleRate(SOAPY_SDR_TX, tx_chan, rate)
    print("Actual Rx Rate %f Msps"%(sdr.getSampleRate(SOAPY_SDR_RX, rx_chan) / 1e6))
    print("Actual Tx Rate %f Msps"%(sdr.getSampleRate(SOAPY_SDR_TX, tx_chan) / 1e6))

    #set antenna
    if rx_ant is not None:
        sdr.setAntenna(SOAPY_SDR_RX, rx_chan, rx_ant)
    if tx_ant is not None:
        sdr.setAntenna(SOAPY_SDR_TX, tx_chan, tx_ant)

    #set overall gain
    if rx_gain is not None:
        sdr.setGain(SOAPY_SDR_RX, rx_chan, rx_gain)
    if tx_gain is not None:
        sdr.setGain(SOAPY_SDR_TX, tx_chan, tx_gain)

    #tune frontends
    if freq is not None:
        sdr.setFrequency(SOAPY_SDR_RX, rx_chan, freq)
    if freq is not None:
        sdr.setFrequency(SOAPY_SDR_TX, tx_chan, freq)

    #set bandwidth
    if rx_bw is not None:
        sdr.setBandwidth(SOAPY_SDR_RX, rx_chan, rx_bw)
    if tx_bw is not None:
        sdr.setBandwidth(SOAPY_SDR_TX, tx_chan, tx_bw)

    #create rx and tx streams
    print("Create Rx and Tx streams")
    rx_stream = sdr.setupStream(SOAPY_SDR_RX, SOAPY_SDR_CF32, [rx_chan])
    tx_stream = sdr.setupStream(SOAPY_SDR_TX, SOAPY_SDR_CF32, [tx_chan])

    #let things settle
    time.sleep(1)

    #transmit a pulse in the near future
    sdr.activateStream(tx_stream)
    tx_pulse = generate_cf32_pulse(num_tx_samps)
    tx_time_0 = int(sdr.getHardwareTime() + 0.1e9) #100ms
    tx_flags = SOAPY_SDR_HAS_TIME | SOAPY_SDR_END_BURST
    status = sdr.writeStream(tx_stream, [tx_pulse], len(tx_pulse), tx_flags, tx_time_0)
    if status.ret != len(tx_pulse):
        raise Exception('transmit failed %s'%str(status))

    #receive slightly before transmit time
    rx_buffs = np.array([], np.complex64)
    rx_flags = SOAPY_SDR_HAS_TIME | SOAPY_SDR_END_BURST
    #half of the samples come before the transmit time
    receive_time = int(tx_time_0 - (num_rx_samps/rate) * 1e9 / 2)
    sdr.activateStream(rx_stream, rx_flags, receive_time, num_rx_samps)
    rx_time_0 = None

    #accumulate receive buffer into large contiguous buffer
    while True:
        rx_buff = np.array([0]*1024, np.complex64)
        timeout_us = int(5e5) #500 ms >> stream time
        status = sdr.readStream(rx_stream, [rx_buff], len(rx_buff), timeoutUs=timeout_us)

        #stash time on first buffer
        if status.ret > 0 and rx_buffs.size == 0:
            rx_time_0 = status.timeNs
            if (status.flags & SOAPY_SDR_HAS_TIME) == 0:
                raise Exception('receive fail - no timestamp on first readStream %s'%(str(status)))

        #accumulate buffer or exit loop
        if status.ret > 0:
            rx_buffs = np.concatenate((rx_buffs, rx_buff[:status.ret]))
        else:
            break

    #cleanup streams
    print("Cleanup streams")
    sdr.deactivateStream(tx_stream)
    sdr.closeStream(rx_stream)
    sdr.closeStream(tx_stream)

    #check resulting buffer
    if len(rx_buffs) != num_rx_samps:
        raise Exception(
            'receive fail - captured samples %d out of %d'%(len(rx_buffs), num_rx_samps))
    if rx_time_0 is None:
        raise Exception('receive fail - no valid timestamp')

    #clear initial samples because transients
    rx_mean = np.mean(rx_buffs)
    for i in range(len(rx_buffs) // 100):
        rx_buffs[i] = rx_mean

    #normalize the samples
    def normalize(samps):
        samps = samps - np.mean(samps) #remove dc
        samps = np.absolute(samps) #magnitude
        samps = samps / max(samps) #norm ampl to peak
        #print samps[:100]
        return samps

    tx_pulse_norm = normalize(tx_pulse)
    rx_buffs_norm = normalize(rx_buffs)

    #dump debug samples
    if dump_dir is not None:
        np.save(os.path.join(dump_dir, 'txNorm.npy'), tx_pulse_norm)
        np.save(os.path.join(dump_dir, 'rxNorm.npy'), rx_buffs_norm)
        np.save(os.path.join(dump_dir, 'rxRawI.npy'), np.real(rx_buffs))
        np.save(os.path.join(dump_dir, 'rxRawQ.npy'), np.imag(rx_buffs))

    #look for the for peak index for time offsets
    rx_argmax_index = np.argmax(rx_buffs_norm)
    tx_argmax_index = np.argmax(tx_pulse_norm)

    #check goodness of peak by comparing argmax and correlation
    rx_coor_index = np.argmax(np.correlate(rx_buffs_norm, tx_pulse_norm)) + len(tx_pulse_norm) // 2
    if abs(rx_coor_index-rx_argmax_index) > len(tx_pulse_norm)/4:
        raise Exception(
            'correlation(%d) does not match argmax(%d), probably bad data' %
            (rx_coor_index, rx_argmax_index))

    #calculate time offset
    tx_peak_time = int(tx_time_0 + (tx_argmax_index / rate) * 1e9)
    rx_peak_time = int(rx_time_0 + (rx_argmax_index / rate) * 1e9)
    time_delta = rx_peak_time - tx_peak_time
    print('>>> Time delta %f us'%(time_delta / 1e3))
    print("Done!")

def main():
    """Parse command line arguments and perform measurement."""
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
    parser.add_argument(
        "--abort-on-error", action='store_true',
        help="Halts operations if the SDR logs an error")

    options = parser.parse_args()

    if options.abort_on_error:
        exception_level = SOAPY_SDR_ERROR
    else:
        exception_level = None
    soapy_log_handle.set_python_log_handler(exception_level=exception_level)
    if options.debug:
        SoapySDR.setLogLevel(SOAPY_SDR_DEBUG)

    measure_delay(
        args=options.args,
        rate=options.rate,
        freq=options.freq,
        rx_bw=options.rx_bw,
        tx_bw=options.tx_bw,
        rx_ant=options.rx_ant,
        tx_ant=options.tx_ant,
        rx_gain=options.rx_gain,
        tx_gain=options.tx_gain,
        rx_chan=options.rx_chan,
        tx_chan=options.tx_chan,
        clock_rate=options.clock_rate,
        dump_dir=options.dump_dir,
    )

if __name__ == '__main__':
    main()
