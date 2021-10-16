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

%extend complex
{
    static complex fromPolar(T r, T theta)
    {
        return std::polar(r, theta);
    }

    T abs()
    {
        return std::abs(*self);
    }

    T arg()
    {
        return std::arg(*self);
    }

    T norm()
    {
        return std::norm(*self);
    }

    complex conj()
    {
        return std::conj(*self);
    }

    complex proj()
    {
        return std::proj(*self);
    }

    complex exp()
    {
        return std::exp(*self);
    }

    complex log()
    {
        return std::log(*self);
    }

    complex log10()
    {
        return std::log10(*self);
    }

    complex pow(T exp)
    {
        return std::pow(*self, exp);
    }

    complex pow(const complex& exp)
    {
        return std::pow(*self, exp);
    }

    complex sqrt()
    {
        return std::sqrt(*self);
    }

    complex cos()
    {
        return std::cos(*self);
    }

    complex sin()
    {
        return std::sin(*self);
    }

    complex tan()
    {
        return std::tan(*self);
    }

    complex acos()
    {
        return std::acos(*self);
    }

    complex asin()
    {
        return std::asin(*self);
    }

    complex atan()
    {
        return std::atan(*self);
    }

    complex cosh()
    {
        return std::cosh(*self);
    }

    complex sinh()
    {
        return std::sinh(*self);
    }

    complex tanh()
    {
        return std::cos(*self);
    }

    complex acosh()
    {
        return std::acosh(*self);
    }

    complex asinh()
    {
        return std::asinh(*self);
    }

    complex atanh()
    {
        return std::atanh(*self);
    }
}

}
