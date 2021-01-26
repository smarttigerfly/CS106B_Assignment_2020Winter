#include "ProblemHandler.h"
#include "DisasterParser.h"
#include "TemporaryComponent.h"
#include "GUIUtils.h"
#include <fstream>
#include <memory>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <vector>
#include "filelib.h"
#include "strlib.h"
#include "gthread.h"
using namespace std;

namespace {
    /* File constants. */
    const string kProblemSuffix = ".dst";
    const string kBasePath = "./";

    /* Background color. */
    const string kBackgroundColor  = "#000000";

    /* Colors to use when drawing the network. */
    struct CityColors {
        string borderColor;
        string fillColor;
        string fontColor;
    };

    enum CityState { // "The state the city is in," not "Singapore." :-)
        UNCOVERED,
        COVERED_INDIRECTLY,
        COVERED_DIRECTLY
    };

    /* Colors to use when drawing cities. */
    const vector<CityColors> kCityColors = {
        { "#101010", "#202020", "#A0A0A0" },   // Uncovered
        { "#303060", "#404058", "#C0C0C0" },   // Indirectly covered
        { "#806030", "#FFDF80", "#000000" },   // Directly covered
    };



    /* Colors to use to draw the roads. */
    const string kDarkRoadColor    = "#505050";
    const string kLightRoadColor   = "#FFFFFF";

    /* Line thicknesses. */
    const double kRoadWidth = 3;
    const double kCityWidth = 1.5;

    /* Font to use for city labels. */
    const string kLabelFont        = "Monospace-BOLD-12";

    /* Radius of a city */
    const double kCityRadius = 25;

    /* Buffer space around the window. */
    const double kBufferSpace = 60;

    /* Lower bound on the width or height of the data range, used for
     * collinear points.
     */
    const double kLogicalPadding = 1e-5;

    /* Max length of a string in a label. */
    const string::size_type kMaxLength = 3;

    /* Geometry information for drawing the network. */
    struct Geometry {
        /* Range of X and Y values in the data set, used for
         * scaling everything.
         */
        double minDataX, minDataY, maxDataX, maxDataY;

        /* Range of X and Y values to use when drawing everything. */
        double minDrawX, minDrawY, maxDrawX, maxDrawY;
    };

    /* Given a data set, fills in the min and max X and Y values
     * encountered in that set.
     */
    void computeDataBounds(const DisasterTest& network, Geometry& geo) {
        geo.minDataX = geo.minDataY = numeric_limits<double>::infinity();
        geo.maxDataX = geo.maxDataY = -numeric_limits<double>::infinity();

        for (const string& cityName: network.cityLocations) {
            geo.minDataX = min(geo.minDataX, network.cityLocations[cityName].getX());
            geo.minDataY = min(geo.minDataY, network.cityLocations[cityName].getY());

            geo.maxDataX = max(geo.maxDataX, network.cityLocations[cityName].getX());
            geo.maxDataY = max(geo.maxDataY, network.cityLocations[cityName].getY());
        }

        /* Pad the boundaries. This accounts for the edge case where one set of bounds is
         * degenerate.
         */
        geo.minDataX -= kLogicalPadding;
        geo.minDataY -= kLogicalPadding;
        geo.maxDataX += kLogicalPadding;
        geo.maxDataY += kLogicalPadding;
    }

    /* Once we have the data bounds, we can compute the graphics bounds,
     * which will try to take maximum advantage of the width and height
     * that we have available to us.
     */
    void computeGraphicsBounds(GWindow& window, Geometry& geo) {
        /* Get the aspect ratio of the window. */
        double winWidth  = window.getCanvasWidth()  - 2 * kBufferSpace;
        double winHeight = window.getCanvasHeight() - 2 * kBufferSpace;
        double winAspect = winWidth / winHeight;

        /* Get the aspect ratio of the data set. */
        double dataAspect = (geo.maxDataX - geo.minDataX) / (geo.maxDataY - geo.minDataY);

        double dataWidth, dataHeight;

        /* If the data aspect ratio exceeds the window aspect ratio,
         * the limiting factor in the display is going to be the
         * width. Therefore, we'll use that to determine our effective
         * width and height.
         */
        if (dataAspect >= winAspect) {
            dataWidth = winWidth;
            dataHeight = dataWidth / dataAspect;
        } else {
            dataHeight = winHeight;
            dataWidth = dataAspect * dataHeight;
        }

        /* Now, go center that in the window. */
        geo.minDrawX = (winWidth  -  dataWidth) / 2.0 + kBufferSpace;
        geo.minDrawY = (winHeight - dataHeight) / 2.0 + kBufferSpace;

        geo.maxDrawX = geo.minDrawX + dataWidth;
        geo.maxDrawY = geo.minDrawY + dataHeight;
    }

