#include "Crystal.h"
#include "gwindow.h"
#include "gthread.h"
#include "ginteractors.h"
#include <algorithm>
#include <limits>
#include <vector>
#include <memory>
using namespace std;

namespace {
    /* Controls for the advance rate. Positive values mean "pause this much time
     * between frames." Negative values mean "advance this many frames on each step."
     */
    const vector<int> kAdvanceRates = {
        2000,
        1000,
        500,
        250,
        125,
        62,
        30,
        15,
        -1,
        -2,
        -4,
    };

    /* Window size. */
    const double kWindowWidth  = 1000;
    const double kWindowHeight = 700;

    /* Constraints on how big or small a cell can display. */
    const double kMaxCellSize  = 32;
    const double kMinCellSize  = 1;

    /* Margin from the border of the window to the actual crystal. */
    const double kWindowPadding = 20;

    /* Font to use for displaying the generation number. */
    const string kLabelFontColor = "#FFFF99"; // Canary Yellow
    const string kLabelFont      = "Monospaced-BOLD-12";

    /* Foreground and background colors. */
    const Vector<string> kColors = {
        "#400020",
        "#D0D0FF"
    };

    /* Window title. */
    const string kWindowTitle = "Crystals";

    /* Type: Geometry
     *
     * Information about how big each cell should be and how to display things properly in the
     * window.
     */
    struct Geometry {
        int minX, minY;      // Lowest X and Y coordinate of any cell
        double baseX, baseY; // Offset from the upper-left corner of the screen to the content
        double cellSize;
    };

    /* Given a crystal, computes the geometry used to draw it. */
    Geometry geometryFor(const Crystal& crystal, GWindow& window) {
        /* Compute the bounding box. */
        int minX = numeric_limits<int>::max();
        int minY = numeric_limits<int>::max();

        int maxX = numeric_limits<int>::min();
        int maxY = numeric_limits<int>::min();

        /* Expand our box around the spots used in the crystal. */
        for (const auto& loc: crystal.used) {
            minX = min(minX, loc.getX());
            minY = min(minY, loc.getY());

            maxX = max(maxX, loc.getX());
            maxY = max(maxY, loc.getY());
        }

        /* Get the width and height of the world. Use doubles to avoid integer overflows.
         * Add one because the width and height are inclusive.
         */
        double crystalWidth  = double(maxX) - double(minX) + 1;
        double crystalHeight = double(maxY) - double(minY) + 1;

        /* Get the width and height of the screen. */
        double windowWidth  = window.getCanvasWidth()  - 2 * kWindowPadding;
        double windowHeight = window.getCanvasHeight() - 2 * kWindowPadding;

        /* Computing the scaling factors needed to scale to the window width and window height. */
        double scaleX = windowWidth / crystalWidth;
        double scaleY = windowHeight / crystalHeight;
        double scale = min(scaleX, scaleY);

        /* Don't make things any bigger than some maximum threshold. This is purely for
         * aesthetics.
         */
        if (scale > kMaxCellSize) scale = kMaxCellSize;

        /* Compute base x and y based on the scale. */
        double baseX = kWindowPadding + (windowWidth  - crystalWidth  * scale) / 2.0;
        double baseY = kWindowPadding + (windowHeight - crystalHeight * scale) / 2.0;

        return {
            minX, minY,
            baseX, baseY,
            scale
        };
    }

    /* Draws the given crystal within the specified window. */
    void drawCrystal(const Crystal& crystal, GWindow& window) {
        /* Edge case: there's nothing to draw in an empty world. */
        if (crystal.used.isEmpty()) return;

        auto g = geometryFor(crystal, window);

        /* For efficiency's sake, only make one GRect. */
        /* The cell size may be fractional. In that case, the graphics system
         * can't render an individual cell. We'll therefore artificially boost the
         * cell size to be at least one.
         */
        GRect toDraw(0, 0, max(kMinCellSize, g.cellSize), max(kMinCellSize, g.cellSize));
        toDraw.setFilled(true);
        toDraw.setColor(kColors[1]);
        toDraw.setFillColor(kColors[1]);

        /* Draw each cell. */
        for (auto loc: crystal.used) {
            double x = g.baseX + (loc.getX() - g.minX) * g.cellSize;
            double y = g.baseY + (loc.getY() - g.minY) * g.cellSize;

            toDraw.setLocation(x, y);
            window.draw(toDraw);
        }
    }

    /* Inserts commas into a numeric string. */
    string addCommasTo(int val) {
        string asStr = to_string(val);
        const int length = int(asStr.length());

        string result;
        for (int i = 0; i < length; i++) {
            /* Run backwards through the source string so determining where commas go
                 * becomes a lot easier.
                 */
            result = asStr[length - 1 - i] + result;

            /* Put commas in provided we've already added three characters, but aren't
                 * about to use all the digits up.
                 */
            if (i % 3 == 2 && i < length - 1) {
                result = ',' + result;
            }
        }
        return result;
    }

    /* Renders a legend describing how many steps have been taken. */
    void drawLegend(int generation, GWindow& window) {
        GText label("Steps: " + addCommasTo(generation));
        label.setFont(kLabelFont);
        label.setColor(kLabelFontColor);
        label.setLocation(0, window.getCanvasHeight() - label.getFontDescent());
        window.draw(&label);
    }

    void draw(const Crystal& crystal, int generation, GWindow& window) {
        GThread::runOnQtGuiThread([&] {
            window.setColor(kColors[0]);
            window.fillRect(0, 0, window.getCanvasWidth(), window.getCanvasHeight());

            drawCrystal(crystal, window);
            drawLegend(generation, window);

            window.repaint();
        });
    }

    /* Graphics struct. Holds all the information necessary to render things. */
    struct Graphics {
        unique_ptr<GWindow> window;
        unique_ptr<GSlider> slider;
    };

    unique_ptr<Graphics> makeGraphics() {
        /* TODO: Switch to make_unique when all compilers support it. */
        unique_ptr<Graphics> result(new Graphics());

        /* Set up the window. */
        result->window.reset(new GWindow(kWindowWidth, kWindowHeight));
        result->window->setExitOnClose(true);
        result->window->setTitle(kWindowTitle);
        result->window->setRepaintImmediately(false);

        /* Set up the slider. */
        result->slider.reset(new GSlider(0, kAdvanceRates.size() - 1, 0));
        result->slider->setPaintTicks(true);
        result->window->addToRegion(new GLabel("Simulation Speed: "), "SOUTH");
        result->window->addToRegion(result->slider.get(), "SOUTH");

        return result;
    }
}

int multimain(CrystalGraphics) {
    auto graphics = makeGraphics();

    /* Seed the crystal at the origin. */
    Crystal crystal;
    crystallizeAt(crystal, 0, 0);

    /* How many steps we've taken. */
    int generation = 1;

    /* Watch it evolve! */
    while (true) {
        draw(crystal, generation, *graphics->window);

        /* Step the animation forward. If we have a positive pause time, pause for
         * that amount.
         */
        int speed = kAdvanceRates[graphics->slider->getValue()];
        if (speed > 0) {
            pause(speed);
        }

        /* Run an appropriate number of steps. If the pause time is positive, that's
         * one frame. Otherwise, it's the number of frames given by the pause time.
         */
        for (int i = 0; i < max(1, -speed); i++) {
            step(crystal);
            generation++;
        }
    }
}
