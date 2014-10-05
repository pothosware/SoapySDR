/* -*- c++ -*- */
/*
 * Copyright 2009-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_THREAD_H
#define INCLUDED_THREAD_H

#include <osmosdr/api.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition_variable.hpp>
#include <vector>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#endif

namespace gr {
  namespace thread {

    typedef boost::thread                    thread;
    typedef boost::mutex                     mutex;
    typedef boost::unique_lock<boost::mutex> scoped_lock;
    typedef boost::condition_variable        condition_variable;

    /*! \brief a system-dependent typedef for the underlying thread type.
     */
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    typedef HANDLE gr_thread_t;
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
    typedef pthread_t gr_thread_t;
#else
    typedef pthread_t gr_thread_t;
#endif


  } /* namespace thread */
} /* namespace gr */

#endif /* INCLUDED_THREAD_H */
