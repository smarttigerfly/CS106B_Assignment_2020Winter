#include "Combine.h"
#include "Testing/CombineTests.h"
using namespace std;

/* TODO: Refer to Combine.h for more information about what this function does,
 * then delete this comment.
 */
Vector<DataPoint> combine(const Vector<Vector<DataPoint>>& sequences) {
    /* TODO: Delete the next few lines and implement this. */
//    if(sequences.size() < 1)
//        error("The lenth of input is error!");
    if(sequences.size() <= 1)
        return sequences.front();
    int midIndex = sequences.size() / 2;
    return merge(combine(sequences.subList(0, midIndex)),
                 combine(sequences.subList(midIndex)));


}


Vector<DataPoint> merge(const Vector<DataPoint>& left, const Vector<DataPoint>& right)
{
    auto leftIndex = 0;
    int rightIndex = 0;
    Vector<DataPoint> result;
    while(leftIndex < left.size() && rightIndex < right.size())
    {
        if(left.get(leftIndex).weight < right.get(rightIndex).weight)
        {
            result.add(left.get(leftIndex));
            leftIndex++;
        } else
        {
            result.add(right.get(rightIndex));
            rightIndex++;
        }
    }
    if(leftIndex >= left.size())
    {
        result += right.subList(rightIndex);
    }
    if(rightIndex >= right.size())
    {
        result += left.subList(leftIndex);
    }
    return result;
}
/* * * * * * Test Cases Below This Point * * * * * */

/* TODO: Add your own custom tests here! */














/* * * * * Provided Tests Below This Point * * * * */


ADD_TEST("Provided Test: Merges two single-item sequences.") {
    DataPoint amy        = { "Amy Liu",          103 };
    DataPoint katherine  = { "Katherine Erdman", 106 };

    /* Try merging in both orders to make sure the result is correct. */
    auto merged = combine({{ amy }, { katherine }});
    EXPECT_EQUAL(merged.size(), 2);
    EXPECT_EQUAL(merged[0], amy);
    EXPECT_EQUAL(merged[1], katherine);

    merged = combine({{ katherine }, { amy }});
    EXPECT_EQUAL(merged.size(), 2);
    EXPECT_EQUAL(merged[0], amy);
    EXPECT_EQUAL(merged[1], katherine);
}

ADD_TEST("Provided Test: Merges four single-item sequences.") {
    DataPoint amy        = { "Amy Liu",          103 };
    DataPoint katherine  = { "Katherine Erdman", 106 };
    DataPoint isabel     = { "Isabel Bush",      107 };
    DataPoint anna       = { "Anna Saplistki",   161 };

    /* Try merging in several different orders. */
    auto merged = combine({{ amy }, { katherine }, { isabel }, { anna }});
    EXPECT_EQUAL(merged.size(), 4);
    EXPECT_EQUAL(merged[0], amy);
    EXPECT_EQUAL(merged[1], katherine);
    EXPECT_EQUAL(merged[2], isabel);
    EXPECT_EQUAL(merged[3], anna);

    merged = combine({{ katherine }, { isabel }, { anna }, { amy }});
    EXPECT_EQUAL(merged.size(), 4);
    EXPECT_EQUAL(merged[0], amy);
    EXPECT_EQUAL(merged[1], katherine);
    EXPECT_EQUAL(merged[2], isabel);
    EXPECT_EQUAL(merged[3], anna);
}

ADD_TEST("Provided Test: Merges three single-item sequences.") {
    DataPoint amy        = { "Amy Liu",          103 };
    DataPoint katherine  = { "Katherine Erdman", 106 };
    DataPoint isabel     = { "Isabel Bush",      107 };

    /* Try merging in several different orders. */
    auto merged = combine({{ amy }, { katherine }, { isabel }});
    EXPECT_EQUAL(merged.size(), 3);
    EXPECT_EQUAL(merged[0], amy);
    EXPECT_EQUAL(merged[1], katherine);
    EXPECT_EQUAL(merged[2], isabel);

    merged = combine({{ katherine }, { isabel }, { amy }});
    EXPECT_EQUAL(merged.size(), 3);
    EXPECT_EQUAL(merged[0], amy);
    EXPECT_EQUAL(merged[1], katherine);
    EXPECT_EQUAL(merged[2], isabel);
}

ADD_TEST("Provided Test: Merges two lists of different sizes") {
    Vector<DataPoint> one = { { "B" , 3 }};
    Vector<DataPoint> two = {
        { "A" , 2 },
        { "C" , 4 },
        { "D" , 5 },
    };

    auto merged = combine({ one, two });
    EXPECT_EQUAL(merged.size(), 4);
    EXPECT_EQUAL(merged[0], two[0]);
    EXPECT_EQUAL(merged[1], one[0]);
    EXPECT_EQUAL(merged[2], two[1]);
    EXPECT_EQUAL(merged[3], two[2]);
}

