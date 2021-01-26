#include "DisasterPlanning.h"
#include "Testing/DisasterPlanningTests.h"
using namespace std;

/* TODO: Refer to DisasterPlanning.h for more information about this function.
 * Then, delete this comment.
 */
bool canBeMadeDisasterReady(const HashMap<string, HashSet<string>>& roadNetwork,
                            int numCities,
                            HashSet<string>& supplyLocations) {
    /* TODO: Delete the next few lines and implement this function. */
    if(numCities < 0)
        error("Value of provided numCities was negative");
    return cityBeReady(roadNetwork, numCities, supplyLocations);
}

bool cityBeReady(const HashMap<string, HashSet<string>>& roadNetwork,
                 int numCities,
                 HashSet<string>& supplyLocations)
{
    if(numCities < 0)
        return false;
    if(roadNetwork.isEmpty())
        return true;

    string thisCity = roadNetwork.front();
    HashSet<string> stockCity = {thisCity};
    stockCity += roadNetwork[thisCity];
    HashMap<string, HashSet<string>> nextRoadNet = roadNetwork;
    nextRoadNet.remove(thisCity);
    if(cityHasBeenCovered(stockCity, supplyLocations))
    {
        return cityBeReady(nextRoadNet, numCities,supplyLocations);

    } else {
        for (string key : stockCity)
        {
            supplyLocations.add(key);
            if(cityBeReady(nextRoadNet, --numCities,supplyLocations))
                return true;
            supplyLocations.remove(key);
            numCities++;
        }
        return false;
    }

}

bool cityHasBeenCovered(HashSet<string> city, HashSet<string> supply)
{
    for (string key : city)
    {
        if (supply.contains(key))
        {
              return true;
        }
    }
    return false;
}




/* * * * * * * Test Helper Functions Below This Point * * * * * */

/* This is a helper function that's useful for designing test cases. You give it a HashMap
 * of cities and what they're adjacent to, and it then produces a new HashMap where if city
 * A links to city B, then city B links back to city A. We recommend using this helper
 * function when writing tests, though you won't need it in your implementation of the main
 * canBeMadeDisasterReady function.
 */
HashMap<string, HashSet<string>> makeSymmetric(const HashMap<string, HashSet<string>>& source) {
    HashMap<string, HashSet<string>> result = source;

    for (const string& from: source) {
        for (const string& to: source[from]) {
            result[from] += to;
            result[to] += from;
        }
    }

    return result;
}

/* This helper function tests whether a city has been covered by a set of supply locations
 * and is used by our testing code. You're welcome to use it in your tests as well!
 */
bool isCovered(const string& city,
               const HashMap<string, HashSet<string>>& roadNetwork,
               const HashSet<string>& supplyLocations) {
    if (supplyLocations.contains(city)) return true;

    for (string neighbor: roadNetwork[city]) {
        if (supplyLocations.contains(neighbor)) return true;
    }

    return false;
}

/* * * * * * Test Cases Below This Point * * * * * */

/* TODO: Add your own custom tests here! */














/* * * * * Provided Tests Below This Point * * * * */

//ADD_TEST("Provided Test: Reports an error if numCities < 0") {
//    HashSet<string> supply;
//    EXPECT_ERROR(canBeMadeDisasterReady({}, -137, supply));
//}

//ADD_TEST("Provided Test: Works for map with one city.") {
//    HashMap<string, HashSet<string>> map = makeSymmetric({
//         { "Solipsist", {} }
//    });

//    HashSet<string> locations0, locations1, locations2;
//    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
//    EXPECT(canBeMadeDisasterReady(map, 1, locations1));
//    EXPECT(canBeMadeDisasterReady(map, 2, locations2));
//}

//ADD_TEST("Provided Test: Works for map with one city, and produces output.") {
//    HashMap<string, HashSet<string>> map = makeSymmetric({
//         { "Solipsist", {} }
//    });

//    HashSet<string> locations0, locations1, locations2;
//    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
//    EXPECT(canBeMadeDisasterReady(map, 1, locations1));
//    EXPECT(canBeMadeDisasterReady(map, 2, locations2));

//    /* Don't check locations0; since the function returned false, the values there
//     * can be anything.
//     */
//    HashSet<string> expected = { "Solipsist" };
//    EXPECT_EQUAL(locations1, expected);
//    EXPECT_EQUAL(locations2, expected);
//}

//ADD_TEST("Provided Test: Works for map with two linked cities.") {
//    HashMap<string, HashSet<string>> map = makeSymmetric({
//         { "A", { "B" } },
//         { "B", {     } }
//    });

//    HashSet<string> locations0, locations1, locations2;
//    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
//    EXPECT(canBeMadeDisasterReady(map, 1, locations1));
//    EXPECT(canBeMadeDisasterReady(map, 2, locations2));
//}

//ADD_TEST("Provided Test: Works for map with two linked cities, and produces output.") {
//    HashMap<string, HashSet<string>> map = makeSymmetric({
//         { "A", { "B" } },
//    });

