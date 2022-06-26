// Copyright (c) 2022 Nicholas Corgan
// SPDX-License-Identifier: BSL-1.0

#include "Stream.hpp"

#include <stdexcept>

namespace SoapySDR { namespace Java {

Stream::Stream(
    SoapySDR::Device *device,
    const int direction,
    const std::string &format,
    const std::vector<size_t> &channels,
    const SoapySDR::Kwargs &args
):
    _device(device),
    _stream(_device->setupStream(direction, format, channels, args)),
    _format(format),
    _channels(channels),
    _args(args),
    _active(false)
{}

Stream::~Stream(void)
{
    if(_active) this->deactivate();
}

ErrorCode Stream::activate(
    const int flags,
    const long long timeNs,
    const size_t numElems)
{
    assert(_device);

    if(_active) throw std::runtime_error("Device already active");

    const auto ret = _device->activateStream(
        _stream,
        flags,
        timeNs,
        numElems);

    if(ret == 0) _active = true;

    return ErrorCode(ret);
}

ErrorCode Stream::deactivate(
    const int flags,
    const long long timeNs)
{
    assert(_device);

    if(not _active) throw std::runtime_error("Device already inactive");

    const auto ret = _device->deactivateStream(
        _stream,
        flags,
        timeNs);

    if(ret == 0) _active = false;

    return ErrorCode(ret);
}

}}
