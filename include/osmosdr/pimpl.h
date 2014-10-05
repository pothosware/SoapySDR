//
// Copyright 2010 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef INCLUDED_OSMOSDR_PIMPL_H
#define INCLUDED_OSMOSDR_PIMPL_H

#include <boost/shared_ptr.hpp>

/*! \file pimpl.h
 * "Pimpl idiom" (pointer to implementation idiom).
 * The OSMOSDR_PIMPL_* macros simplify code overhead for declaring and making pimpls.
 *
 * Each pimpl is implemented as a shared pointer to the implementation:
 * - The container class will not have to deallocate the pimpl.
 * - The container class will use the pimpl as a regular pointer.
 * - Usage: _impl->method(arg0, arg1)
 * - Usage: _impl->member = value;
 *
 * \see http://en.wikipedia.org/wiki/Opaque_pointer
 */

/*!
 * Make a declaration for a pimpl in a header file.
 * - Usage: OSMOSDR_PIMPL_DECL(impl) _impl;
 * \param _name the name of the pimpl class
 */
#define OSMOSDR_PIMPL_DECL(_name) \
    struct _name; boost::shared_ptr<_name>

/*!
 * Make an instance of a pimpl in a source file.
 * - Usage: _impl = OSMOSDR_PIMPL_MAKE(impl, ());
 * - Usage: _impl = OSMOSDR_PIMPL_MAKE(impl, (a0, a1));
 * \param _name the name of the pimpl class
 * \param _args the constructor args for the pimpl
 */
#define OSMOSDR_PIMPL_MAKE(_name, _args) \
    boost::shared_ptr<_name>(new _name _args)

#endif /* INCLUDED_OSMOSDR_PIMPL_H */
