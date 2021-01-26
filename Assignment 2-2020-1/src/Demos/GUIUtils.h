#ifndef GUIUtils_Included
#define GUIUtils_Included

#include "TemporaryComponent.h"
#include "gobjects.h"
#include "ginteractors.h"
#include "gwindow.h"
#include <string>
#include <vector>
#include <tuple>
#include <memory>

/* Type: TextRender
 *
 * Object capable of rendering nicely-formatted text on the screen.
 */
class TextRender {
public:
    /* Bounding rectangle for the text render. */
    GRectangle bounds() const;

    /* Color for this text render. */
    std::string color() const;

    /* Draws this text render in the specified window. */
    void draw(GWindow& window);

    /* Lays out a string to fit within the specified rectangle. The text might not fill up the
     * entirety of that rectangle if it isn't long enough.
     *
     * This function will attempt to respect the font size specified in the font string, but may
     * have to shrink the font size to get things to fit.
     */
    static std::shared_ptr<TextRender> construct(const std::string& text,
                                                 const GRectangle& bounds,
                                                 const std::string& color,
                                                 const std::string& font);

private:
    TextRender() = default;
    GRectangle mBounds;
    GRectangle mComputedBounds;
    std::string mFont;
    std::vector<std::tuple<GPoint, std::string>> mLines;
    std::string mColor;
};

/* Type: LegendRender
 *
 * Object capable of rendering a nicely-formatted chart legend on the screen.
 */
class LegendRender {
public:
    GRectangle bounds() const;

    void draw(GWindow& window);

    /* Draws a legend for a chart, returning the bounding box for that legend.
     *
     * The input lists of strings may be shorter than the input list of colors, but must
     * not be longer.
     */
    static std::shared_ptr<LegendRender> construct(const std::vector<std::string>& strings,
                                                   const std::vector<std::string>& colors,
                                                   const GRectangle& bounds,
                                                   const std::string& textColor,
                                                   const std::string& font,
                                                   const std::string& borderColor);

private:
    LegendRender() = default;
    GRectangle mBounds;
    GRectangle mComputedBounds;
    std::vector<std::shared_ptr<TextRender>> mLines;
    std::vector<std::string> mBulletColors;
    std::string mBorderColor;
};

/* Type: LineGraphRender
 *
 * Object capable of rendering a nicely-formatted line graph on the screen.
 */
class LineGraphRender {
public:
    GRectangle bounds() const;

    void draw(GWindow& window);

    /* Assembles the line graph renderer.
     *
     * The X and Y axes will assign labels to major tick marks. You can leave those labels
     * blank if you'd like, but the number of labels will be inferred from the lengths of
     * the label lists.
     *
     * You can have minor tick marks on each of the axes if you'd like by setting the
     * major tick size to a number greater than zero.
     *
     * The data points provided should be normalized to a virtual space where (0, 0) is the
     * origin and (1, 1) is the upper-right corner of the line graph. The lines don't have
     * to have the same number of points or pass perfectly over the tick marks on the X
     * axis.
     */
    static std::shared_ptr<LineGraphRender> construct(const std::vector<std::vector<GPoint>>& lines,
                                                      const std::vector<std::string>& xLabels,
                                                      const std::vector<std::string>& yLabels,
                                                      std::size_t xMinorTicks,
                                                      std::size_t yMinorTicks,
                                                      const GRectangle& bounds,
                                                      const std::string& xLabelFont,
                                                      const std::string& yLabelFont,
                                                      const std::vector<std::string>& lineColors,
                                                      const std::string& axisColors);

private:
    LineGraphRender() = default;

    /* Axis start and end points. */
    GPoint mOrigin, mXEnd, mYEnd;

    /* Line labels and fonts. */
    std::vector<std::string> mXLabels, mYLabels;
    std::string mXLabelFont, mYLabelFont;

    /* Axis parameters. */
    std::string mAxisColor;
    std::size_t mXMinorTicks, mYMinorTicks;

    /* The lines, in real rather than virtual space. */
    std::vector<std::vector<GPoint>> mLines;
    std::vector<std::string> mLineColors;

    GRectangle mBounds;

    /* Draws each component. */
    void drawXAxis(GWindow& window) const;
    void drawYAxis(GWindow& window) const;
    void drawLines(GWindow& window) const;
};

/* Clears the display, resetting it to a specific color. */
void clearDisplay(GWindow& window, const std::string& backgroundColor);

/* Given a coordinate on the Earth's surface, along with a centering coordinate, returns
 * the (x, y) coordinates at which to display that coordinate in a Mollweide projection.
 * The returned coordinates are in logical space ([-2, +2] x [-1, +1]) and will need to
 * be converted to screen space before being displayed.
 *
 * The input longitudes and latitudes should be in degrees, not radians.
 */
std::tuple<double, double> mollweideProjectionOf(double latitude, double longitude,
                                                 double centerLongitude = 0.0,
                                                 double centerLatitude  = 0.0);

/* Creates a slider bar in the SOUTH console that slides over a given year range. */
Temporary<GSlider> makeYearSlider(GWindow& window, int startYear, int endYear);

#endif
