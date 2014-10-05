# Copyright 2011 Free Software Foundation, Inc.
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

########################################################################
# Setup library
########################################################################
INCLUDE(GrPlatform) #define LIB_SUFFIX
INCLUDE(GrMiscUtils)
INCLUDE(GrComponent)

########################################################################
# Helpful Macros
########################################################################
MACRO(GR_OSMOSDR_APPEND_SRCS)
    LIST(APPEND gr_osmosdr_srcs ${ARGV})
ENDMACRO(GR_OSMOSDR_APPEND_SRCS)

MACRO(GR_OSMOSDR_APPEND_LIBS)
    LIST(APPEND gr_osmosdr_libs ${ARGV})
ENDMACRO(GR_OSMOSDR_APPEND_LIBS)

GR_OSMOSDR_APPEND_SRCS(
    source_impl.cc
    sink_impl.cc
    ranges.cc
    device.cc
)

GR_OSMOSDR_APPEND_LIBS(
    ${Boost_LIBRARIES}
    ${GNURADIO_ALL_LIBRARIES}
)

########################################################################
# Setup IQBalance component
########################################################################
GR_REGISTER_COMPONENT("Osmocom IQ Imbalance Correction" ENABLE_IQBALANCE GNURADIO_IQBALANCE_FOUND)
if(ENABLE_IQBALANCE)
add_definitions(-DHAVE_IQBALANCE=1)
include_directories(${GNURADIO_IQBALANCE_INCLUDE_DIRS})
GR_OSMOSDR_APPEND_LIBS(${GNURADIO_IQBALANCE_LIBRARIES})
endif(ENABLE_IQBALANCE)

########################################################################
# Setup OsmoSDR component
########################################################################
GR_REGISTER_COMPONENT("sysmocom OsmoSDR" ENABLE_OSMOSDR LIBOSMOSDR_FOUND)
if(ENABLE_OSMOSDR)
GR_INCLUDE_SUBDIRECTORY(osmosdr)
endif(ENABLE_OSMOSDR)

########################################################################
# Setup FCD component
########################################################################
GR_REGISTER_COMPONENT("FUNcube Dongle" ENABLE_FCD GNURADIO_FCD_FOUND)
GR_REGISTER_COMPONENT("FUNcube Dongle Pro+" ENABLE_FCDPP GNURADIO_FCDPP_FOUND)
if(ENABLE_FCD)
add_definitions(-DHAVE_FCD=1)
endif(ENABLE_FCD)
if(ENABLE_FCDPP)
add_definitions(-DHAVE_FCDPP=1)
endif(ENABLE_FCDPP)
if(ENABLE_FCD OR ENABLE_FCDPP)
GR_INCLUDE_SUBDIRECTORY(fcd)
endif(ENABLE_FCD OR ENABLE_FCDPP)

########################################################################
# Setup File component
########################################################################
GR_REGISTER_COMPONENT("IQ File Source" ENABLE_FILE GNURADIO_BLOCKS_FOUND)
if(ENABLE_FILE)
GR_INCLUDE_SUBDIRECTORY(file)
endif(ENABLE_FILE)

########################################################################
# Setup RTL component
########################################################################
GR_REGISTER_COMPONENT("Osmocom RTLSDR" ENABLE_RTL LIBRTLSDR_FOUND)
if(ENABLE_RTL)
GR_INCLUDE_SUBDIRECTORY(rtl)
endif(ENABLE_RTL)

########################################################################
# Setup RTL_TCP component
########################################################################
GR_REGISTER_COMPONENT("RTLSDR TCP Client" ENABLE_RTL_TCP GNURADIO_BLOCKS_FOUND)
if(ENABLE_RTL_TCP)
GR_INCLUDE_SUBDIRECTORY(rtl_tcp)
endif(ENABLE_RTL_TCP)

########################################################################
# Setup UHD component
########################################################################
GR_REGISTER_COMPONENT("Ettus USRP Devices" ENABLE_UHD UHD_FOUND GNURADIO_UHD_FOUND)
if(ENABLE_UHD)
GR_INCLUDE_SUBDIRECTORY(uhd)
endif(ENABLE_UHD)

########################################################################
# Setup MiriSDR component
########################################################################
GR_REGISTER_COMPONENT("Osmocom MiriSDR" ENABLE_MIRI LIBMIRISDR_FOUND)
if(ENABLE_MIRI)
GR_INCLUDE_SUBDIRECTORY(miri)
endif(ENABLE_MIRI)

########################################################################
# Setup HackRF component
########################################################################
GR_REGISTER_COMPONENT("HackRF Jawbreaker" ENABLE_HACKRF LIBHACKRF_FOUND)
if(ENABLE_HACKRF)
GR_INCLUDE_SUBDIRECTORY(hackrf)
endif(ENABLE_HACKRF)

########################################################################
# Setup bladeRF component
########################################################################
GR_REGISTER_COMPONENT("nuand bladeRF" ENABLE_BLADERF LIBBLADERF_FOUND)
if(ENABLE_BLADERF)
GR_INCLUDE_SUBDIRECTORY(bladerf)
endif(ENABLE_BLADERF)

########################################################################
# Setup RFSPACE component
########################################################################
GR_REGISTER_COMPONENT("RFSPACE Receivers" ENABLE_RFSPACE)
if(ENABLE_RFSPACE)
GR_INCLUDE_SUBDIRECTORY(rfspace)
endif(ENABLE_RFSPACE)

########################################################################
# Setup AIRSPY component
########################################################################
GR_REGISTER_COMPONENT("AIRSPY Receiver" ENABLE_AIRSPY LIBAIRSPY_FOUND)
if(ENABLE_AIRSPY)
GR_INCLUDE_SUBDIRECTORY(airspy)
endif(ENABLE_AIRSPY)

########################################################################
# Setup configuration file
########################################################################
ADD_DEFINITIONS(-DHAVE_CONFIG_H=1)
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})
CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
@ONLY)

########################################################################
# Set up Windows DLL resource files
########################################################################
IF(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-osmosdr.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-osmosdr.rc
    @ONLY)

    GR_OSMOSDR_APPEND_SRCS(${CMAKE_CURRENT_BINARY_DIR}/gnuradio-osmosdr.rc)
ENDIF(MSVC)

########################################################################
# Setup libgnuradio-osmosdr library
########################################################################
ADD_LIBRARY(gnuradio-osmosdr SHARED ${gr_osmosdr_srcs})
TARGET_LINK_LIBRARIES(gnuradio-osmosdr ${gr_osmosdr_libs})
SET_TARGET_PROPERTIES(gnuradio-osmosdr PROPERTIES DEFINE_SYMBOL "gnuradio_osmosdr_EXPORTS")
GR_LIBRARY_FOO(gnuradio-osmosdr)