//    HashSet<string> locations0, locations1, locations2;
//    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
//    EXPECT(canBeMadeDisasterReady(map, 1, locations1));
//    EXPECT(canBeMadeDisasterReady(map, 2, locations2));

//    EXPECT_EQUAL(locations1.size(), 1);
//    EXPECT(locations1.isSubsetOf({"A", "B"}));

//    EXPECT(locations2.size() <= 2);
//    EXPECT(locations2.isSubsetOf({"A", "B"}));
//}

//ADD_TEST("Provided Test: Works for four cities in a line.") {
//    HashMap<string, HashSet<string>> map = makeSymmetric({
//         { "A", { "B" } },
//         { "B", { "C" } },
//         { "C", { "D" } }
//    });

//    HashSet<string> locations0, locations1, locations2, locations3, locations4;
//    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
//    EXPECT(!canBeMadeDisasterReady(map, 1, locations1));
//    EXPECT(canBeMadeDisasterReady(map, 2, locations2));
//    EXPECT(canBeMadeDisasterReady(map, 3, locations3));
//    EXPECT(canBeMadeDisasterReady(map, 4, locations4));
//}

//ADD_TEST("Provided Test: Works for four cities in a line, and produces output.") {
//    HashMap<string, HashSet<string>> map = makeSymmetric({
//         { "A", { "B" } },
//         { "B", { "C" } },
//         { "C", { "D" } }
//    });

//    HashSet<string> locations0, locations1, locations2, locations3, locations4;
//    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
//    EXPECT(!canBeMadeDisasterReady(map, 1, locations1));
//    EXPECT(canBeMadeDisasterReady(map, 2, locations2));
//    EXPECT(canBeMadeDisasterReady(map, 3, locations3));
//    EXPECT(canBeMadeDisasterReady(map, 4, locations4));

//    EXPECT(locations2.size() <= 2);
//    EXPECT(locations3.size() <= 3);
//    EXPECT(locations4.size() <= 4);
//    EXPECT(locations2.isSubsetOf({"A", "B", "C", "D"}));
//    EXPECT(locations3.isSubsetOf({"A", "B", "C", "D"}));
//    EXPECT(locations4.isSubsetOf({"A", "B", "C", "D"}));

//    /* Check if locations2 is a solution. (locations3 and locations4 must be) */
//    EXPECT(isCovered("A", map, locations2));
//    EXPECT(isCovered("B", map, locations2));
//    EXPECT(isCovered("C", map, locations2));
//    EXPECT(isCovered("D", map, locations2));
//}

//ADD_TEST("Provided Test: Works for four disconnected cities.") {
//    HashMap<string, HashSet<string>> map = makeSymmetric({
//        { "A", { } },
//        { "B", { } },
//        { "C", { } },
//        { "D", { } }
//    });

//    HashSet<string> locations0, locations1, locations2, locations3, locations4;
//    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
//    EXPECT(!canBeMadeDisasterReady(map, 1, locations1));
//    EXPECT(!canBeMadeDisasterReady(map, 2, locations2));
//    EXPECT(!canBeMadeDisasterReady(map, 3, locations3));
//    EXPECT(canBeMadeDisasterReady(map, 4, locations4));
//}

//ADD_TEST("Provided Test: Works for four disconnected cities, and produces output.") {
//    HashMap<string, HashSet<string>> map = makeSymmetric({
//        { "A", { } },
//        { "B", { } },
//        { "C", { } },
//        { "D", { } }
//    });

//    HashSet<string> locations0, locations1, locations2, locations3, locations4;
//    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
//    EXPECT(!canBeMadeDisasterReady(map, 1, locations1));
//    EXPECT(!canBeMadeDisasterReady(map, 2, locations2));
//    EXPECT(!canBeMadeDisasterReady(map, 3, locations3));
//    EXPECT(canBeMadeDisasterReady(map, 4, locations4));

//    HashSet<string> expected = { "A", "B", "C", "D" };
//    EXPECT_EQUAL(locations4, expected);
//}

ADD_TEST("Provided Test: Works on a 3x3 grid.") {
    /* Make a 3x3 grid of cities. */
    HashMap<string, HashSet<string>> map = makeSymmetric({
        { "A1", { "A2", "B1" } },
        { "A2", { "A3", "B2" } },
        { "A3", { "B3"       } },
        { "B1", { "B2", "C1" } },
        { "B2", { "B3", "C2" } },
        { "B3", { "C3"       } },
        { "C1", { "C2" } },
        { "C2", { "C3" } },
    });

    HashSet<string> locations0, locations1, locations2, locations3;
    //EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
    //EXPECT(!canBeMadeDisasterReady(map, 1, locations0));
    //EXPECT(!canBeMadeDisasterReady(map, 2, locations0));
    EXPECT( canBeMadeDisasterReady(map, 3, locations0));
}

