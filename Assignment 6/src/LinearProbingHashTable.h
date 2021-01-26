#ifndef LinearProbingHashTable_Included
#define LinearProbingHashTable_Included

#include "HashFunction.h"
#include "Demos/Utility.h"
#include <string>

class LinearProbingHashTable {
public:
    /**
     * Constructs a new linear probing table that uses the hash function given
     * as the argument. (Note that the hash function lets you determine how
     * many slots to use; you can get this by calling hashFn.numSlots().)
     */
    LinearProbingHashTable(HashFunction<std::string> hashFn);

    /**
     * Cleans up all memory allocated by this hash table.
     */
    ~LinearProbingHashTable();

    /**
     * Returns whether the table is empty.
     */
    bool isEmpty() const;

    /**
     * Returns the number of elements in the table.
     */
    int size() const;

    /**
     * Inserts the specified element into this hash table. If the element already
     * exists, this leaves the table unchanged. If there is no space in the table
     * to insert an element - that is, every slot is full - this should return
     * false to indicate that there is no more space.
     *
     * This function returns whether the element was inserted into the table.
     */
    bool insert(const std::string& key);

    /**
     * Returns whether the specified key is contained in this hash tasble.
     */
    bool contains(const std::string& key) const;

    /**
      * Returns the index of the specified key which is contianed in this hash table.
      * Returns -1 if the specified key is not contained in this hash table.
      */
    int indexofKey(const std::string& key) const;

    /**
     * Removes the specified element from this hash table. If the element is not
     * present in the hash table, this operation is a no-op.
     *
     * You should implement this operation using tombstone deletion - mark the
     * slot containing the element as a tombstone.
     *
     * Returns true if the element was removed and false otherwise.
     */
    bool remove(const std::string& key);

    /**
     * Prints out relevant information to assist with debugging.
     */
    void printDebugInfo() const;

private:
    /* Type representing a slot in a linear probing table.
     *
     * TODO: When you start implementing the remove operation, edit this
     * type however you'd like in order to support tombstones. Then,
     * delete this comment.
     */
    struct Slot {
        std::string value;
        bool isEmpty;
        bool isTomb;
    };

    /* TODO: Add any private member variables, member functions, or member
     * types that you'd like. You're welcome to implement this type however
     * you'd like, provided that you do all your own memory management, you
     * don't use any container types (e.g. Vector, HashSet, etc.), and that
     * you use linear probing with tombstones.
     *
     * TODO: Delete this comment before submitting.
     */
    Slot* elems;
    int allocatedSize;
    int logicalSize;
    HashFunction<std::string> hashFunString;

    DISALLOW_COPYING_OF(LinearProbingHashTable);
};

#endif
