#include "GUI/ProblemHandler.h"
#include "Combine.h"
#include "HeapPQueue.h"
#include "TopK.h"
#include "GUI/TemporaryComponent.h"
#include "GUI/GUIUtils.h"
#include "ginteractors.h"
#include "gtimer.h"
#include <functional>
#include <chrono>
#include <cstdint>
#include <string>
#include <limits>
#include <cmath>
using namespace std;

namespace {
    /* Graphics constants. */
    const double kEventTimerSpeed = 1; // Fast enough for things to run smoothly, not enough to drown us
    const string kBackgroundColor = "White";
    const double kPadding = 20;

    /* Axis constants. */
    const string kAxisColor = "#555555"; // Davy's gray
    const string kAxisFont  = "Serif-8";
    const size_t kMaxAxisLabels = 10;    // Max number of major tick marks on the X axis

    /* Content areas. */
    const double kHeaderHeight    = 50;
    const string kHeaderFont      = "Serif-BOLDITALIC-24";
    const string kHeaderFontColor = kAxisColor;

    /* Legend area, expressed as an offset from the start of the chart area. */
    const double kLegendXOffset   = 100;
    const double kLegendYOffset   =   0;
    const double kLegendWidth     = 150;
    const double kLegendHeight    = 200;
    const string kLegendFont      = "SansSerif-12";
    const string kLegendFontColor = kAxisColor;


    /**
     * A type representing a stopwatch. This is used to time the costs of various
     * operations.
     */
    class Timer {
    public:
      void start() {
        current = chrono::high_resolution_clock::now();
      }

      void stop() {
        total += chrono::high_resolution_clock::now() - current;
      }

      double elapsed() const {
        return chrono::duration_cast<chrono::nanoseconds>(total).count();
      }

    private:
      chrono::high_resolution_clock::duration total =
        chrono::high_resolution_clock::duration::zero();
      chrono::high_resolution_clock::time_point current;
    };

    /* All the information needed to run a time test. */
    struct TimeTest {
        function<void (size_t n, size_t k, Timer& timer)> callback; // Given n, k, and a timer, do work.
        vector<size_t> ns, ks;                                      // Values of n and k
        string name;                                                // What we're testing
    };

    /* Generates numbers in linear and exponential sequences */
    vector<size_t> linRange(size_t low, size_t high, size_t step) {
        vector<size_t> result;
        for (; low <= high; low += step) {
            result.push_back(low);
        }
        return result;
    }

    vector<size_t> expRange(size_t low, size_t high, double step) {
        vector<size_t> result;
        for (; low <= high; low *= step) {
            result.push_back(low);
        }
        return result;
    }

    /* Test cases for multiway merge. */
    TimeTest combineTests() {
        return {
            [](int n, int k, Timer& timer) {
                /* Make a bunch of vectors to merge. */
                Vector<Vector<DataPoint>> toMerge(k);

                /* Add elements in following a mostly geometric distribution. */
                for (int i = 0; i < n; i++) {
                    int index = 0;
                    while (index < k - 1 && randomBool()) index++;

                    toMerge[index] += { to_string(i), i };
                }

                /* Run the multiway merge. */
                timer.start();
                (void) combine(toMerge);
                timer.stop();
            },
            linRange(10000, 100000, 5000),
            expRange(1, 1 << 12, 4),
            "Combine"
        };
    }

    /* Test cases for the heap. We just do a simple heapsort. */
    TimeTest heapPQueueTests() {
        static const int kNumTrials = 25;
        return {
            [](int n, int, Timer& timer) {
                for (int round = 0; round < kNumTrials; round++) {
                    HeapPQueue hpq;

                    /* Enqueue n elements. */
                    for (int i = 0; i < n; i++) {
                        DataPoint toAdd = { "", randomInteger(0, numeric_limits<int>::max()) };
                        timer.start();
                        hpq.enqueue(toAdd);
                        timer.stop();
                    }

                    /* Dequeue all elements. */
                    for (int i = 0; i < n; i++) {
                        timer.start();
                        (void) hpq.dequeue();
                        timer.stop();
                    }
                }
            },
            linRange(0, 50000, 500),
            { 1 }, // Unused; doesn't matter
            "HeapPQueue (enqueue/dequeue n elements)"
        };
    }

