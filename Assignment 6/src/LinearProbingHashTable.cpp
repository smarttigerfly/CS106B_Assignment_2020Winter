#include "LinearProbingHashTable.h"
#include "Testing/LinearProbingTests.h"
using namespace std;

LinearProbingHashTable::LinearProbingHashTable(HashFunction<std::string> hashFn) {
    /* TODO: Delete this comment and the next line, then implement this function. */
    hashFunString = hashFn;
    allocatedSize = hashFunString.numSlots();
    logicalSize = 0;
    elems = new Slot[allocatedSize];
    for(int i = 0; i < allocatedSize; i++)
    {
        elems[i].isEmpty = true;
        elems[i].isTomb = false;
    }
}

LinearProbingHashTable::~LinearProbingHashTable() {
    /* TODO: Delete this comment, then implement this function. */
    delete [] elems;
}

int LinearProbingHashTable::size() const {
    /* TODO: Delete this comment and the next lines, then implement this function. */
    return logicalSize;
}

bool LinearProbingHashTable::isEmpty() const {
    /* TODO: Delete this comment and the next lines, then implement this function. */
    return logicalSize == 0;
}

bool LinearProbingHashTable::insert(const string& elem) {
    /* TODO: Delete this comment and the next lines, then implement this function. */
    if(contains(elem))
        return false;
    int hasInt = hashFunString(elem);
    int insertNum = 0;
    for(; insertNum < allocatedSize; )
    {
        int insertIndex = hasInt;
        if(hasInt >= allocatedSize)
        {
            insertIndex = hasInt - allocatedSize;
        }
        if(elems[insertIndex].isEmpty ||
           elems[insertIndex].isTomb)
        {
            elems[insertIndex].value = elem;
            elems[insertIndex].isEmpty = false;
            elems[insertIndex].isTomb = false;
            logicalSize++;
            return true;
        }
        hasInt++;
        insertNum++;
    }

    return false;
}

bool LinearProbingHashTable::contains(const string& elem) const {
    /* TODO: Delete this comment and the next lines, then implement this function. */
    if(indexofKey(elem) < 0)
        return false;
    else
        return true;
}

int LinearProbingHashTable::indexofKey(const std::string& elem) const
{
    int index = hashFunString(elem);
    int searchNum = 0;
    for (; searchNum < logicalSize; )
    {
        int elemIndex = index;
        if(index >= allocatedSize)
            elemIndex = index - allocatedSize;
        index++;
        if(elems[elemIndex].isTomb)
            continue;
        if(elems[elemIndex].isEmpty)
            break;
        //Slot has a element;
        searchNum++;
        if(elems[elemIndex].value == elem)
            return elemIndex;
    }
    return -1;
}

bool LinearProbingHashTable::remove(const string& elem) {
    /* TODO: Delete this comment and the next lines, then implement this function. */

    int elemIndex = indexofKey(elem);
    if(elemIndex < 0)
        return false;
    else
    {
        elems[elemIndex].value = "";
        elems[elemIndex].isEmpty = true;
        elems[elemIndex].isTomb = true;
        logicalSize--;
        return true;
    }

//    int index = hashFunString(elem);
//    int searchNum = 0;

//    for (; searchNum < logicalSize; )
//    {
//        int elemIndex = index;
//        if(index >= allocatedSize)
//            elemIndex = index - allocatedSize;
//        index++;
//        if(elems[elemIndex].isTomb)
//            continue;
//        if(elems[elemIndex].isEmpty)
//            break;
//        //Slot has a element;
//        searchNum++;
//        if(elems[elemIndex].value == elem)
//        {
//            elems[elemIndex].value = "";
//            elems[elemIndex].isEmpty = true;
//            elems[elemIndex].isTomb = true;
//            logicalSize--;
//            return true;
//        }

//    }
//    return false;

}

void LinearProbingHashTable::printDebugInfo() const {
    /* TODO: Remove this comment and implement this function. */
    for(int i = 0; i < size(); i++)
    {
        cout << "elems[" << i << "]:" << elems[i].value << endl;
    }
}


/* * * * * * Test Cases Below This Point * * * * * */

/* Optional: Add your own custom tests here! */














/* * * * * Provided Tests Below This Point * * * * */
#include "vector.h"