ADD_TEST("Provided Test: Works on a 3x3 grid, and produces output.") {
    /* Make a 3x3 grid of cities. */
    HashMap<string, HashSet<string>> map = makeSymmetric({
        { "A1", { "A2", "B1" } },
        { "A2", { "A3", "B2" } },
        { "A3", { "B3"       } },
        { "B1", { "B2", "C1" } },
        { "B2", { "B3", "C2" } },
        { "B3", { "C3"       } },
        { "C1", { "C2" } },
        { "C2", { "C3" } },
    });

    HashSet<string> locations0, locations1, locations2, locations3;
    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
    EXPECT(!canBeMadeDisasterReady(map, 1, locations1));
    EXPECT(!canBeMadeDisasterReady(map, 2, locations2));
    EXPECT( canBeMadeDisasterReady(map, 3, locations3));

    /* Check that we have a solution. */
    EXPECT(isCovered("A1", map, locations3));
    EXPECT(isCovered("A2", map, locations3));
    EXPECT(isCovered("A3", map, locations3));
    EXPECT(isCovered("B1", map, locations3));
    EXPECT(isCovered("B2", map, locations3));
    EXPECT(isCovered("B3", map, locations3));
    EXPECT(isCovered("C1", map, locations3));
    EXPECT(isCovered("C2", map, locations3));
    EXPECT(isCovered("C3", map, locations3));
}

ADD_TEST("Provided Test: Can solve ethane example with two cities.") {
    HashMap<string, HashSet<string>> map = makeSymmetric({
        { "C1", {"H1", "H3", "H5", "C2"} },
        { "C2", {"H2", "H4", "H6"} },
    });

    HashSet<string> locations;
    EXPECT(canBeMadeDisasterReady(map, 2, locations));
}

ADD_TEST("Provided Test: Can solve ethane example with two cities, and produces output.") {
    HashMap<string, HashSet<string>> map = makeSymmetric({
        { "C1", {"H1", "H3", "H5", "C2"} },
        { "C2", {"H2", "H4", "H6"} },
    });

    HashSet<string> locations;
    EXPECT(canBeMadeDisasterReady(map, 2, locations));
    EXPECT(locations.size() == 2);
    EXPECT(locations.contains("C1"));
    EXPECT(locations.contains("C2"));
}

ADD_TEST("Provided Test: Solves \"Don't be Greedy\" from the handout.") {
    HashMap<string, HashSet<string>> map = makeSymmetric({
        { "A", { "B" } },
        { "B", { "C", "D" } },
        { "C", { "D" } },
        { "D", { "E", "F" } },
        { "E", { "F" } },
        { "F", { "G" } },
    });

    HashSet<string> locations0, locations1, locations2;
    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
    EXPECT(!canBeMadeDisasterReady(map, 1, locations1));
    EXPECT(canBeMadeDisasterReady(map, 2, locations2));
}

ADD_TEST("Provided Test: Solves \"Don't be Greedy\" from the handout, and produces output.") {
    HashMap<string, HashSet<string>> map = makeSymmetric({
        { "A", { "B" } },
        { "B", { "C", "D" } },
        { "C", { "D" } },
        { "D", { "G", "F" } },
        { "E", { "F" } },
        { "F", { "G" } },
    });

    HashSet<string> locations0, locations1, locations2;
    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
    EXPECT(!canBeMadeDisasterReady(map, 1, locations1));
    EXPECT(canBeMadeDisasterReady(map, 2, locations2));

    HashSet<string> expected = {"B", "F"};
    EXPECT_EQUAL(locations2, expected);
}

ADD_TEST("Provided Test: Stress test: 6 x 6 grid. (This should take at most a few seconds.)") {
    HashMap<string, HashSet<string>> grid;

    /* Build the grid. */
    char maxRow = 'F';
    int  maxCol = 6;
    for (char row = 'A'; row <= maxRow; row++) {
        for (int col = 1; col <= maxCol; col++) {
            if (row != maxRow) {
                grid[row + to_string(col)] += (char(row + 1) + to_string(col));
            }
            if (col != maxCol) {
                grid[row + to_string(col)] += (char(row) + to_string(col + 1));
            }
        }
    }
    grid = makeSymmetric(grid);

    HashSet<string> locations;
    EXPECT(canBeMadeDisasterReady(grid, 10, locations));
}

ADD_TEST("Provided Test: Stress test output. (This should take at most a few seconds.)") {
    HashMap<string, HashSet<string>> grid;

    /* Build the grid. */
    char maxRow = 'F';
    int  maxCol = 6;
    for (char row = 'A'; row <= maxRow; row++) {
        for (int col = 1; col <= maxCol; col++) {
            if (row != maxRow) {
                grid[row + to_string(col)] += (char(row + 1) + to_string(col));
            }
            if (col != maxCol) {
                grid[row + to_string(col)] += (char(row) + to_string(col + 1));
            }
        }
    }
    grid = makeSymmetric(grid);

    HashSet<string> locations;
    EXPECT(canBeMadeDisasterReady(grid, 10, locations));

    for (char row = 'A'; row <= maxRow; row++) {
        for (int col = 1; col <= maxCol; col++) {
            EXPECT(isCovered(row + to_string(col), grid, locations));
        }
    }
}