    /* Test cases for top-k. */
    TimeTest topKTests() {
        return {
            [](int n, int k, Timer& timer) {
                stringstream source;

                /* Fill the source in with n elements provided in an order where everything
                 * must be inserted.
                 */
                for (int i = 0; i < n; i++) {
                    source << DataPoint{"", i};
                }

                /* Run the top-k search. */
                timer.start();
                (void) topK(source, k);
                timer.stop();
            },
            linRange(10000, 100000, 5000),
            expRange(1, 1 << 12, 4),
            "Streaming Top-K"
        };
    }

    /* Colors to use for various values of k. */
    const vector<string> kLineColors = {
        "#CC0000", // Rosso Corsa
        "#EE7F2D", // Princeton Orange
        "#FFC40C", // Mikado Yellow
        "#008000", // Office Green
        "#007BA7", // Cerulean
        "#B53389", // Fandango
        "#343434", // Jet
    };

    /* Problem handler that reports timing information for the different types. */
    class PerformanceGUI: public ProblemHandler {
    public:
        PerformanceGUI(GWindow& window);

        void timerFired() override;
        void actionPerformed(GObservable* source) override;

    protected:
        void repaint(GWindow& window) override;

    private:
        /* Each timer tick steps us through the next test case. */
        GTimer timer{kEventTimerSpeed};

        /* Which test, if any, we're doing. */
        bool isActive = false;
        TimeTest currTest;

        size_t nextN = 0, nextK = 0;    // Next values of n and k to use.
        vector<string> axisLabels{2};   // Labels to use on the x axis. Defaults to two blank strings
        size_t axisTicks = 0;           // Minor ticks to use on the x axis

        /* Test case results so far. */
        Vector<Vector<double>> results;

        /* Buttons for what to test. */
        Temporary<GButton> combine;
        Temporary<GButton> heapPQ;
        Temporary<GButton> topK;

        void startTests(const TimeTest& test);
        void stepTests();

        void drawChart (GWindow& window, const GRectangle& bounds);
        void drawHeader(GWindow& window, const GRectangle& bounds);
        void drawLegend(GWindow& window, const GRectangle& bounds);
    };

    PerformanceGUI::PerformanceGUI(GWindow& window) {
        combine = Temporary<GButton>(new GButton("Combine"),      window, "SOUTH");
        heapPQ  = Temporary<GButton>(new GButton("Heap Priority Queue"), window, "SOUTH");
        topK    = Temporary<GButton>(new GButton("Streaming Top-K"),     window, "SOUTH");
    }

    void PerformanceGUI::stepTests() {
        /* Otherwise, run the next test. */
        Timer timer;
        currTest.callback(currTest.ns[nextN], currTest.ks[nextK], timer);

        /* Store the result. */
        results.back() += timer.elapsed();

        /* Step forward. */
        nextN++;

        /* Finished all n's? Advance k. */
        if (nextN == currTest.ns.size()) {
            nextN = 0;
            nextK++;

            /* Finished all k's? We're done! */
            if (nextK == currTest.ks.size()) {
                timer.stop();
                isActive = false;
                return;
            }

            /* Need a new place to store our results. */
            results.add({});
        }
    }

    void PerformanceGUI::timerFired() {
        /* If we don't have an active test, there's nothing to do. */
        if (!isActive) {
            timer.stop();
            return;
        }

        stepTests();

        /* Need to redraw things! */
        requestRepaint();
    }

    /* Begins a new testing session. */
    void PerformanceGUI::startTests(const TimeTest& test) {
        /* Clear out all old test information. */
        currTest = test;

        /* Reset state. */
        nextN = nextK = 0;
        results.clear();
        results.add({});

        /* Set up x-axis labels. */
        axisLabels.clear();

        /* We will never have more than kMaxAxisLabels labels on the X axis. If we
         * do, we will start introducing minor tickmarks.
         *
         * The way we're going to do this is as follows. We'll compute
         * ceil(#labels / kAxisLabels), which gives back the number of tick skips
         * to use. For example, if we want 11 labels and the max is 10, we'll compute
         * ceil(11 / 10) - 1 = 2, and have one minor tick in between each set
         * of major ticks.
         *
         * We actually compute this using the technique that (a + b - 1) / b computes
         * ceil(a / b).
         */
        axisTicks = ((test.ns.size() + kMaxAxisLabels - 1) / kMaxAxisLabels);
        size_t numMajor = (test.ns.size() + axisTicks - 1) / axisTicks;

        for (size_t i = 0; i < numMajor; i++) {
            size_t effectiveIndex = i * axisTicks; // We're skipping things, after all!

            string label;
            if (effectiveIndex == 0) {
                label = "n=" + to_string(test.ns[effectiveIndex]);
            } else if (effectiveIndex < test.ns.size()) {
                label = to_string(test.ns[effectiveIndex]);
            } else {
                label = ""; // Not needed, but helps clarify intent.
            }

            axisLabels.push_back(label);
        }

        /* The axisTicks parameter actually literally controls how many minor ticks are
         * drawn, not the skip size between major ticks. As a result, we need to subtract
         * one here in order to ensure that we draw teh right number of minor ticks.
         */
        axisTicks--;

        isActive = true;
        timer.start();
    }


