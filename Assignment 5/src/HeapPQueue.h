#ifndef HeapPQueue_Included
#define HeapPQueue_Included

#include "Demos/DataPoint.h"

/**
 * Priority queue type implemented using a binary heap. Refer back to the assignment handout
 * for details about how binary heaps work.
 *
 * As a reminder, you are required to do all your own memory management using new[] and
 * delete[].
 */
class HeapPQueue {
public:
    /**
     * Creates a new, empty priority queue.
     */
    HeapPQueue();

    /**
     * Cleans up all memory allocated by this priorty queue. Remember, you're responsible
     * for managing your own memory!
     */
    ~HeapPQueue();

    /**
     * Adds a new data point into the queue. This operation runs in time O(log n),
     * where n is the number of elements in the queue.
     *
     * @param data The data point to add.
     */
    void enqueue(const DataPoint& data);

    /**
     * Removes and returns the lowest-weight data point in the priority queue. If multiple
     * elements are tied for having the loweset weight, any one of them may be returned.
     *
     * If the priority queue is empty, this function calls error() to report an error.
     *
     * This operation must run in time O(log n), where n is the number of elements in the
     * queue.
     *
     * @return The lowest-weight data point in the queue.
     */
    DataPoint dequeue();

    /**
     * Returns, but does not remove, the element that would next be removed via a call to
     * dequeue.
     *
     * If the priority queue is empty, this function calls error() to report an error.
     *
     * This operation must run in time O(1).
     *
     * @return
     */
    DataPoint peek() const;

    /**
     * Returns whether the priority queue is empty.
     *
     * This operation must run in time O(1).
     *
     * @return Whether the priority queue is empty.
     */
    bool isEmpty() const;

    /**
     * Returns the number of data points in this priority queue.
     *
     * This operation must run in time O(1).
     *
     * @return The number of elements in the priority queue.
     */
    int  size() const;

    /* This function exists purely for testing purposes. You can have it do whatever you'd
     * like and we won't be invoking it when grading. In the past, students have had this
     * function print out the array representing the heap, or information about how much
     * space is allocated, etc. Feel free to use it as you see fit!
     */
    void printDebugInfo();

private:
    /* TODO: Add whatever fields, types, and helper functions that you'd like! */
    void swap(DataPoint& left, DataPoint& right);
    int isRightPos(const int pos);
    void grow();

    DataPoint* elems;
    int allocatedSize;
    int logicalSize;




    /* Weird C++isms: C++ loves to make copies of things, which is usually a good thing but
     * for the purposes of this assignment requires some C++ knowledge we haven't yet covered.
     * This code disables all copy functions to make sure you don't accidentally end up
     * debugging something that isn't your fault.
     *
     * Curious what this does? Take CS106L!
     */
    HeapPQueue(const HeapPQueue &) = delete;
    HeapPQueue(HeapPQueue &&) = delete;
    void operator= (HeapPQueue) = delete;
};

#endif
