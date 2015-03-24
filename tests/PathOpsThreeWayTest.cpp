/*
 * Copyright 2014 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
#include "SkIntersections.h"
#include "SkTDArray.h"
#include "Test.h"

// check intersections for consistency

struct Curve {
    int ptCount;
    SkDCubic curve;  // largest can hold lines / quads/ cubics
};

static const Curve testSet0[] = {  // extracted from skpClip2
    {4, {{{134,11414}, {131.990234,11414}, {130.32666,11415.4824}, {130.042755,11417.4131}}} },
    {4, {{{130.042755,11417.4131}, {130.233124,11418.3193}, {131.037079,11419}, {132,11419}}} },
    {4, {{{132,11419}, {130.895432,11419}, {130,11418.1045}, {130,11417}}} },
};

static const Curve testSet1[] = {  // extracted from cubicOp85i
    {4, {{{3,4}, {1,5}, {4,3}, {6,4}}} },
    {1, {{{6,4}, {3,4}}} },
    {4, {{{3,4}, {4,6}, {4,3}, {5,1}}} },
    {1, {{{5,1}, {3,4}}} },
};

static const struct TestSet {
    const Curve* tests;
    int testCount;
} testSets[] = {
    { testSet0, (int) SK_ARRAY_COUNT(testSet0) },
    { testSet1, (int) SK_ARRAY_COUNT(testSet1) },
};

static const int testSetsCount = (int) SK_ARRAY_COUNT(testSets);

static void testSetTest(skiatest::Reporter* reporter, int index) {
    const TestSet& testSet = testSets[index];
    int testCount = testSet.testCount;
    SkASSERT(testCount > 1);
    SkTDArray<SkIntersections> combos;
    for (int outer = 0; outer < testCount - 1; ++outer) {
        const Curve& oTest = testSet.tests[outer];
        for (int inner = outer + 1; inner < testCount; ++inner) {
            const Curve& iTest = testSet.tests[inner];
            SkIntersections* i = combos.append();
            sk_bzero(i, sizeof(SkIntersections));
            if (oTest.ptCount == 1 && iTest.ptCount == 1) {
                i->intersect(*(const SkDLine*) &oTest.curve, *(const SkDLine*) &iTest.curve);
            } else if (oTest.ptCount == 1 && iTest.ptCount == 4) {
                i->intersect(iTest.curve, *(const SkDLine*) &oTest.curve);
            } else if (oTest.ptCount == 4 && iTest.ptCount == 1) {
                i->intersect(oTest.curve, *(const SkDLine*) &oTest.curve);
            } else if (oTest.ptCount == 4 && iTest.ptCount == 4) {
                i->intersectB(oTest.curve, iTest.curve);
            } else {
                SkASSERT(0);
            }
//            i->dump();
        }
    }
}

DEF_TEST(PathOpsThreeWay, reporter) {
    for (int index = 0; index < testSetsCount; ++index) {
        testSetTest(reporter, index);
        reporter->bumpTestCount();
    }
}

DEF_TEST(PathOpsThreeWayOneOff, reporter) {
    int index = 1;
    testSetTest(reporter, index);
}
