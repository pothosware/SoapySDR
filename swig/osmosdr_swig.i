/* -*- c++ -*- */

#define OSMOSDR_API

// suppress Warning 319: No access specifier given for base class 'boost::noncopyable' (ignored).
#pragma SWIG nowarn=319

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "osmosdr_swig_doc.i"

%{
#include "osmosdr/device.h"
#include "osmosdr/source.h"
#include "osmosdr/sink.h"
%}

// Workaround for a SWIG 2.0.4 bug with templates. Probably needs to be looked in to.
%{
#if PY_VERSION_HEX >= 0x03020000
# define SWIGPY_SLICE_ARG(obj) ((PyObject*) (obj))
#else
# define SWIGPY_SLICE_ARG(obj) ((PySliceObject*) (obj))
#endif
%}

%template(string_vector_t) std::vector<std::string>;

//%template(size_vector_t) std::vector<size_t>;

%include <osmosdr/pimpl.h>

%ignore osmosdr::device_t::operator[]; //ignore warnings about %extend

%template(string_string_dict_t) std::map<std::string, std::string>; //define before device
%template(devices_t) std::vector<osmosdr::device_t>;
%include <osmosdr/device.h>

%template(range_vector_t) std::vector<osmosdr::range_t>; //define before range
%include <osmosdr/ranges.h>

%define OSMOSDR_SWIG_BLOCK_MAGIC2(PKG, BASE_NAME)
%template(BASE_NAME ## _sptr) boost::shared_ptr<PKG ## :: ## BASE_NAME>;
%pythoncode %{
BASE_NAME ## _sptr.__repr__ = lambda self: "<gr_block %s (%d)>" % (self.name(), self.unique_id())
BASE_NAME = BASE_NAME.make;
%}
%enddef

%include "osmosdr/source.h"
%include "osmosdr/sink.h"

OSMOSDR_SWIG_BLOCK_MAGIC2(osmosdr,source);
OSMOSDR_SWIG_BLOCK_MAGIC2(osmosdr,sink);