    void PerformanceGUI::actionPerformed(GObservable* source) {
        if (source == combine) {
            startTests(combineTests());
        } else if (source == heapPQ) {
            startTests(heapPQueueTests());
        } else if (source == topK) {
            startTests(topKTests());
        }
    }

    void PerformanceGUI::repaint(GWindow& window) {
        clearDisplay(window, kBackgroundColor);

        /* Space for the header. */
        GRectangle header = {
            kPadding, kPadding,
            window.getCanvasWidth() - 2 * kPadding,
            kHeaderHeight
        };

        double chartTop = header.getY() + header.getHeight();

        /* Set up the chart. */
        GRectangle chart = {
            kPadding, chartTop,
            window.getCanvasWidth()  - 2 * kPadding,
            window.getCanvasHeight() - chartTop - kPadding
        };

        /* And the legend. */
        GRectangle legend = {
            chart.getX() + kLegendXOffset, chart.getY() + kLegendYOffset,
            kLegendWidth, kLegendHeight
        };

        drawHeader(window, header);
        drawChart (window, chart);
        drawLegend(window, legend);
    }

    void PerformanceGUI::drawHeader(GWindow& window, const GRectangle& bounds) {
        GText label(currTest.name);
        label.setFont(kHeaderFont);
        label.setColor(kHeaderFontColor);

        label.setLocation(bounds.getX() + (bounds.getWidth() - label.getWidth()) / 2.0,
                          bounds.getY() + (bounds.getHeight() - label.getHeight()) / 2.0 + label.getFontAscent());

        window.draw(&label);
    }

    void PerformanceGUI::drawChart(GWindow& window, const GRectangle& bounds) {
        /* Y-axis labels will be empty. We need a minimum of two, though, so we'll make those. */
        vector<string> yLabels(2);

        /* Convert our results into line segments. */

        /* Find the maximum value across everything. */
        double maxValue = 0;
        double minValue = numeric_limits<double>::infinity();
        for (const auto& result: results) {
            for (double val: result) {
                maxValue = max(val, maxValue);
                minValue = min(val, minValue);
            }
        }

        /* Nudge the max value up a bit to avoid dividing by zero. */
        maxValue = nextafter(maxValue, numeric_limits<double>::infinity());

        /* Normalize everything by that amount. */
        vector<vector<GPoint>> lines;
        for (const auto& result: results) {
            vector<GPoint> line;
            for (int i = 0; i < result.size(); i++) {
                line.push_back({ i * 1.0 / (currTest.ns.size() - 1), (result[i] - minValue) / (maxValue - minValue) });
            }
            lines.push_back(line);
        }

        /* Draw it! */
        LineGraphRender::construct(lines, axisLabels, yLabels, axisTicks, 0, bounds, kAxisFont, kAxisFont, kLineColors, kAxisColor)->draw(window);
    }

    void PerformanceGUI::drawLegend(GWindow& window, const GRectangle& bounds) {
        /* If there is only one value of k, don't draw anything. */
        if (currTest.ks.size() <= 1) return;

        /* Otherwise, there are many values of k. Assemble labels based on the number of lines
         * we have drawn so far.
         */
        vector<string> labels;
        for (size_t i = 0; i < currTest.ks.size() && i < size_t(results.size()); i++) {
            labels.push_back("k = " + to_string(currTest.ks[i]));
        }

        /* Set up the legend. Don't draw it yet; we need to clear the area behind it. */
        auto legend = LegendRender::construct(labels, kLineColors, bounds, kLegendFontColor, kLegendFont, kLegendFontColor);
        window.setColor(kBackgroundColor);
        window.fillRect(legend->computedBounds());

        /* Now draw it. */
        legend->draw(window);
    }
}

shared_ptr<ProblemHandler> makeTimeTestsGUI(GWindow& window) {
    return make_shared<PerformanceGUI>(window);
}
