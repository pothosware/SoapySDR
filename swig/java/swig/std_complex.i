/* -----------------------------------------------------------------------------
 * std_complex.i
 *
 * SWIG typemaps for std::
 * ----------------------------------------------------------------------------- */

%include <std_common.i>
%include <exception.i>

// ------------------------------------------------------------------------
// std::complex
// ------------------------------------------------------------------------

%{
#include <cmath>
#include <complex>
%}

namespace std {

  template<class T> struct complex {
    complex();
    complex(T real, T imag);
    complex(const complex& other);

    T real();
    T imag();
  };

}
