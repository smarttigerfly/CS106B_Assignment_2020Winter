#include "RobinHoodHashTable.h"
#include "Testing/RobinHoodTests.h"
using namespace std;

RobinHoodHashTable::RobinHoodHashTable(HashFunction<string> hashFn) {
    /* TODO: Delete this comment, then implement this function. */
    hashFunString = hashFn;
    allocatedSize = hashFunString.numSlots();
    logicalSize = 0;
    elems = new Slot[allocatedSize];
    for(int i = 0; i < allocatedSize; i++)
    {
        elems[i].isEmpty = true;
    }
}

RobinHoodHashTable::~RobinHoodHashTable() {
    /* TODO: Delete this comment, then implement this function. */
    delete [] elems;
}

int RobinHoodHashTable::size() const {
    /* TODO: Delete this comment and the next line, then implement this function. */
    return logicalSize;
}

bool RobinHoodHashTable::isEmpty() const {
    /* TODO: Delete this comment and the next line, then implement this function. */
    return logicalSize == 0;
}

bool RobinHoodHashTable::insert(const string& elem) {
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

        if(elems[insertIndex].isEmpty)
        {
            elems[insertIndex].value = elem;
            elems[insertIndex].isEmpty = false;
            elems[insertIndex].farFromHome = insertNum;
            logicalSize++;
            return true;
        } else
        {
            if(insertNum >  elems[insertIndex].farFromHome)
            {
                if(moveElement(insertIndex, 0))
                {
                    elems[insertIndex].value = elem;
                    elems[insertIndex].isEmpty = false;
                    elems[insertIndex].farFromHome = insertNum;
                    logicalSize++;
                    return true;
                } else {
                    return false;
                }
            } else {
                hasInt++;
                insertNum++;
            }
        }

    }

    return false;
}

bool RobinHoodHashTable::contains(const string& elem) const {
    /* TODO: Delete this comment and the next lines, then implement this function. */
    if(indexofKey(elem) < 0)
        return false;
    else
        return true;
}

bool RobinHoodHashTable::remove(const string& elem) {
    /* TODO: Delete this comment and the next lines, then implement this function. */
    int elemIndex = indexofKey(elem);
    if(elemIndex < 0)
        return false;
    else
    {
        elems[elemIndex].value = "";
        elems[elemIndex].isEmpty = true;
        elems[elemIndex].farFromHome = 0;
        logicalSize--;
        backwardShiftDele(elemIndex);
        return true;
    }

}

void RobinHoodHashTable::printDebugInfo() const {
    /* TODO: Remove this comment and implement this function. */
}

/* * * * * * Test Cases Below This Point * * * * * */

/* Optional: Add your own custom tests here! */

int RobinHoodHashTable::indexofKey(const std::string& elem) const
{
    int index = hashFunString(elem);
    int searchNum = 0;
    for (; searchNum < logicalSize; )
    {
        int elemIndex = index;
        if(index >= allocatedSize)
            elemIndex = index - allocatedSize;
        index++;
        if(elems[elemIndex].isEmpty)
            break;  //跳出循环，返回-1，不包含此元素
        else
        {
            if(elems[elemIndex].value == elem)
                return elemIndex;
            if(searchNum > elems[elemIndex].farFromHome)
                break;  //跳出循环，返回-1，不包含此元素
            searchNum++;
        }

    }
    return -1;
}


bool RobinHoodHashTable::moveElement(int index, int moveNum)
{
    if(moveNum >= allocatedSize)
        return false;
    moveNum++;
    if(elems[index].isEmpty)
        return true;
    else
    {
        int nextIndex = index + 1;
        if(nextIndex >= allocatedSize )
            nextIndex -= allocatedSize;
        if(moveElement(nextIndex, moveNum))
        {
            elems[nextIndex].value = elems[index].value;
            elems[nextIndex].isEmpty = false;
            elems[nextIndex].farFromHome = elems[index].farFromHome + 1;
            return true;
        } else
        {
            return false;
        }
    }
}

void RobinHoodHashTable::backwardShiftDele(int index)
{
    int nextIndex = index + 1;
    if(nextIndex >= allocatedSize )
        nextIndex -= allocatedSize;
    if(elems[nextIndex].isEmpty ||
       elems[nextIndex].farFromHome == 0)
        return;
    else
    {
        elems[index].value = elems[nextIndex].value;
        elems[nextIndex].value = "";
        elems[index].isEmpty = false;
        elems[nextIndex].isEmpty = true;
        elems[index].farFromHome = elems[nextIndex].farFromHome - 1;
        elems[nextIndex].farFromHome = 0;
        backwardShiftDele(nextIndex);
    }
}








/* * * * * Provided Tests Below This Point * * * * */
#include "Demos/Utility.h"
#include "vector.h"