ADD_TEST("Provided Test: Table is initially empty.") {
    LinearProbingHashTable table(Hash::random(10));

    EXPECT_EQUAL(table.size(), 0);
    EXPECT(table.isEmpty());
}

ADD_TEST("Provided Test: Stores the hash function given in the constructor.") {
    /* This is an evil hash function that throws errors on certain inputs. If you
     * don't store the hash function we give you, this test will fail.
     */
    LinearProbingHashTable table(Hash::radioactive(10));

    EXPECT_ERROR(table.contains("137"));
    EXPECT_ERROR(table.contains(""));
    EXPECT(!table.contains("lead shielding"));
}

ADD_TEST("Provided Test: Can insert and look up a single value.") {
    LinearProbingHashTable table(Hash::random(10));

    EXPECT(!table.contains("0"));
    EXPECT(table.insert("0"));
    EXPECT(table.contains("0"));
}

ADD_TEST("Provided Test: Is case-sensitive.") {
    LinearProbingHashTable table(Hash::random(10));

    EXPECT(!table.contains("a"));
    EXPECT(!table.contains("A"));
    EXPECT(table.insert("a"));
    EXPECT(table.contains("a"));
    EXPECT(!table.contains("A"));
}

ADD_TEST("Provided Test: Insertions work with hash collisions.") {
    /* Use a very, very bad hash function that maps everything to slot zero. */
    LinearProbingHashTable table(Hash::zero(10));

    Vector<string> toAdd = {
        "Quokka", "Pudu", "Gerenuk", "Dikdik"
    };
    Vector<string> toNotAdd = {
        "Springbok", "Kudu"
    };

    for (string animal: toAdd) {
        EXPECT(table.insert(animal));
    }
    EXPECT_EQUAL(table.size(), toAdd.size());

    for (string animal: toAdd) {
        EXPECT(table.contains(animal));
    }
    for (string animal: toNotAdd) {
        EXPECT(!table.contains(animal));
    }
}

ADD_TEST("Provided Test: Insertions/lookups succeed when ranges overlap.") {
    /* The hash function we use maps strings to their numeric values. This allows
     * us to control the contents of the hash table.
     */
    LinearProbingHashTable table(Hash::identity(10));

    /* Place 0 and 10 in. The table should now look like this:
     *
     *            0 10 . . . . . . . .
     *
     */
    EXPECT(table.insert("0"));
    EXPECT(table.insert("10"));
    EXPECT(table.contains("0"));
    EXPECT(table.contains("10"));

    /* Now, insert the values 1, 2, 3, 4, and 5. The table should look
     * like this:
     *
     *           0 10  1  2  3  4  5  .  .  .
     */
    EXPECT(table.insert("1"));
    EXPECT(table.insert("2"));
    EXPECT(table.insert("3"));
    EXPECT(table.insert("4"));
    EXPECT(table.insert("5"));

    /* Make sure we can find everything. */
    EXPECT(table.contains("0"));
    EXPECT(table.contains("1"));
    EXPECT(table.contains("2"));
    EXPECT(table.contains("3"));
    EXPECT(table.contains("4"));
    EXPECT(table.contains("5"));
    EXPECT(!table.contains("6"));
    EXPECT(!table.contains("7"));
    EXPECT(!table.contains("8"));
    EXPECT(!table.contains("9"));
    EXPECT(table.contains("10"));

    EXPECT_EQUAL(table.size(), 7);
}

ADD_TEST("Provided Test: Wraps around the end of the table.") {
    /* Everything goes in slot 7. This is a terrible hash function that's just used for
     * testing purposes.
     */
    LinearProbingHashTable table(Hash::constant(10, 7));

    /* Insert a bunch of values. */
    Vector<string> toAdd = {
        "H", "He", "Li", "Be", "B", "C", "N"
    };
    Vector<string> toNotAdd = {
        "O", "F", "Ne"
    };

    /* Add the elements in. */
    for (string elem: toAdd) {
        EXPECT(table.insert(elem));
    }

    /* Confirm they're all there. */
    for (string elem: toAdd) {
        EXPECT(table.contains(elem));
    }

    /* Confirm others aren't. */
    for (string elem: toNotAdd) {
        EXPECT(!table.contains(elem));
    }
}

