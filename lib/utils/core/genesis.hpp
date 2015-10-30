#ifndef GENESIS_UTILS_CORE_GENESIS_H_
#define GENESIS_UTILS_CORE_GENESIS_H_

/**
 * @brief Some stuff that is totally not imporatant, but nice.
 *
 * @file
 * @ingroup utils
 */

#include <stdint.h>
#include <string>

/**
 * @namespace genesis
 *
 * @brief This namespace contains all symbols of genesis in order to keep them
 * separate when used as a library.
 */
namespace genesis {

/**
 * @brief Returns the header for genesis.
 */
inline std::string genesis_header()
{
    return "\
                                     ,     \n\
        __    __    __    __   __     __   \n\
      /   ) /___) /   ) /___) (_ ` / (_ `  \n\
     (___/ (___  /   / (___  (__) / (__)   \n\
        /                                  \n\
    (__/       2014-2015 by Lucas Czech    \n";
}

inline std::string ee(int r)
{
    uint64_t x[4] = {1198840465960072866,1198609267608314688,1376216421886990656,1545107134173456};
    std::string s; for(int i=0;i<(2*r)/3;++i) { s += (((x[(i/7)%4]/r)>>((i%7)*8))%256); } return s;
}

} // namespace genesis

#endif // include guard
