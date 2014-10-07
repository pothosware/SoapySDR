//
// Copyright 2011-2011 Ettus Research LLC
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

#include <osmosdr/ranges.h>
#include <stdexcept>
#include <boost/math/special_functions/round.hpp>
#include <boost/foreach.hpp>
#include <algorithm>
#include <sstream>

using namespace osmosdr;

/***********************************************************************
 * range_t implementation code
 **********************************************************************/
struct range_t::impl{
    impl(double start, double stop, double step):
        start(start), stop(stop), step(step)
    {
        /* NOP */
    }
    double start, stop, step;
};

range_t::range_t(double value):
    _impl(OSMOSDR_PIMPL_MAKE(impl, (value, value, 0)))
{
    /* NOP */
}

range_t::range_t(
    double start, double stop, double step
):
    _impl(OSMOSDR_PIMPL_MAKE(impl, (start, stop, step)))
{
    if (stop < start){
        throw std::runtime_error("cannot make range where stop < start");
    }
}

double range_t::start(void) const{
    return _impl->start;
}

double range_t::stop(void) const{
    return _impl->stop;
}

double range_t::step(void) const{
    return _impl->step;
}

const std::string range_t::to_pp_string(void) const{
    std::stringstream ss;
    ss << "(" << this->start();
    if (this->start() != this->stop()) ss << ", " << this->stop();
    if (this->step() != 0) ss << ", " << this->step();
    ss << ")";
    return ss.str();
}

/***********************************************************************
 * meta_range_t implementation code
 **********************************************************************/
void check_meta_range_monotonic(const meta_range_t &mr){
    if (mr.empty()){
        throw std::runtime_error("meta-range cannot be empty");
    }
    for (size_t i = 1; i < mr.size(); i++){
        if (mr.at(i).start() < mr.at(i-1).stop()){
            throw std::runtime_error("meta-range is not monotonic");
        }
    }
}

meta_range_t::meta_range_t(void){
    /* NOP */
}

meta_range_t::meta_range_t(
    double start, double stop, double step
):
    std::vector<range_t > (1, range_t(start, stop, step))
{
    /* NOP */
}

double meta_range_t::start(void) const{
    check_meta_range_monotonic(*this);
    double min_start = this->front().start();
    BOOST_FOREACH(const range_t &r, (*this)){
        min_start = std::min(min_start, r.start());
    }
    return min_start;
}

double meta_range_t::stop(void) const{
    check_meta_range_monotonic(*this);
    double max_stop = this->front().stop();
    BOOST_FOREACH(const range_t &r, (*this)){
        max_stop = std::max(max_stop, r.stop());
    }
    return max_stop;
}

double meta_range_t::step(void) const{
    check_meta_range_monotonic(*this);
    std::vector<double> non_zero_steps;
    range_t last = this->front();
    BOOST_FOREACH(const range_t &r, (*this)){
        //steps at each range
        if (r.step() > 0) non_zero_steps.push_back(r.step());
        //and steps in-between ranges
        double ibtw_step = r.start() - last.stop();
        if (ibtw_step > 0) non_zero_steps.push_back(ibtw_step);
        //store ref to last
        last = r;
    }
    if (non_zero_steps.empty()) return 0; //all zero steps, its zero...
    return *std::min_element(non_zero_steps.begin(), non_zero_steps.end());
}

double meta_range_t::clip(double value, bool clip_step) const{
    check_meta_range_monotonic(*this);
    double last_stop = this->front().stop();
    BOOST_FOREACH(const range_t &r, (*this)){
        //in-between ranges, clip to nearest
        if (value < r.start()){
            return (std::abs(value - r.start()) < std::abs(value - last_stop))?
                r.start() : last_stop;
        }
        //in this range, clip here
        if (value <= r.stop()){
            if (! clip_step || r.step() == 0) return value;
            return boost::math::round((value - r.start())/r.step())*r.step() + r.start();
        }
        //continue on to the next range
        last_stop = r.stop();
    }
    return last_stop;
}

std::vector<double> meta_range_t::values() const {
    std::vector<double> values;

    BOOST_FOREACH(const range_t &r, (*this)) {
        if (r.start() != r.stop()) {
            if ( r.step() == 0 ) {
                values.push_back( r.start() );
                values.push_back( r.stop() );
            } else {
                for ( double val = r.start(); val <= r.stop(); val += r.step() ) {
                    values.push_back( val );
                }
            }
        } else {
            values.push_back( r.start() );
        }
    }

    return values;
}

const std::string meta_range_t::to_pp_string(void) const{
    std::stringstream ss;
    BOOST_FOREACH(const range_t &r, (*this)){
        ss << r.to_pp_string() << std::endl;
    }
    return ss.str();
}
