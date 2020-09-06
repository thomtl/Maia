#include <Maia/math/misc.hpp>

size_t log2i(size_t x){
    return (x == 0) ? 0 : ((8 * sizeof(size_t)) - __builtin_clz(x) - 1);
}