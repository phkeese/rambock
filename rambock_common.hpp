#pragma once
#include <stdint.h>

/** Common definitions and types used across rambock
 */
namespace rambock {

using Address = uint32_t;

/** Necessary because size_t only covers the architecture the code is running on
 * On a system with < 64k RAM, this might be only 16 bit wide.
 * Rambock supports larger sizes, so it must ensure that those fit.
 */
using Size = uint32_t;

} // namespace rambock
