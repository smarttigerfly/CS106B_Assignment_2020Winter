#ifndef Crystal_Included
#define Crystal_Included

#include "hashset.h"
#include "queue.h"
#include "point.h"

/**
 * Type: Crystal
 *
 * A type representing a crystal that grows in two-dimensional space.
 */
struct Crystal {
    /* Locations where crystal has deposited so far. */
    HashSet<Point> used;

    /* List of locations where the crystal hasn't yet grown, but which could serve
     * as a spot for the crystal to expand.
     */
    Queue<Point> sites;
};

/**
 * Adds a crystal at the given location. The neighbors of that location in the cardinal
 * directions are then marked as active sites.
 *
 * @param crystal The current state of the crystal.
 * @param x     Location of the crystallization.
 * @param y     Location of the crystallization.
 */
void crystallizeAt(Crystal& crystal, int x, int y);

/**
 * Advances the simulation of the growing crystal forward one step. All sites that
 * were in the queue on entry to this function will be processed, and the queue will
 * end up filled with all of the active sites that were generated as a result.
 *
 * @param crystal The current state of the crystal.
 */
void step(Crystal& crystal);

#endif
