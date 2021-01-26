#include "Demos/MazeGenerator.h"
#include "Testing/LabyrinthEscapeTests.h"
using namespace std;

/* Change this constant to contain your name (and your partner's name, if you're
 * working in a pair).
 *
 * WARNING: Once you've set set this constant and started exploring your maze,
 * do NOT edit the value of kYourName. Changing kYourName will change which
 * maze you get back, which might invalidate all your hard work!
 */
const string kYourName = "WangXiaohu";

/* Change these constants to contain the paths out of your mazes. */
const string kPathOutOfNormalMaze = "EESSENNSSWSWWENWN";
const string kPathOutOfTwistyMaze = "ESEWWSENNNWWW";

ADD_TEST("Provided Test: Escape from the regular labyrinth!") {
    /* A maze for you to escape from. This maze will be personalized
     * based on the constant kYourName.
     */
    MazeCell* startLocation = mazeFor(kYourName);

    /* Set a breakpoint on the next line. As a reminder, the labyrinth you
     * get will be personalized to you, so don't start exploring the labyrinth
     * unless you've edited the constant kYourName to include your name(s)!
     * Otherwise, you'll be escaping the wrong labyrinth.
     *
     * This is the labyrinth you'll escape from in Milestone Two. The maze
     * will be in the shape of a regular grid, with pointers pointing in the
     * directions you expect them to point.
     */
    EXPECT(isPathToFreedom(startLocation, kPathOutOfNormalMaze));

    /* We are aware that we didn't free any of the memory allocated in this maze,
     * causing a memory leak. Since this is purely for educational purposes,
     * we're going to let that slide, but you should generally not write code
     * like this.
     */
}





ADD_TEST("Provided Test: Escape from the twisty labyrinth!") {
    MazeCell* startLocation = twistyMazeFor(kYourName);

    /* This test case is for Milestone 3.
     *
     *  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     *  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     *  >>>> Do not start working on this section until you've read the <<<<
     *  >>>> instructions for Milestone 3. The maze here is structured  <<<<
     *  >>>> differently than the maze in Milestone 2, and escaping it  <<<<
     *  >>>> requires a different set of skills.                        <<<<
     *  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     *  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     *
     * This is the labyrinth you'll escape from in Milestone Three. The
     * maze is not necessarily a rectangular grid, and the pointers in each
     * maze cell might not point in the direction they're labeled as pointing.
     * However, you can rely on the fact that if one MazeCell links to a
     * second, the second (somehow) links directly back to the first.
     */
    EXPECT(isPathToFreedom(startLocation, kPathOutOfTwistyMaze));

    /* We are aware that we didn't free any of the memory allocated in this maze,
     * causing a memory leak. Since this is purely for educational purposes,
     * we're going to let that slide, but you should generally not write code
     * like this.
     */
}