    /* Given the road network, determines its geometry. */
    Geometry geometryFor(GWindow& window, const DisasterTest& network) {
        Geometry result;
        computeDataBounds(network, result);
        computeGraphicsBounds(window, result);
        return result;
    }

    /* Converts a coordinate in logical space into a coordinate in
     * physical space.
     */
    GPoint logicalToPhysical(const GPoint& pt, const Geometry& geo) {
        double x = ((pt.getX() - geo.minDataX) / (geo.maxDataX - geo.minDataX)) * (geo.maxDrawX - geo.minDrawX) + geo.minDrawX;
        double y = ((pt.getY() - geo.minDataY) / (geo.maxDataY - geo.minDataY)) * (geo.maxDrawY - geo.minDrawY) + geo.minDrawY;

        return { x, y };
    }

    /* Draws all the roads in the network, highlighting ones that
     * are adjacent to lit cities.
     */
    void drawRoads(GWindow& window,
                   const Geometry& geo,
                   const DisasterTest& network,
                   const HashSet<string>& selected) {
        /* For efficiency's sake, just create one line. */
        GLine toDraw;
        toDraw.setLineWidth(kRoadWidth);

        for (const string& source: network.network) {
            for (const string& dest: network.network[source]) {
                /* Selected roads draw in the bright color; deselected
                 * roads draw in a the dark color.
                 */
                toDraw.setColor((selected.contains(source) || selected.contains(dest))? kLightRoadColor : kDarkRoadColor);

                /* Draw the line, remembering that the coordinates are in
                 * logical rather than physical space.
                 */
                auto src = logicalToPhysical(network.cityLocations[source], geo);
                auto dst = logicalToPhysical(network.cityLocations[dest], geo);
                toDraw.setStartPoint(src.getX(), src.getY());
                toDraw.setEndPoint(dst.getX(), dst.getY());

                window.draw(toDraw);
            }
        }
    }

    /* Returns a shortened name for the given city name. We use the first
     * three letters of the name if it's a single word and otherwise use
     * its initials.
     */
    string shorthandFor(const string& name) {
        auto components = stringSplit(name, " ");
        if (components.size() == 0) {
            error("It shouldn't be possible for there to be no components of the city name.");
            return "";
        } else if (components.size() == 1) {
            if (components[0].length() < kMaxLength) return components[0];
            else return components[0].substr(0, 3);
        } else {
            /* Use initials. */
            string result;
            for (size_t i = 0; result.length() < kMaxLength && i < components.size(); i++) {
                /* Skip empty components, which might exist if there are consecutive spaces in
                 * the name
                 */
                if (!components[i].empty()) {
                    result += components[i][0];
                }
            }
            return result;
        }
    }

    /* Draws all the cities, highlighting the ones that are in the
     * selected set.
     */
    void drawCities(GWindow& window,
                    const Geometry& geo,
                    const DisasterTest& network,
                    const HashSet<string>& selected) {

        /* For simplicity, just make a single label and oval. */
        GText label;
        label.setFont(kLabelFont);

        GOval oval(0, 0, 2 * kCityRadius, 2 * kCityRadius);
        oval.setLineWidth(kCityWidth);
        oval.setFilled(true);

        for (const string& city: network.network) {
            /* Figure out the center of the city on the screen. */
            auto center = logicalToPhysical(network.cityLocations[city], geo);

            /* See what state the city is in with regards to coverage. */
            CityState state = UNCOVERED;
            if (selected.contains(city)) state = COVERED_DIRECTLY;
            else if (!(selected * network.network[city]).isEmpty()) state = COVERED_INDIRECTLY;

            /* There's no way to draw a filled circle with a boundary as one call. */
            oval.setColor(kCityColors[state].borderColor);
            oval.setFillColor(kCityColors[state].fillColor);
            window.draw(oval,
                        center.getX() - kCityRadius,
                        center.getY() - kCityRadius);

            /* Set the label text and color. */
            label.setLabel(shorthandFor(city));
            label.setColor(kCityColors[state].fontColor);

            window.draw(label,
                        center.getX() - label.getWidth() / 2.0,
                        center.getY() + label.getFontAscent() / 2.0);
        }
    }