ADD_TEST("Provided Test: Doesn't allow for duplicates.") {
    /* Drop everything into slot zero, just for consistency. */
    LinearProbingHashTable table(Hash::zero(10));

    EXPECT(table.insert("Dikdik"));
    EXPECT_EQUAL(table.size(), 1);

    /* Insert the same value more times than the table can hold. */
    for (int i = 0; i < 100; i++) {
        EXPECT(!table.insert("Dikdik"));
        EXPECT_EQUAL(table.size(), 1);
    }
}

ADD_TEST("Provided Test: Handles inserting the empty string.") {
    LinearProbingHashTable table(Hash::random(10));

    EXPECT(!table.contains(""));
    EXPECT(table.insert(""));
    EXPECT(table.contains(""));
    EXPECT_EQUAL(table.size(), 1);
}

ADD_TEST("Provided Test: Lookups work even if the table is full.") {
    /* Dump everything in bucket 7. This is a terrible hash function, but it's
     * useful for testing.
     */
    LinearProbingHashTable table(Hash::constant(10, 7));

    /* Fill the table. */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.insert(to_string(i)));
    }
    EXPECT_EQUAL(table.size(), 10);

    /* Search for all present items. */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.contains(to_string(i)));
    }

    /* Confirm elements that aren't there don't show up. Watch out! This edge
     * case might cause your code to hang if you haven't anticipated it.
     */
    for (int i = 10; i < 20; i++) {
        EXPECT(!table.contains(to_string(i)));
    }
}

ADD_TEST("Provided Test: Won't insert elements if table is full.") {
    /* Terrible hash function that places everything in slot zero. */
    LinearProbingHashTable table(Hash::zero(10));

    /* Load the table. */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.insert(to_string(i))); // Should succeed
    }
    EXPECT_EQUAL(table.size(), 10);

    /* All these operations should fail. */
    for (int i = 10; i < 20; i++) {
        EXPECT(!table.insert(to_string(i)));
    }
}

ADD_TEST("Provided Test: Stress Test: Handles pure insertion of elements.") {
    const int kNumTrials = 50; // Do this lots of times to smoke out any errors that might be lurking.
    for (int trial = 0; trial < kNumTrials; trial++) {
        LinearProbingHashTable table(Hash::random(100));

        const int kNumElems = 75;
        for (int i = 0; i < kNumElems; i++) {
            /* Confirm only the proper elements exist at this point. */
            for (int j = 0; j < kNumElems; j++) {
                EXPECT_EQUAL(table.contains(to_string(j)), bool(j < i));
            }

            /* Add the element. */
            EXPECT(table.insert(to_string(i))); // Should succeed
            EXPECT_EQUAL(table.size(), i + 1);
        }
    }
}

ADD_TEST("Provided Test: Stress Test: Inserts/searches work in expected time O(1).") {
    /* Huge number of slots. */
    const int kNumSlots = 1000000;

    /* Create an enormous hash table with a random hash function. */
    LinearProbingHashTable table(Hash::random(kNumSlots));

    /* Search the table for lots of elements. This should quick, since
     * the table is empty.
     */
    for (int i = 0; i < kNumSlots; i++) {
        EXPECT(!table.contains(to_string(i)));
    }

    /* Insert a lot elements. */
    const int kLotsOfElems = 100000; // 10% load factor - quite small!
    for (int i = 0; i < kLotsOfElems; i++) {
        EXPECT(table.insert(to_string(i)));
    }
    for (int i = 0; i < kLotsOfElems; i++) {
        EXPECT(table.contains(to_string(i)));
    }

    /* Confirm other elements aren't there. These false lookups should still be fast
     * due to the low load factor.
     */
    for (int i = kLotsOfElems; i < 2 * kLotsOfElems; i++) {
        EXPECT(!table.contains(to_string(i)));
    }
}

ADD_TEST("Provided Test: Can insert and remove a single element.") {
    /* Bad hash function mapping each string to the number it represents. */
    LinearProbingHashTable table(Hash::identity(200));

    /* Insert an element. */
    EXPECT(table.insert("137"));
    EXPECT_EQUAL(table.size(), 1);
    EXPECT(!table.isEmpty());

    /* Remove the element. */
    EXPECT(table.remove("137"));
    EXPECT_EQUAL(table.size(), 0);
    EXPECT(table.isEmpty());
    EXPECT(!table.contains("137"));
}

