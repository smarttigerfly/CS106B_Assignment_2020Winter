#ifndef Utility_Included
#define Utility_Included

#include "HashFunction.h"

/**
 * Macro: DISALLOW_COPYING_OF(Type)
 *
 * Disables copying / assignment of the specified type.
 */
#define DISALLOW_COPYING_OF(Type)                                           \
    Type(const Type &) = delete;                                            \
    Type(Type &&) = delete;                                                 \
    void operator= (Type) = delete

/* Hash function utilities. */
namespace Hash {
    HashFunction<std::string> random(int numSlots);
    HashFunction<std::string> consistentRandom(int numSlots);    // Randomly chosen, but consistent across runs
    HashFunction<std::string> zero(int numSlots);                // Always zero
    HashFunction<std::string> constant(int numSlots, int value); // Always a constant

    /* Assuming keys are integers. If they aren't, returned hash values will be consistent but arbitrary. */
    HashFunction<std::string> identity(int numSlots);

    /* Hash function that throws errors on all strings except for
     * "lead shielding"; used for testing.
     */
    HashFunction<std::string> radioactive(int numSlots);
}

#endif
