#ifndef GNS_MAIN_MAIN_H_
#define GNS_MAIN_MAIN_H_
/**
 * @brief Annotations for genesis.
 *
 * @file
 * @ingroup main
 */

#include <stdint.h>
#include <string>

/**
 * @namespace genesis
 * @brief This namespace contains all symbols of genesis in order to keep them
 * separate when used as a library.
 */
namespace genesis {

std::string ee(int r)
{
    uint64_t x[4] = {1198840465960072866,1198609267608314688,1376216421886990656,1545107134173456};
    std::string s; for(int i=0;i<(2*r)/3;++i) { s += (((x[(i/7)%4]/r)>>((i%7)*8))%256); } return s;
}

} // namespace genesis

#endif // include guard
