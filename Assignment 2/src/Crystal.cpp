/* File: Crystal.cpp
 *
 * TODO: Delete this comment and replace it with something more insightful about how your
 * program works.
 */
#include "Crystal.h"
#include "Testing/CrystalTests.h"
using namespace std;

void crystallizeAt(Crystal& crystal, int x, int y) {
    /* TODO: Delete the next few lines and implement this function. */
    (void) crystal;
    (void) x;
    (void) y;
}

void step(Crystal& crystal) {
    /* TODO: Delete the next line and implement this function. */
    (void) crystal;
}



/* * * * * * Tests Below This Point * * * * * */

ADD_TEST("Crystalizing at (0, 0) works correctly.") {
    Crystal crystal;
    crystallizeAt(crystal, 0, 0);

    EXPECT(crystal.used.size() == 1);
    EXPECT(crystal.used.contains({0, 0}));
    EXPECT(crystal.sites.size() == 4);
}

/* TODO: Delete this comment and add more of your own tests here! */
