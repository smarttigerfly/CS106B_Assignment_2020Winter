#include "Labyrinth.h"
#include "Demos/MazeGenerator.h"
#include "Testing/LabyrinthTests.h"
using namespace std;

bool isPathToFreedom(MazeCell* start, const string& moves) {
    /* TODO: Delete this comment and the next few lines, then implement
     * this function.
     */

     MazeCell* currentM = start;
     bool hasSpellbook = false;
     bool hasPotion = false;
     bool hasWand = false;

    for(int i = 0; i < moves.length(); i++)
    {
        if(currentM == nullptr)
            return false;
        if(currentM->whatsHere == "Spellbook")
            hasSpellbook = true;
        if(currentM->whatsHere == "Potion")
            hasPotion = true;
        if(currentM->whatsHere == "Wand")
            hasWand = true;
        switch (moves[i]) {
        case 'N':
            currentM = currentM->north;
            break;
        case 'S':
            currentM = currentM->south;
            break;
        case 'E':
            currentM = currentM->east;
            break;
        case 'W':
            currentM = currentM->west;
            break;
        default:
            error("The path is illegal!");
            return false;
            break;
        }
    }

    //The last char
    if(currentM == nullptr)
        return false;
    if(currentM->whatsHere == "Spellbook")
        hasSpellbook = true;
    if(currentM->whatsHere == "Potion")
        hasPotion = true;
    if(currentM->whatsHere == "Wand")
        hasWand = true;

    if(hasSpellbook && hasPotion && hasWand)
        return true;
    else
        return false;
}


/* * * * * * Test Cases Below This Point * * * * * */

/* Optional: Add your own custom tests here! */














/* * * * * Provided Tests Below This Point * * * * */

/* Utility function to free all memory allocated for a maze. */
void deleteMaze(const Grid<MazeCell*>& maze) {
    for (auto* elem: maze) {
        delete elem;
    }
    /* Good question to ponder: why don't we delete maze, or empty maze, or something
     * along those lines?
     */
}

ADD_TEST("Provided Test: Checks paths in the sample maze.") {
    /* Make sure we don't leak any memory. */
    int allocs = instancesOf("MazeCell");

    auto maze = toMaze({"* *-W *",
                        "| |   |",
                        "*-* * *",
                        "  | | |",
                        "S *-*-*",
                        "|   | |",
                        "*-*-* P"});

    /* These paths are the ones in the handout. They all work. */
    EXPECT(isPathToFreedom(maze[2][2], "ESNWWNNEWSSESWWN"));
    EXPECT(isPathToFreedom(maze[2][2], "SWWNSEENWNNEWSSEES"));
    EXPECT(isPathToFreedom(maze[2][2], "WNNEWSSESWWNSEENES"));

    /* These paths don't work, since they don't pick up all items. */
    EXPECT(!isPathToFreedom(maze[2][2], "ESNW"));
    EXPECT(!isPathToFreedom(maze[2][2], "SWWN"));
    EXPECT(!isPathToFreedom(maze[2][2], "WNNE"));

    /* These paths don't work, since they aren't legal paths. */
    EXPECT(!isPathToFreedom(maze[2][2], "WW"));
    EXPECT(!isPathToFreedom(maze[2][2], "NN"));
    EXPECT(!isPathToFreedom(maze[2][2], "EE"));
    EXPECT(!isPathToFreedom(maze[2][2], "SS"));

    deleteMaze(maze);

    EXPECT_EQUAL(allocs, instancesOf("MazeCell"));
}

ADD_TEST("Provided Test: Can't walk through walls.") {
    int allocs = instancesOf("MazeCell");

    auto maze = toMaze({"* S *",
                        "     ",
                        "W * P",
                        "     ",
                        "* * *"});

    EXPECT(!isPathToFreedom(maze[1][1], "WNEES"));
    EXPECT(!isPathToFreedom(maze[1][1], "NWSEE"));
    EXPECT(!isPathToFreedom(maze[1][1], "ENWWS"));
    EXPECT(!isPathToFreedom(maze[1][1], "SWNNEES"));

    deleteMaze(maze);

    EXPECT_EQUAL(allocs, instancesOf("MazeCell"));
}

ADD_TEST("Provided Test: Works when starting on an item.") {
    int allocs = instancesOf("MazeCell");

    auto maze = toMaze({"P-S-W"});

    EXPECT(isPathToFreedom(maze[0][0], "EE"));
    EXPECT(isPathToFreedom(maze[0][1], "WEE"));
    EXPECT(isPathToFreedom(maze[0][2], "WW"));

    deleteMaze(maze);

    EXPECT_EQUAL(allocs, instancesOf("MazeCell"));
}

ADD_TEST("Provided Test: Reports errors if given illegal characters.") {
    int allocs = instancesOf("MazeCell");

    auto maze = toMaze({"* *-W *",
                        "| |   |",
                        "*-* * *",
                        "  | | |",
                        "S *-*-*",
                        "|   | |",
                        "*-*-* P"});

    /* These paths contain characters that aren't even close to permissible. */
    EXPECT_ERROR(isPathToFreedom(maze[0][0], "Q"));
    EXPECT_ERROR(isPathToFreedom(maze[0][0], "X"));
    EXPECT_ERROR(isPathToFreedom(maze[0][0], "!"));
    EXPECT_ERROR(isPathToFreedom(maze[0][0], "?"));

    EXPECT_ERROR(isPathToFreedom(maze[0][0], "n"));
    EXPECT_ERROR(isPathToFreedom(maze[0][0], "s"));
    EXPECT_ERROR(isPathToFreedom(maze[0][0], "e"));
    EXPECT_ERROR(isPathToFreedom(maze[0][0], "w"));

    /* These are tricky - they're legal paths that happen to have an unexpected
     * character at the end.
     */
    EXPECT_ERROR(isPathToFreedom(maze[2][2], "ESNWWNNEWSSESWWNQ"));
    EXPECT_ERROR(isPathToFreedom(maze[2][2], "SWWNSEENWNNEWSSEES!!!"));
    EXPECT_ERROR(isPathToFreedom(maze[2][2], "WNNEWSSESWWNSEENES??"));

    deleteMaze(maze);

    EXPECT_EQUAL(allocs, instancesOf("MazeCell"));
}