ADD_TEST("Provided Test: Handles a single tombstone.") {
    /* Hash function mapping each item to its numeric position, which makes the test
     * predictably control where elements go.
     */
    LinearProbingHashTable table(Hash::identity(10));

    /* Inserting these values makes the table look like this:
     *
     *  .  1  2  3 13  5  .  .  .  .
     */
    EXPECT(table.insert("1"));
    EXPECT(table.insert("2"));
    EXPECT(table.insert("3"));
    EXPECT(table.insert("13"));
    EXPECT(table.insert("5"));
    EXPECT_EQUAL(table.size(), 5);

    /* Delete 3 from the table. We should now have this, where T
     * is a tombstone.
     *
     *    .  1  2  T 13  5  .  .  .  .
     */
    EXPECT(table.remove("3"));
    EXPECT_EQUAL(table.size(), 4);

    /* Confirm everything else is still there. Note that if 13
     * is missing, there's a chance we aren't scanning over the
     * tombstone slot, and if 5 is missing it might mean that we
     * tried pulling previous elements backwards, which isn't
     * safe.
     */
    EXPECT(table.contains("1"));
    EXPECT(table.contains("2"));
    EXPECT(!table.contains("3"));
    EXPECT(table.contains("13"));
    EXPECT(table.contains("5"));
}

ADD_TEST("Provided Test: Handles lookups with long chains of tombstones") {
    /* Hash function mapping each item to its numeric position, which makes the test
     * predictably control where elements go.
     */
    LinearProbingHashTable table(Hash::identity(20));

    /* Form this pattern:
     *
     * T 1 . . 4 5 T T T T
     */
    EXPECT(table.insert("4"));
    EXPECT(table.insert("5"));
    EXPECT(table.insert("6"));
    EXPECT(table.insert("7"));
    EXPECT(table.insert("8"));
    EXPECT(table.insert("9"));
    EXPECT(table.insert("0"));
    EXPECT(table.insert("1"));
    EXPECT(table.remove("6"));
    EXPECT(table.remove("7"));
    EXPECT(table.remove("8"));
    EXPECT(table.remove("9"));
    EXPECT(table.remove("0"));

    /* Confirm that 1, 4, and 5 are there. */
    EXPECT(table.contains("4"));
    EXPECT(table.contains("5"));
    EXPECT(table.contains("1"));

    /* Ensure nothing else is. */
    EXPECT(!table.contains("6"));
    EXPECT(!table.contains("7"));
    EXPECT(!table.contains("8"));
    EXPECT(!table.contains("9"));
    EXPECT(!table.contains("0"));

    EXPECT_EQUAL(table.size(), 3);
}

ADD_TEST("Provided Test: Recycles space from tombstones.") {
    LinearProbingHashTable table(Hash::random(10));

    /* Add ten values. */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.insert(to_string(i)));
    }
    EXPECT_EQUAL(table.size(), 10);

    /* Remove all of them. */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.remove(to_string(i)));
    }

    /* The table should now be empty, though each slot is now a tombstone. */
    EXPECT_EQUAL(table.size(), 0);
    EXPECT(table.isEmpty());

    /* Add the elements a second time. If space for tombstones isn't recycled,
     * these operations will fail.
     */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.insert(to_string(i)));
        EXPECT_EQUAL(table.size(), i + 1);
    }

    /* These operations should fail, because the table is now full. */
    for (int i = 10; i < 20; i++) {
        EXPECT(!table.insert(to_string(i)));
        EXPECT_EQUAL(table.size(), 10);
    }
}

