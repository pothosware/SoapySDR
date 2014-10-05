/* -*- c++ -*- */
/*
 * Copyright 2012 Dimitri Stolnikov <horiz0n@gmx.net>
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_OSMOSDR_DEVICE_H
#define INCLUDED_OSMOSDR_DEVICE_H

#include <osmosdr/api.h>
#include <osmosdr/pimpl.h>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

typedef std::map<std::string, std::string> string_string_dict_t;

namespace osmosdr {

  /*!
   * Mapping of key/value pairs for locating devices on the system.
   * When left empty, the device discovery routines will search
   * all available transports on the system (ethernet, usb...).
   *
   * The logical device can also be used to pass arguments into
   * the transport layer control to set (for example) buffer sizes.
   *
   * An arguments string, is a way to represent a device address
   * using a single string with delimiter characters.
   */
  class OSMOSDR_API device_t : public string_string_dict_t
  {
  public:
    /*!
     * Create a logical device from an args string.
     * \param args the arguments string
     */
    device_t(const std::string &args = "");

    /*!
     * Convert a logical device into a pretty print string.
     * \return a printable string representing the device
     */
    std::string to_pp_string(void) const;

    /*!
     * Convert the logical device into an args string.
     * The args string contains delimiter symbols.
     * \return a string with delimiter markup
     */
    std::string to_string(void) const;

    /*!
     * Lexically cast a parameter to the specified type,
     * or use the default value if the key is not found.
     * \param key the key as one of the parameters
     * \param def the value to use when key is not present
     * \return the casted value as type T or the default
     * \throw error when the parameter cannot be casted
     */
    template <typename T> T cast(const std::string &key, const T &def) const
    {
      if (!this->count(key)) return def;
      try { return boost::lexical_cast<T>(this->at(key)); }
      catch(const boost::bad_lexical_cast &) {
        throw std::runtime_error("cannot cast " + key + " = " + this->at(key));
      }
    }
  };

  //! A typedef for a vector of logical devices
  typedef std::vector<device_t> devices_t;

  /*!
   * The device interface represents the underyling hardware.
   * The api allows for discovery, configuration, and streaming.
   */
  class OSMOSDR_API device : boost::noncopyable
  {
  public:
    /*!
     * \brief Find logical radio devices attached to the host.
     *
     * The device hint should be used to narrow down the search
     * to particular transport types and/or transport arguments.
     *
     * The device hint "nofake" switches off dummy devices created
     * by "file" (and other) implementations.
     *
     * \param hint a partially (or fully) filled in logical device
     * \return a vector of logical devices for all radios on the system
     */
    static devices_t find(const device_t &hint = osmosdr::device_t());
  };

} //namespace osmosdr

#endif /* INCLUDED_OSMOSDR_DEVICE_H */