ADD_TEST("Provided Test: Merges three lists of different sizes") {
    Vector<DataPoint> one = {
        { "G" , 7 }
    };
    Vector<DataPoint> two = {
        { "A" , 1 },
        { "D" , 4 },
        { "F" , 6 },
    };
    Vector<DataPoint> three = {
        { "B" , 2 },
        { "C" , 3 },
        { "E" , 5 },
        { "H" , 8 },
        { "I" , 9 },
    };

    auto merged = combine({ one, two, three });
    EXPECT_EQUAL(merged.size(), 9);

    Vector<DataPoint> expected;
    for (int i = 1; i <= 9; i++) {
        expected += DataPoint{ string(1, 'A' - 1 + i), i };
    }

    EXPECT_EQUAL(merged, expected);
}

ADD_TEST("Provided Test: Preserves duplicates in input sequences.") {
    Vector<DataPoint> one = {
        { "A" , 1 },
        { "B" , 3 },
    };
    Vector<DataPoint> two = {
        { "A", 1 },
        { "B", 2 },
        { "C", 3 },
        { "D", 4 },
    };
    Vector<Vector<DataPoint>> vec = { one, two };
    auto merged = combine(vec);
    EXPECT_EQUAL(merged.size(), 6);
    EXPECT_EQUAL(merged[0].weight, two[0].weight);
    EXPECT_EQUAL(merged[1].weight, one[0].weight);
    EXPECT_EQUAL(merged[2].weight, two[1].weight);
    EXPECT_EQUAL(merged[3].weight, two[2].weight);
    EXPECT_EQUAL(merged[4].weight, one[1].weight);
    EXPECT_EQUAL(merged[5].weight, two[3].weight);
}

ADD_TEST("Provided Test: Works with negative elements.") {
    Vector<DataPoint> one = {
        { "A" , -5 },
        { "B" , -1 },
        { "C" ,  2 },
    };
    Vector<DataPoint> two = {
        { "A", -2 },
        { "C",  3 },
        { "D",  4 },
    };
    Vector<Vector<DataPoint>> vec = { one, two };
    auto merged = combine(vec);
    EXPECT_EQUAL(merged.size(), 6);
    EXPECT_EQUAL(merged[0], one[0]);
    EXPECT_EQUAL(merged[1], two[0]);
    EXPECT_EQUAL(merged[2], one[1]);
    EXPECT_EQUAL(merged[3], one[2]);
    EXPECT_EQUAL(merged[4], two[1]);
    EXPECT_EQUAL(merged[5], two[2]);
}

ADD_TEST("Provided Test: Works in the example from the handout.") {
    Vector<Vector<int>> sequences = {
        { 3, 8, 10, 11, 13, 16 },
        { 0, 1, 12 },
        { 4, 9, 17, 19 },
        { 7 },
        { 5, 6, 14, 18 },
        { 2, 15 }
    };

    /* The example in the handout uses integers, but we need DataPoints. Tag each integer
     * with an empty string.
     */
    Vector<Vector<DataPoint>> dataPoints;
    for (auto sequence: sequences) {
        Vector<DataPoint> result;
        for (int elem: sequence) {
            result += { "", elem };
        }
        dataPoints += result;
    }

    /* Merge them! */
    auto merged = combine(dataPoints);

    EXPECT_EQUAL(merged.size(), 20);
    for (int i = 0; i < 20; i++) {
        EXPECT_EQUAL(merged[i].weight, i);
    }
}

ADD_TEST("Provided Test: 1,000 sequences with 0 total elements (should take at most a few seconds)") {
    Vector<Vector<DataPoint>> vec;
    for (int i = 0; i < 1000; i++) vec.add({});
    auto merged = combine(vec);
    EXPECT_EQUAL(merged.size(), 0);
}

ADD_TEST("Provided Test: 1,000 sequences with 1 total element (should take at most a few seconds)") {
    DataPoint point = { "I'm the only point!", 1 };
    Vector<Vector<DataPoint>> vec = {{ point }};
    for (int i = 0; i < 1000 - 1; i++) vec.add({});
    auto merged = combine(vec);
    EXPECT_EQUAL(merged.size(), 1);
    EXPECT_EQUAL(merged[0], point);
}

ADD_TEST("Provided Test: 1,000 sequences with 1,000 total elements (should take at most a few seconds)") {
    Vector<Vector<DataPoint>> vec;
    for (int i = 0; i < 1000; i++) vec.add({{ "", i }});
    auto merged = combine(vec);
    EXPECT_EQUAL(merged.size(), 1000);
    EXPECT_EQUAL(merged[0].weight, 0);
    EXPECT_EQUAL(merged[999].weight, 999);
}

ADD_TEST("Provided Test: 100 sequences of random lengths") {
    Vector<Vector<DataPoint>> vec;
    int totalElems = 0;
    Vector<int> elems;
    for (int i = 0; i < 100; i++) {
        int length = randomInteger(0, 20);
        Vector<DataPoint> seq;
        for (int j = 0; j < length; j++) {
            seq.add({ "", j });
            totalElems++;
            elems.add(j);
        }
        vec.add(seq);
    }
    auto merged = combine(vec);
    sort(elems.begin(), elems.end());
    EXPECT_EQUAL(merged.size(), totalElems);
    EXPECT_EQUAL(merged[0].weight, elems[0]);
    EXPECT_EQUAL(merged[totalElems - 1].weight, elems[totalElems - 1]);
}

