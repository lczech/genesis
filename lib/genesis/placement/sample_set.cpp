/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief Implementation of SampleSet class.
 *
 * @file
 * @ingroup placement
 */

#include "genesis/placement/sample_set.hpp"

#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/operators.hpp"
#include "genesis/utils/core/logging.hpp"

#include <sstream>

namespace genesis {
namespace placement {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

void SampleSet::swap( SampleSet& other )
{
    using std::swap;
    swap( smps_, other.smps_ );
}

// =================================================================================================
//     Modifiers
// =================================================================================================

void SampleSet::add( Sample const& smp )
{
    smps_.push_back( { "", smp } );
}

void SampleSet::add( Sample const& smp, std::string const& name )
{
    smps_.push_back( { name, smp } );
}

void SampleSet::add( Sample&& smp )
{
    smps_.push_back( { "", std::move(smp) } );
}

void SampleSet::add( Sample&& smp, std::string const& name )
{
    smps_.push_back( { name, std::move(smp) } );
}

void SampleSet::remove_at( size_t index )
{
    smps_.erase( smps_.begin() + index );
}

void SampleSet::clear ()
{
    smps_.clear();
}

// =================================================================================================
//     Accessors
// =================================================================================================

SampleSet::iterator SampleSet::begin()
{
    return smps_.begin();
}

SampleSet::iterator SampleSet::end()
{
    return smps_.end();
}

SampleSet::const_iterator SampleSet::begin() const
{
    return smps_.cbegin();
}

SampleSet::const_iterator SampleSet::end() const
{
    return smps_.cend();
}

SampleSet::NamedSample& SampleSet::at ( size_t index )
{
    return smps_.at(index);
}
SampleSet::NamedSample const& SampleSet::at ( size_t index ) const
{
    return smps_.at(index);
}

SampleSet::NamedSample& SampleSet::operator [] ( size_t index )
{
    return smps_[index];
}

SampleSet::NamedSample const& SampleSet::operator [] ( size_t index ) const
{
    return smps_[index];
}


bool SampleSet::empty() const
{
    return smps_.empty();
}

size_t SampleSet::size() const
{
    return smps_.size();
}

} // namespace placement
} // namespace genesis