    void visualizeNetwork(GWindow& window,
                          const DisasterTest& network,
                          const HashSet<string>& selected) {
        clearDisplay(window, kBackgroundColor);

        /* Edge case: Don't draw if the window is too small. */
        if (window.getCanvasWidth()  <= 2 * kBufferSpace ||
            window.getCanvasHeight() <= 2 * kBufferSpace) {
            return;
        }

        /* There's a weird edge case where if there are no cities,
         * the window geometry can't be calculated properly. Therefore,
         * we're going skip all this logic if there's nothing to draw.
         */
        if (!network.network.isEmpty()) {
            Geometry geo = geometryFor(window, network);

            /* Draw the roads under the cities to avoid weird graphics
             * artifacts.
             */
            drawRoads(window, geo, network, selected);
            drawCities(window, geo, network, selected);
        }
    }

    vector<string> sampleProblems(const string& basePath) {
        vector<string> result;
        for (const auto& file: listDirectory(basePath)) {
            if (endsWith(file, kProblemSuffix)) {
                result.push_back(file);
            }
        }
        return result;
    }

    /* Uses binary search to find the optimal number of cities to use for disaster
     * preparedness, populating the result field with the minimum group of cities
     * that ended up being needed.
     */
    void solveOptimally(const DisasterTest& test, HashSet<string>& result) {
        /* The variable 'low' is the lowest number that might be feasible.
         * The variable 'high' is the highest number that we know is feasible.
         */
        int low = 0, high = test.network.size();

        /* Begin with a feasible solution that uses as many cities as we'd like. */
        (void) canBeMadeDisasterReady(test.network, high, result);

        while (low < high) {
            /* This line looks weird, but it's designed to avoid integer overflows
             * on large inputs. The idea that (high + low) can overflow, but
             * (high - low) / 2 never will.
             */
            int mid = low + (high - low) / 2;
            HashSet<string> thisResult;

            /* If this option works, decrease high to it, since we know all is good. */
            if (canBeMadeDisasterReady(test.network, mid, thisResult)) {
                high = mid;
                result = thisResult; // Remember this result for later.
            }
            /* Otherwise, rule out anything less than or equal to it. */
            else {
                low = mid + 1;
            }
        }
    }

    class DisasterGUI: public ProblemHandler {
    public:
        DisasterGUI(GWindow& window);

        void actionPerformed(GObservable* source) override;
        void changeOccurredIn(GObservable* source) override;

    protected:
        void repaint(GWindow& window) override;

    private:
        /* Dropdown of all the problems to choose from. */
        Temporary<GComboBox> mProblems;

        /* Button to trigger the solver. */
        Temporary<GButton> mSolve;

        /* Current network and solution. */
        DisasterTest    mNetwork;
        HashSet<string> mSelected;

        /* Loads the world with the given name. */
        void loadWorld(const string& filename);

        /* Computes an optimal solution. */
        void solve();
    };

    DisasterGUI::DisasterGUI(GWindow& window) {
        GComboBox* choices = new GComboBox();
        for (const string& file: sampleProblems(kBasePath)) {
            choices->addItem(file);
        }
        choices->setEditable(false);

        mProblems = Temporary<GComboBox>(choices, window, "SOUTH");
        mSolve    = Temporary<GButton>(new GButton("Solve"), window, "SOUTH");

        loadWorld(choices->getSelectedItem());
    }

    void DisasterGUI::changeOccurredIn(GObservable* source) {
        if (source == mProblems) {
            loadWorld(mProblems->getSelectedItem());
        }
    }

    void DisasterGUI::actionPerformed(GObservable* source) {
        if (source == mSolve) {
            solve();
        }
    }

    void DisasterGUI::repaint(GWindow& window) {
        visualizeNetwork(window, mNetwork, mSelected);
    }

    void DisasterGUI::loadWorld(const string& filename) {
        ifstream input(kBasePath + filename);
        if (!input) error("Cannot open file.");

        mNetwork = loadDisaster(input);
        mSelected.clear();
        requestRepaint();
    }

    void DisasterGUI::solve() {
        /* Clear out any old solution. We're going to get a new one. */
        mSelected.clear();

        /* Disable all controls until the operation finishes. */
        mSolve->setEnabled(false);
        mProblems->setEnabled(false);

        solveOptimally(mNetwork, mSelected);

        /* Enable controls. */
        mSolve->setEnabled(true);
        mProblems->setEnabled(true);

        requestRepaint();
    }
}

shared_ptr<ProblemHandler> makeDisasterPlanningGUI(GWindow& window) {
    return make_shared<DisasterGUI>(window);
}
