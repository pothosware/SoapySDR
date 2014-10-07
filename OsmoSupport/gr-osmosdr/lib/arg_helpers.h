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

#ifndef OSMOSDR_ARG_HELPERS_H
#define OSMOSDR_ARG_HELPERS_H

#include <iostream>
#include <vector>
#include <map>

#include <gnuradio/io_signature.h>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

typedef std::map< std::string, std::string > dict_t;
typedef std::pair< std::string, std::string > pair_t;

inline std::vector< std::string > args_to_vector( const std::string &args )
{
  std::vector< std::string > result;

  boost::escaped_list_separator<char> separator("\\", " ", "'");
  typedef boost::tokenizer< boost::escaped_list_separator<char> > tokenizer_t;
  tokenizer_t tokens(args, separator);

  BOOST_FOREACH(std::string token, tokens)
    result.push_back(token);

  return result;
}

inline std::vector< std::string > params_to_vector( const std::string &params )
{
  std::vector< std::string > result;

  boost::escaped_list_separator<char> separator("\\", ",", "'");
  typedef boost::tokenizer< boost::escaped_list_separator<char> > tokenizer_t;
  tokenizer_t tokens(params, separator);

  BOOST_FOREACH(std::string token, tokens)
    result.push_back(token);

  return result;
}

inline pair_t param_to_pair( const std::string &param )
{
  pair_t result;

  std::size_t pos = param.find('=');
  if(pos != std::string::npos)
  {
    result.first = param.substr(0, pos);
    result.second = param.substr(pos + 1);
  }
  else
  {
    result.first = param;
    result.second = "";
  }

  return result;
}

inline dict_t params_to_dict( const std::string &params )
{
  dict_t result;

  std::vector< std::string > param_list = params_to_vector( params );
  BOOST_FOREACH(std::string param, param_list)
  {
    pair_t pair = param_to_pair( param );
    std::string value = pair.second;
    if (value.length() && value[0] == '\'' && value[ value.length() - 1 ] == '\'')
      value = value.substr(1, value.length() - 1);
    result[ pair.first ] = value;
  }

  return result;
}

struct is_nchan_argument
{
  bool operator ()(const std::string &str)
  {
    return str.find("numchan=") == 0;
  }
};

inline gr::io_signature::sptr args_to_io_signature( const std::string &args )
{
  size_t max_nchan = 0;
  size_t dev_nchan = 0;
  std::vector< std::string > arg_list = args_to_vector( args );

  BOOST_FOREACH( std::string arg, arg_list )
  {
    if ( arg.find( "numchan=" ) == 0 ) // try to parse global nchan value
    {
      pair_t pair = param_to_pair( arg );
      max_nchan = boost::lexical_cast<size_t>( pair.second );
    }
  }

  arg_list.erase( std::remove_if( // remove any global nchan tokens
                    arg_list.begin(),
                    arg_list.end(),
                    is_nchan_argument() ),
                  arg_list.end() );

  // try to parse device specific nchan values, assume 1 channel if none given

  BOOST_FOREACH( std::string arg, arg_list )
  {
    dict_t dict = params_to_dict(arg);
    if (dict.count("nchan"))
    {
      dev_nchan += boost::lexical_cast<size_t>( dict["nchan"] );
    }
    else // no channels given via args
    {
      dev_nchan++; // assume one channel
    }
  }

  // if at least one nchan was given, perform a sanity check
  if ( max_nchan && dev_nchan && max_nchan != dev_nchan )
    throw std::runtime_error("Wrong device arguments specified. Missing nchan?");

  const size_t nchan = std::max<size_t>(dev_nchan, 1); // assume at least one
  return gr::io_signature::make(nchan, nchan, sizeof(gr_complex));
}

#endif // OSMOSDR_ARG_HELPERS_H
