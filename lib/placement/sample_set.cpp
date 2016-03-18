/**
 * @brief Implementation of SampleSet class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/sample_set.hpp"

#include "placement/function/functions.hpp"
#include "placement/function/operators.hpp"
#include "utils/core/logging.hpp"

#include <sstream>

namespace genesis {
namespace placement {

// =============================================================================
//     Constructors and Rule of Five
// =============================================================================

void SampleSet::swap( SampleSet& other )
{
    using std::swap;
    swap( smps_, other.smps_ );
}

// =============================================================================
//     Modifiers
// =============================================================================

void SampleSet::add( std::string const& name, Sample const& smp)
{
    smps_.push_back( { name, smp } );
}

void SampleSet::remove_at( size_t index )
{
    smps_.erase( smps_.begin() + index );
}

/**
 * @brief
 */
void SampleSet::clear ()
{
    smps_.clear();
}

// =============================================================================
//     Accessors
// =============================================================================

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

/**
 * @brief Return whether the SampleSet is empty.
 */
bool SampleSet::empty() const
{
    return smps_.empty();
}

/**
 * @brief Return the size of the SampleSet, i.e., the number of Sample%s.
 */
size_t SampleSet::size() const
{
    return smps_.size();
}

} // namespace placement
} // namespace genesis