ADD_TEST("Provided Test: Insertions over tombstones don't add duplicates.") {
    /* Hash function dropping everything into slot 4. This is a terrible hash
     * function that no one would ever use, but for testing it's super
     * convenient!
     */
    LinearProbingHashTable table(Hash::constant(10, 4));

    /* Form this pattern:
     *
     * T 1 . . 4 5 T T T T
     */
    EXPECT(table.insert("4"));
    EXPECT(table.insert("5"));
    EXPECT(table.insert("6"));
    EXPECT(table.insert("7"));
    EXPECT(table.insert("8"));
    EXPECT(table.insert("9"));
    EXPECT(table.insert("0"));
    EXPECT(table.insert("1"));
    EXPECT(table.remove("6"));
    EXPECT(table.remove("7"));
    EXPECT(table.remove("8"));
    EXPECT(table.remove("9"));
    EXPECT(table.remove("0"));

    /* Insert 1 into the table. The element 1 is already present, but to find it
     * you have to skip over tombstones, even though insertions like to fill in
     * tombstones.
     */
    EXPECT(!table.insert("1"));
    EXPECT_EQUAL(table.size(), 3);

    /* Ensure that we can't remove two copies of 1. */
    EXPECT(table.remove("1"));
    EXPECT_EQUAL(table.size(), 2);

    EXPECT(!table.remove("1"));
    EXPECT_EQUAL(table.size(), 2);

    /* Confirm that 4 and 5 are still there. */
    EXPECT(table.contains("4"));
    EXPECT(table.contains("5"));

    /* Ensure nothing else is. */
    EXPECT(!table.contains("6"));
    EXPECT(!table.contains("7"));
    EXPECT(!table.contains("8"));
    EXPECT(!table.contains("9"));
    EXPECT(!table.contains("0"));
}

ADD_TEST("Provided Test: Handles removing the empty string.") {
    LinearProbingHashTable table(Hash::random(10));

    EXPECT(!table.remove(""));

    EXPECT(!table.contains(""));
    EXPECT(table.insert(""));
    EXPECT(table.contains(""));
    EXPECT_EQUAL(table.size(), 1);

    EXPECT(table.remove(""));
    EXPECT_EQUAL(table.size(), 0);
    EXPECT(!table.contains(""));

    EXPECT(!table.remove(""));
}

ADD_TEST("Provided Test: Can remove from a full table.") {
    /* Drop everything in bucket 7, which is a terrible choice of hash function but
     * which makes testing a lot easier.
     */
    LinearProbingHashTable table(Hash::constant(10, 7));

    /* Fill the table. */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.insert(to_string(i)));
    }
    EXPECT_EQUAL(table.size(), 10);

    /* Try removing some elements that aren't present. This may hang if your implementation
     * of remove wasn't anticipating this case.
     */
    for (int i = 10; i < 20; i++) {
        EXPECT(!table.remove(to_string(i)));
    }

    /* Now, do the actual removals. */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.remove(to_string(i)));
    }
    EXPECT(table.isEmpty());
    EXPECT_EQUAL(table.size(), 0);
}

ADD_TEST("Provided Test: Stress Test: Handles large numbers of removals.") {
    const int kNumTrials = 50; // Do this lots of times to smoke out any errors that might be lurking.
    for (int trial = 0; trial < kNumTrials; trial++) {
        LinearProbingHashTable table(Hash::random(100));

        const int kNumElems = 75;
        for (int i = 0; i < kNumElems; i++) {
            table.insert(to_string(i));
        }

        EXPECT_EQUAL(table.size(), kNumElems);

        for (int i = 0; i < kNumElems; i++) {
            /* Confirm only the proper elements exist at this point. */
            for (int j = 0; j < kNumElems; j++) {
                EXPECT_EQUAL(table.contains(to_string(j)), bool(j >= i));
            }

            EXPECT(table.remove(to_string(i)));
            EXPECT_EQUAL(table.size(), kNumElems - i - 1);
        }
    }
}

ADD_TEST("Provided Test: Stress Test: Inserts/searches/deletes work in expected time O(1).") {
    /* Huge number of slots. */
    const int kNumSlots = 1000000;

    /* Create an enormous hash table with a random hash function. */
    LinearProbingHashTable table(Hash::random(kNumSlots));

    /* Insert a lot elements. */
    const int kLotsOfElems = 100000; // 10% load factor - quite small!
    for (int i = 0; i < kLotsOfElems; i++) {
        EXPECT(table.insert(to_string(i)));
    }

    /* Remove the middle half of them. */
    for (int i = kLotsOfElems / 4; i < 3 * kLotsOfElems / 4; i++) {
        EXPECT(table.remove(to_string(i)));
    }

    /* Search for lots of elements and confirm the ones are supposed to be there
     * are indeed there.
     */
    for (int i = 0; i < kLotsOfElems; i++) {
        EXPECT_EQUAL(table.contains(to_string(i)), bool(i < kLotsOfElems / 4 || i >= 3 * kLotsOfElems / 4));
    }
}
