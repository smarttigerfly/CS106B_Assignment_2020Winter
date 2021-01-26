#ifndef Labyrinth_Included
#define Labyrinth_Included

#include "Demos/MemoryDiagnostics.h"
#include <string>

/**
 * Type representing a cell in the maze.
 */
struct MazeCell {
    std::string whatsHere; // One of "", "Potion", "Spellbook", and "Wand"

    MazeCell* north;
    MazeCell* south;
    MazeCell* east;
    MazeCell* west;

    TRACK_ALLOCATIONS_OF(MazeCell);
};

/**
 * Given a location in a maze, returns whether the given sequence of
 * steps will let you escape the maze.
 *
 * To escape the maze, you need to find the Potion, the Spellbook, and
 * the Wand. You can only take steps in the four cardinal directions,
 * and you can't move in directions that don't exist in the maze.
 */
bool isPathToFreedom(MazeCell* start, const std::string& moves);

#endif