ADD_TEST("Provided Test: Table is initially empty.") {
    RobinHoodHashTable table(Hash::random(10));

    EXPECT_EQUAL(table.size(), 0);
    EXPECT(table.isEmpty());
}

ADD_TEST("Provided Test: Stores the hash function given in the constructor.") {
    /* This is an evil hash function that throws errors on certain inputs. If you
     * don't store the hash function we give you, this test will fail.
     */
    RobinHoodHashTable table(Hash::radioactive(10));

    EXPECT_ERROR(table.contains("137"));
    EXPECT_ERROR(table.contains(""));
    EXPECT(!table.contains("lead shielding"));
}

ADD_TEST("Provided Test: Can insert and look up a single value.") {
    RobinHoodHashTable table(Hash::random(10));

    EXPECT(!table.contains("0"));
    EXPECT(table.insert("0"));
    EXPECT(table.contains("0"));
}

ADD_TEST("Provided Test: Is case-sensitive.") {
    RobinHoodHashTable table(Hash::random(10));

    EXPECT(!table.contains("a"));
    EXPECT(!table.contains("A"));
    EXPECT(table.insert("a"));
    EXPECT(table.contains("a"));
    EXPECT(!table.contains("A"));
}

ADD_TEST("Provided Test: Insertions work with hash collisions.") {
    /* Use a very, very bad hash function that maps everything to slot zero. */
    RobinHoodHashTable table(Hash::zero(10));

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

ADD_TEST("Provided Test: Insertions/lookups succeed when ranges overlap (v1).") {
    /* The hash function we use maps strings to their numeric values. This allows
     * us to control the contents of the hash table.
     */
    RobinHoodHashTable table(Hash::identity(10));

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

ADD_TEST("Provided Test: Insertions/lookups succeed when ranges overlap (v2).") {
    /* The hash function we use maps strings to their numeric values. This allows
     * us to control the contents of the hash table.
     */
    RobinHoodHashTable table(Hash::identity(10));

    /* Insert the values 1, 2, 3, 4, and 5. The table should look
     * like this:
     *
     *           .  1  2  3  4  5  .  .  .  .
     */
    EXPECT(table.insert("1"));
    EXPECT(table.insert("2"));
    EXPECT(table.insert("3"));
    EXPECT(table.insert("4"));
    EXPECT(table.insert("5"));

    /* Add in 0 and 10. The table should now look like this:
     *
     *    0  10  1  2  3  4  5  .  .
     *
     * Notice that elements needed to be shifted over.
     */
    EXPECT(table.insert("0"));
    EXPECT(table.insert("10"));

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
    RobinHoodHashTable table(Hash::constant(10, 7));

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
    RobinHoodHashTable table(Hash::zero(10));

    EXPECT(table.insert("Dikdik"));
    EXPECT_EQUAL(table.size(), 1);

    /* Insert the same value more times than the table can hold. */
    for (int i = 0; i < 100; i++) {
        EXPECT(!table.insert("Dikdik"));
        EXPECT_EQUAL(table.size(), 1);
    }
}

ADD_TEST("Provided Test: Handles inserting the empty string.") {
    RobinHoodHashTable table(Hash::random(10));

    EXPECT(!table.contains(""));
    EXPECT(table.insert(""));
    EXPECT(table.contains(""));
    EXPECT_EQUAL(table.size(), 1);
}

ADD_TEST("Provided Test: Lookups work even if the table is full.") {
    /* Dump everything in bucket 7. This is a terrible hash function, but it's
     * useful for testing.
     */
    RobinHoodHashTable table(Hash::constant(10, 7));

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
    RobinHoodHashTable table(Hash::zero(10));

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

ADD_TEST("Provided Test: Stress Test: Searches cut off early.") {
    /* We're going to fill an enormous table with a bunch of blocks of elements, then try doing
     * lookups. Using regular linear probing, this will grind to a halt because the table is
     * full. Using Robin Hood hashing, this should cut off extremely quickly.
     *
     * The table will generally look like this:
     *
     * |--- block with hash code 0 ---| |-- block with hash code 1 ---| |-- block with hash code 2 --| ...
     *
     * If we ever do a lookup for an element, it should never run off the end of a block, because
     * as soon as we hit the start of the next block the element we're scanning for will be
     * further from home than the element at the block start.
     */
    const int kElemsPerBlock = 5;
    const int kNumBlocks     = 100000;
    const int kNumElems      = kNumBlocks * kElemsPerBlock;

    /* Each element's position is given by its value, mod the table size. */
    RobinHoodHashTable table(Hash::identity(kNumElems));

    /* Fill the table with many blocks of values with the same hash code. */
    for (int block = 0; block < kNumBlocks; block++) {
        for (int elem = 0; elem < kElemsPerBlock; elem++) {
            /* These values should all drop into a position given by the block index
             * times the number of blocks per element.
             */
            int value = block * kElemsPerBlock + elem * kNumElems;
            EXPECT(table.insert(to_string(value)));
        }
    }
    EXPECT_EQUAL(table.size(), kNumElems);

    /* Confirm each element is present. This should be very quick. */
    for (int block = 0; block < kNumBlocks; block++) {
        for (int elem = 0; elem < kElemsPerBlock; elem++) {
            /* These values should all drop into a position given by the block index
             * times the number of blocks per element.
             */
            int value = block * kElemsPerBlock + elem * kNumElems;
            EXPECT(table.contains(to_string(value)));
        }
    }

    /* Now, try looking up a bunch of values that aren't there. This should, again, be extremely
     * quick because we never need to scan past the end of a block.
     */
    for (int block = 0; block < kNumBlocks; block++) {
        for (int elem = kElemsPerBlock; elem < 2 * kElemsPerBlock; elem++) {
            /* These values should all drop into a position given by the block index
             * times the number of blocks per element.
             */
            int value = block * kElemsPerBlock + elem * kNumElems;
            EXPECT(!table.contains(to_string(value)));
        }
    }
}

ADD_TEST("Provided Test: Stress Test: Handles pure insertion of elements.") {
    const int kNumTrials = 50; // Do this lots of times to smoke out any errors that might be lurking.
    for (int trial = 0; trial < kNumTrials; trial++) {
        RobinHoodHashTable table(Hash::random(100));

        const int kNumElems = 75;
        for (int i = 0; i < kNumElems; i++) {
            /* Confirm only the proper elements exist at this point. */
            for (int j = 0; j < kNumElems; j++) {
                EXPECT_EQUAL(table.contains(to_string(j)), j < i);
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
    RobinHoodHashTable table(Hash::random(kNumSlots));

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
    RobinHoodHashTable table(Hash::identity(200));

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

ADD_TEST("Provided Test: Handles removing the empty string.") {
    RobinHoodHashTable table(Hash::random(10));

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

ADD_TEST("Provided Test: Doesn't backward-shift elements from their homes.") {
    /* Each element hashes to its own numeric value, modded by the table size. */
    RobinHoodHashTable table(Hash::identity(10));

    /* Insert two adjacent runs of values to get this sequence:
     *
     * .  1 11 21 31  5 15 25 35 .
     */
    EXPECT(table.insert("1"));
    EXPECT(table.insert("11"));
    EXPECT(table.insert("21"));
    EXPECT(table.insert("31"));
    EXPECT(table.insert("5"));
    EXPECT(table.insert("15"));
    EXPECT(table.insert("25"));
    EXPECT(table.insert("35"));
    EXPECT_EQUAL(table.size(), 8);

    /* Remove 11. This should backward-shift delete to give
     *
     * .  1 21 31  .  5 15 25 35 .
     *
     * which means that 5, 15, 25, and 35 should all still be found.
     */
    EXPECT(table.remove("11"));
    EXPECT(table.contains("1"));
    EXPECT(!table.contains("11"));
    EXPECT(table.contains("21"));
    EXPECT(table.contains("31"));
    EXPECT(table.contains("5"));
    EXPECT(table.contains("15"));
    EXPECT(table.contains("25"));
    EXPECT(table.contains("35"));
}

ADD_TEST("Provided Test: Deletes around the end of the table.") {
    /* Everything goes in slot 8. */
    RobinHoodHashTable table(Hash::constant(10, 8));

    /* Insert a bunch of values. */
    for (int i = 0; i < 5; i++) {
        EXPECT(table.insert(to_string(i)));
    }
    EXPECT_EQUAL(table.size(), 5);

    /* Delete three of the values. */
    for (int i = 0; i < 3; i++) {
        EXPECT(table.remove(to_string(i)));
    }
    EXPECT_EQUAL(table.size(), 2);

    /* Confirm everything else is still there. */
    for (int i = 0; i < 5; i++) {
        EXPECT_EQUAL(table.contains(to_string(i)), i >= 3);
    }
}

ADD_TEST("Provided Test: Can remove from a full table.") {
    /* Drop everything in bucket 7, which is a terrible choice of hash function but
     * which makes testing a lot easier.
     */
    RobinHoodHashTable table(Hash::constant(10, 7));

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
        RobinHoodHashTable table(Hash::random(100));

        const int kNumElems = 75;
        for (int i = 0; i < kNumElems; i++) {
            table.insert(to_string(i));
        }

        EXPECT_EQUAL(table.size(), kNumElems);

        for (int i = 0; i < kNumElems; i++) {
            /* Confirm only the proper elements exist at this point. */
            for (int j = 0; j < kNumElems; j++) {
                EXPECT_EQUAL(table.contains(to_string(j)), j >= i);
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
    RobinHoodHashTable table(Hash::random(kNumSlots));

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

