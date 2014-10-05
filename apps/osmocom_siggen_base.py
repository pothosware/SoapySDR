#!/usr/bin/env python
#
# Copyright 2008,2009,2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

SAMP_RATE_KEY = 'samp_rate'
GAIN_KEY = lambda x: 'gain:'+x
BWIDTH_KEY = 'bwidth'
TX_FREQ_KEY = 'tx_freq'
FREQ_CORR_KEY = 'freq_corr'
AMPLITUDE_KEY = 'amplitude'
AMPL_RANGE_KEY = 'ampl_range'
WAVEFORM_FREQ_KEY = 'waveform_freq'
WAVEFORM_OFFSET_KEY = 'waveform_offset'
WAVEFORM2_FREQ_KEY = 'waveform2_freq'
FREQ_RANGE_KEY = 'freq_range'
GAIN_RANGE_KEY = lambda x: 'gain_range:'+x
BWIDTH_RANGE_KEY = 'bwidth_range'
DC_OFFSET_REAL = 'dc_offset_real'
DC_OFFSET_IMAG = 'dc_offset_imag'
IQ_BALANCE_MAG = 'iq_balance_mag'
IQ_BALANCE_PHA = 'iq_balance_pha'
TYPE_KEY = 'type'

def setter(ps, key, val): ps[key] = val

import osmosdr
from gnuradio import blocks
from gnuradio import filter
from gnuradio import analog
from gnuradio import digital
from gnuradio import gr, gru, eng_notation
from gnuradio.gr.pubsub import pubsub
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys
import math
import numpy
import random

n2s = eng_notation.num_to_str

waveforms = { analog.GR_SIN_WAVE    : "Sinusoid",
              analog.GR_CONST_WAVE  : "Constant",
              analog.GR_GAUSSIAN    : "Gaussian Noise",
              analog.GR_UNIFORM     : "Uniform Noise",
              "2tone"               : "Two Tone (IMD)",
              "sweep"               : "Freq. Sweep",
              "gsm"                 : "GSM Bursts" }

class gsm_source_c(gr.hier_block2):
    def __init__(self, sample_rate, amplitude):
        gr.hier_block2.__init__(self, "gsm_source_c",
            gr.io_signature(0, 0, 0), # Input signature
            gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

        self._symb_rate = 13e6 / 48;
        self._samples_per_symbol = 2

        self._data  = blocks.vector_source_b(self.gen_gsm_seq(), True, 2)
        self._split = blocks.vector_to_streams(gr.sizeof_char*1, 2)

        self._pack = blocks.unpacked_to_packed_bb(1, gr.GR_MSB_FIRST)
        self._mod  = digital.gmsk_mod(self._samples_per_symbol, bt=0.35)

        self._pwr_f = blocks.char_to_float(1, 1)
        self._pwr_c = blocks.float_to_complex(1)
        self._pwr_w = blocks.repeat(gr.sizeof_gr_complex*1, self._samples_per_symbol)

        self._mul = blocks.multiply_vcc(1)
        self._interpolate = filter.fractional_resampler_cc( 0,
            (self._symb_rate * self._samples_per_symbol) / sample_rate )
        self._scale = blocks.multiply_const_cc(amplitude)

        self.connect(self._data, self._split)
        self.connect((self._split, 0), self._pack, self._mod, (self._mul, 0))
        self.connect((self._split, 1), self._pwr_f, self._pwr_c, self._pwr_w, (self._mul, 1))
        self.connect(self._mul, self._interpolate, self._scale, self)

    def set_amplitude(self, amplitude):
        self._scale.set_k(amplitude)

    def set_sampling_freq(self, sample_rate):
        self._interpolate.set_interp_ratio( (self._symb_rate * self._samples_per_symbol) / sample_rate )

    def gen_gsm_burst(self, l):
        chunks = [
            [0,0,0],
            list(numpy.random.randint(0, 2, 58)),
            [0,0,1,0,0,1,0,1,1,1,0,0,0,0,1,0,0,0,1,0,0,1,0,1,1,1],
            list(numpy.random.randint(0, 2, 58)),
            [0,0,0],
        ]
        burst = sum(chunks,[])
        burst = sum(map(list, zip(burst, (1,) * len(burst))), [])
        burst += [1,0] * (l-148)
        return map(int, burst)

    def gen_gsm_frame(self):
        return \
            self.gen_gsm_burst(158) + \
            self.gen_gsm_burst(158) + \
            self.gen_gsm_burst(158) + \
            self.gen_gsm_burst(159) + \
            self.gen_gsm_burst(158) + \
            self.gen_gsm_burst(158) + \
            self.gen_gsm_burst(158) + \
            self.gen_gsm_burst(159)

    def gen_gsm_seq(self):
        return sum([self.gen_gsm_frame() for i in range(10)], [])

#
# GUI-unaware GNU Radio flowgraph.  This may be used either with command
# line applications or GUI applications.
#
class top_block(gr.top_block, pubsub):
    def __init__(self, options, args):
        gr.top_block.__init__(self)
        pubsub.__init__(self)
        self._verbose = options.verbose

        #initialize values from options
        self._setup_osmosdr(options)
        self[SAMP_RATE_KEY] = options.samp_rate
        self[TX_FREQ_KEY] = options.tx_freq
        self[FREQ_CORR_KEY] = options.freq_corr
        self[AMPLITUDE_KEY] = options.amplitude
        self[WAVEFORM_FREQ_KEY] = options.waveform_freq
        self[WAVEFORM_OFFSET_KEY] = options.offset
        self[WAVEFORM2_FREQ_KEY] = options.waveform2_freq

        # initialize reasonable defaults for DC / IQ correction
        self[DC_OFFSET_REAL] = 0
        self[DC_OFFSET_IMAG] = 0
        self[IQ_BALANCE_MAG] = 0
        self[IQ_BALANCE_PHA] = 0

        #subscribe set methods
        self.subscribe(SAMP_RATE_KEY, self.set_samp_rate)

        for name in self.get_gain_names():
            self.subscribe(GAIN_KEY(name), (lambda gain,self=self,name=name: self.set_named_gain(gain, name)))

        self.subscribe(BWIDTH_KEY, self.set_bandwidth)
        self.subscribe(TX_FREQ_KEY, self.set_freq)
        self.subscribe(FREQ_CORR_KEY, self.set_freq_corr)
        self.subscribe(AMPLITUDE_KEY, self.set_amplitude)
        self.subscribe(WAVEFORM_FREQ_KEY, self.set_waveform_freq)
        self.subscribe(WAVEFORM2_FREQ_KEY, self.set_waveform2_freq)
        self.subscribe(TYPE_KEY, self.set_waveform)

        self.subscribe(DC_OFFSET_REAL, self.set_dc_offset)
        self.subscribe(DC_OFFSET_IMAG, self.set_dc_offset)
        self.subscribe(IQ_BALANCE_MAG, self.set_iq_balance)
        self.subscribe(IQ_BALANCE_PHA, self.set_iq_balance)

        #force update on pubsub keys
        for key in (SAMP_RATE_KEY, GAIN_KEY, BWIDTH_KEY,
                    TX_FREQ_KEY, FREQ_CORR_KEY, AMPLITUDE_KEY,
                    WAVEFORM_FREQ_KEY, WAVEFORM_OFFSET_KEY, WAVEFORM2_FREQ_KEY):
            #print key, "=", self[key]
            self[key] = self[key]
        self[TYPE_KEY] = options.type #set type last

    def _setup_osmosdr(self, options):
        self._sink = osmosdr.sink(options.args)

        try:
            self._sink.get_sample_rates().start()
        except RuntimeError:
            print "Sink has no sample rates (wrong device arguments?)."
            sys.exit(1)

        if options.samp_rate is None:
            options.samp_rate = self._sink.get_sample_rates().start()

        self.set_samp_rate(options.samp_rate)

        # Set the gain from options
        if(options.gain):
            gain = self._sink.set_gain(options.gain)
            if self._verbose:
                print "Set gain to:", gain

        if self._verbose:
            gain_names = self.src.get_gain_names()
            for name in gain_names:
                range = self.src.get_gain_range(name)
                print "%s gain range: start %d stop %d step %d" % (name, range.start(), range.stop(), range.step())

        if options.gains:
            for tuple in options.gains.split(","):
                name, gain = tuple.split(":")
                gain = int(gain)
                print "Setting gain %s to %d." % (name, gain)
                self.src.set_gain(gain, name)

        if self._verbose:
            rates = self.src.get_sample_rates()
            print 'Supported sample rates %d-%d step %d.' % (rates.start(), rates.stop(), rates.step())

        # Set the antenna
        if(options.antenna):
            ant = self._sink.set_antenna(options.antenna, 0)
            if self._verbose:
                print "Set antenna to:", ant

        self.publish(FREQ_RANGE_KEY, self._sink.get_freq_range)

        for name in self.get_gain_names():
            self.publish(GAIN_RANGE_KEY(name), (lambda self=self,name=name: self._sink.get_gain_range(name)))

        self.publish(BWIDTH_RANGE_KEY, self._sink.get_bandwidth_range)

        for name in self.get_gain_names():
            self.publish(GAIN_KEY(name), (lambda self=self,name=name: self._sink.get_gain(name)))

        self.publish(BWIDTH_KEY, self._sink.get_bandwidth)

    def get_gain_names(self):
        return self._sink.get_gain_names()

    def set_samp_rate(self, sr):
        sr = self._sink.set_sample_rate(sr)

        if self[TYPE_KEY] in (analog.GR_SIN_WAVE, analog.GR_CONST_WAVE):
            self._src.set_sampling_freq(self[SAMP_RATE_KEY])
        elif self[TYPE_KEY] == "2tone":
            self._src1.set_sampling_freq(self[SAMP_RATE_KEY])
            self._src2.set_sampling_freq(self[SAMP_RATE_KEY])
        elif self[TYPE_KEY] == "sweep":
            self._src1.set_sampling_freq(self[SAMP_RATE_KEY])
            self._src2.set_sampling_freq(self[WAVEFORM_FREQ_KEY]*2*math.pi/self[SAMP_RATE_KEY])
        elif self[TYPE_KEY] == "gsm":
            self._src.set_sampling_freq(self[SAMP_RATE_KEY])
        else:
            return True # Waveform not yet set

        if self._verbose:
            print "Set sample rate to:", sr

        return True

    def set_named_gain(self, gain, name):
        if gain is None:
            g = self[GAIN_RANGE_KEY(name)]
            gain = float(g.start()+g.stop())/2
            if self._verbose:
                print "Using auto-calculated mid-point gain"
            self[GAIN_KEY(name)] = gain
            return

        gain = self._sink.set_gain(gain, name)
        if self._verbose:
            print "Set " + name + " gain to:", gain

    def set_bandwidth(self, bw):
        clipped_bw = self[BWIDTH_RANGE_KEY].clip(bw)
        if self._sink.get_bandwidth() != clipped_bw:
            bw = self._sink.set_bandwidth(clipped_bw)

            if self._verbose:
                print "Set bandwidth to:", bw

    def set_dc_offset(self, value):
        correction = complex( self[DC_OFFSET_REAL], self[DC_OFFSET_IMAG] )

        try:
            self._sink.set_dc_offset( correction )

            if self._verbose:
                print "Set DC offset to", correction
        except RuntimeError as ex:
            print ex

    def set_iq_balance(self, value):
        correction = complex( self[IQ_BALANCE_MAG], self[IQ_BALANCE_PHA] )

        try:
            self._sink.set_iq_balance( correction )

            if self._verbose:
                print "Set IQ balance to", correction
        except RuntimeError as ex:
            print ex

    def set_freq(self, freq):
        if freq is None:
            f = self[FREQ_RANGE_KEY]
            freq = float(f.start()+f.stop())/2.0
            if self._verbose:
                print "Using auto-calculated mid-point frequency"
            self[TX_FREQ_KEY] = freq
            return

        freq = self._sink.set_center_freq(freq)
        if freq is not None:
            self._freq = freq
            if self._verbose:
                print "Set center frequency to", freq
        elif self._verbose:
            print "Failed to set freq."
        return freq

    def set_freq_corr(self, ppm):
        if ppm is None:
            ppm = 0.0
            if self._verbose:
                print "Using frequency corrrection of", ppm
            self[FREQ_CORR_KEY] = ppm
            return

        ppm = self._sink.set_freq_corr(ppm)
        if self._verbose:
            print "Set frequency correction to:", ppm

    def set_waveform_freq(self, freq):
        if self[TYPE_KEY] == analog.GR_SIN_WAVE:
            self._src.set_frequency(freq)
        elif self[TYPE_KEY] == "2tone":
            self._src1.set_frequency(freq)
        elif self[TYPE_KEY] == 'sweep':
            #there is no set sensitivity, redo fg
            self[TYPE_KEY] = self[TYPE_KEY]
        return True

    def set_waveform2_freq(self, freq):
        if freq is None:
            self[WAVEFORM2_FREQ_KEY] = -self[WAVEFORM_FREQ_KEY]
            return
        if self[TYPE_KEY] == "2tone":
            self._src2.set_frequency(freq)
        elif self[TYPE_KEY] == "sweep":
            self._src1.set_frequency(freq)
        return True

    def set_waveform(self, type):
        self.lock()
        self.disconnect_all()
        if type == analog.GR_SIN_WAVE or type == analog.GR_CONST_WAVE:
            self._src = analog.sig_source_c(self[SAMP_RATE_KEY],        # Sample rate
                                        type,                       # Waveform type
                                        self[WAVEFORM_FREQ_KEY],    # Waveform frequency
                                        self[AMPLITUDE_KEY],        # Waveform amplitude
                                        self[WAVEFORM_OFFSET_KEY])  # Waveform offset
        elif type == analog.GR_GAUSSIAN or type == analog.GR_UNIFORM:
            self._src = analog.noise_source_c(type, self[AMPLITUDE_KEY])
        elif type == "2tone":
            self._src1 = analog.sig_source_c(self[SAMP_RATE_KEY],
                                         analog.GR_SIN_WAVE,
                                         self[WAVEFORM_FREQ_KEY],
                                         self[AMPLITUDE_KEY]/2.0,
                                         0)
            if(self[WAVEFORM2_FREQ_KEY] is None):
                self[WAVEFORM2_FREQ_KEY] = -self[WAVEFORM_FREQ_KEY]

            self._src2 = analog.sig_source_c(self[SAMP_RATE_KEY],
                                         analog.GR_SIN_WAVE,
                                         self[WAVEFORM2_FREQ_KEY],
                                         self[AMPLITUDE_KEY]/2.0,
                                         0)
            self._src = blocks.add_cc()
            self.connect(self._src1,(self._src,0))
            self.connect(self._src2,(self._src,1))
        elif type == "sweep":
            # rf freq is center frequency
            # waveform_freq is total swept width
            # waveform2_freq is sweep rate
            # will sweep from (rf_freq-waveform_freq/2) to (rf_freq+waveform_freq/2)
            if self[WAVEFORM2_FREQ_KEY] is None:
                self[WAVEFORM2_FREQ_KEY] = 0.1

            self._src1 = analog.sig_source_f(self[SAMP_RATE_KEY],
                                         analog.GR_TRI_WAVE,
                                         self[WAVEFORM2_FREQ_KEY],
                                         1.0,
                                         -0.5)
            self._src2 = analog.frequency_modulator_fc(self[WAVEFORM_FREQ_KEY]*2*math.pi/self[SAMP_RATE_KEY])
            self._src = blocks.multiply_const_cc(self[AMPLITUDE_KEY])
            self.connect(self._src1,self._src2,self._src)
        elif type == "gsm":
            self._src = gsm_source_c(self[SAMP_RATE_KEY], self[AMPLITUDE_KEY])
        else:
            raise RuntimeError("Unknown waveform type")

        self.connect(self._src, self._sink)
        self.unlock()

        if self._verbose:
            print "Set baseband modulation to:", waveforms[type]
            if type == analog.GR_SIN_WAVE:
                print "Modulation frequency: %sHz" % (n2s(self[WAVEFORM_FREQ_KEY]),)
                print "Initial phase:", self[WAVEFORM_OFFSET_KEY]
            elif type == "2tone":
                print "Tone 1: %sHz" % (n2s(self[WAVEFORM_FREQ_KEY]),)
                print "Tone 2: %sHz" % (n2s(self[WAVEFORM2_FREQ_KEY]),)
            elif type == "sweep":
                print "Sweeping across %sHz to %sHz" % (n2s(-self[WAVEFORM_FREQ_KEY]/2.0),n2s(self[WAVEFORM_FREQ_KEY]/2.0))
                print "Sweep rate: %sHz" % (n2s(self[WAVEFORM2_FREQ_KEY]),)
            elif type == "gsm":
                print "GSM Burst Sequence"
            print "TX amplitude:", self[AMPLITUDE_KEY]

    def set_amplitude(self, amplitude):
        if amplitude < 0.0 or amplitude > 1.0:
            if self._verbose:
                print "Amplitude out of range:", amplitude
            return False

        if self[TYPE_KEY] in (analog.GR_SIN_WAVE, analog.GR_CONST_WAVE, analog.GR_GAUSSIAN, analog.GR_UNIFORM):
            self._src.set_amplitude(amplitude)
        elif self[TYPE_KEY] == "2tone":
            self._src1.set_amplitude(amplitude/2.0)
            self._src2.set_amplitude(amplitude/2.0)
        elif self[TYPE_KEY] == "sweep":
            self._src.set_k(amplitude)
        elif self[TYPE_KEY] == "gsm":
            self._src.set_amplitude(amplitude)
        else:
            return True # Waveform not yet set

        if self._verbose:
            print "Set amplitude to:", amplitude
        return True

def get_options():
    usage="%prog: [options]"

    parser = OptionParser(option_class=eng_option, usage=usage)
    parser.add_option("-a", "--args", type="string", default="",
                      help="Device args, [default=%default]")
    parser.add_option("-A", "--antenna", type="string", default=None,
                      help="Select Rx Antenna where appropriate")
    parser.add_option("-s", "--samp-rate", type="eng_float", default=None,
                      help="Set sample rate (bandwidth), minimum by default")
    parser.add_option("-g", "--gain", type="eng_float", default=None,
                      help="Set gain in dB (default is midpoint)")
    parser.add_option("-G", "--gains", type="string", default=None,
                      help="Set named gain in dB, name:gain,name:gain,...")
    parser.add_option("-f", "--tx-freq", type="eng_float", default=None,
                      help="Set carrier frequency to FREQ [default=mid-point]",
                      metavar="FREQ")
    parser.add_option("-c", "--freq-corr", type="int", default=None,
                      help="Set carrier frequency correction [default=0]")
    parser.add_option("-x", "--waveform-freq", type="eng_float", default=0,
                      help="Set baseband waveform frequency to FREQ [default=%default]")
    parser.add_option("-y", "--waveform2-freq", type="eng_float", default=None,
                      help="Set 2nd waveform frequency to FREQ [default=%default]")
    parser.add_option("--sine", dest="type", action="store_const", const=analog.GR_SIN_WAVE,
                      help="Generate a carrier modulated by a complex sine wave",
                      default=analog.GR_SIN_WAVE)
    parser.add_option("--const", dest="type", action="store_const", const=analog.GR_CONST_WAVE,
                      help="Generate a constant carrier")
    parser.add_option("--offset", type="eng_float", default=0,
                      help="Set waveform phase offset to OFFSET [default=%default]")
    parser.add_option("--gaussian", dest="type", action="store_const", const=analog.GR_GAUSSIAN,
                      help="Generate Gaussian random output")
    parser.add_option("--uniform", dest="type", action="store_const", const=analog.GR_UNIFORM,
                      help="Generate Uniform random output")
    parser.add_option("--2tone", dest="type", action="store_const", const="2tone",
                      help="Generate Two Tone signal for IMD testing")
    parser.add_option("--sweep", dest="type", action="store_const", const="sweep",
                      help="Generate a swept sine wave")
    parser.add_option("--gsm", dest="type", action="store_const", const="gsm",
                      help="Generate GMSK modulated GSM Burst Sequence")
    parser.add_option("", "--amplitude", type="eng_float", default=0.3,
                      help="Set output amplitude to AMPL (0.1-1.0) [default=%default]",
                      metavar="AMPL")
    parser.add_option("-v", "--verbose", action="store_true", default=False,
                      help="Use verbose console output [default=%default]")

    (options, args) = parser.parse_args()

    return (options, args)

# If this script is executed, the following runs. If it is imported,
# the below does not run.
def test_main():
    if gr.enable_realtime_scheduling() != gr.RT_OK:
        print "Note: failed to enable realtime scheduling, continuing"

    # Grab command line options and create top block
    try:
        (options, args) = get_options()
        tb = top_block(options, args)

    except RuntimeError, e:
        print e
        sys.exit(1)

    tb.start()
    raw_input('Press Enter to quit: ')
    tb.stop()
    tb.wait()

# Make sure to create the top block (tb) within a function:
# That code in main will allow tb to go out of scope on return,
# which will call the decontructor on radio and stop transmit.
# Whats odd is that grc works fine with tb in the __main__,
# perhaps its because the try/except clauses around tb.
if __name__ == "__main__":
    test_main()
