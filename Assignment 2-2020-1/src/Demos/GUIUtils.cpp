#include "GUIUtils.h"
#include "strlib.h"
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;

namespace {
    /* Multiplier of the line height to use between natural linebreaks. */
    const double kLineSpacing = 1.1;

    /* Multiplier of the line height to use between paragraph breaks. */
    const double kParagraphSpacing = kLineSpacing + 0.35;

    /* Tokenizes a string into individually-renderable units. */
    vector<string> tokenize(const string& text) {
        istringstream input(text);

        string current;
        vector<string> result;

        while (true) {
            int next = input.get();

            /* If we hit EOF, flush the current token and stop. */
            if (next == EOF) {
                if (!current.empty()) result.push_back(current);
                return result;
            }

            /* If we hit a space, flush the current token and start a new one. */
            if (isspace(char(next))) {
                if (!current.empty()) result.push_back(current);
                current = string(1, char(next));
            }
            /* Otherwise, we have a non-space character. Extend the current token, flushing
             * it if it's a whitespace token.
             */
            else {
                if (!current.empty() && isspace(current[0])) {
                    result.push_back(current);
                    current.clear();
                }

                current += char(next);
            }
        }
    }

    /* Attempts to line wrap the text to fit it within the given rectangle. On success, returns true and
     * fills in the outparameters with information about the render. Otherwise, returns false and leaves
     * them unmodified.
     */
    bool lineWrapWithin(const vector<string>& tokens,
                        const GRectangle& bounds,
                        const string& font,
                        vector<tuple<GPoint, string>>& lines,
                        GRectangle& computedBounds) {
        /* Set up a rendering label. */
        GText label("");
        label.setFont(font);

        /* Compute the line height from the font metrics. */
        double lineHeight = label.getFontAscent() + label.getFontDescent();

        /* Track the original starting coordinates. We'll need this for later when we compute
         * the bounding box.
         */
        double renderedWidth = 0;
        double renderedHeight = 0;

        /* Initial position is in the upper-left corner, shifted down by the font ascent
         * (since we draw on the baseline.)
         */
        double x = bounds.getX();
        double y = bounds.getY() + label.getFontAscent();

        vector<tuple<GPoint, string>> computedLines;
        string currLine;

        /* Begin laying out tokens. */
        for (const auto& token: tokens) {
            /* If this is newline, insert a hard line break. */
            if (token == "\n") {
                /* Cache the line generated so far. */
                computedLines.push_back(make_tuple(GPoint(bounds.getX(), y), currLine));
                currLine.clear();

                /* Shift down to the next line. */
                x = bounds.getX();
                y += lineHeight * kParagraphSpacing;
            }
            /* Otherwise, if this is another sort of whitespace token, scoot over as appropriate,
             * but don't adjust the bounding rectangle.
             */
            else if (isspace(token[0])) {
                /* Don't do anything if we're at the front of a line. */
                if (int(x - bounds.getX()) != 0) {
                    label.setText(token);
                    currLine += token;
                    x += label.getWidth();
                }
            }
            /* Otherwise, draw this token and update the global bounding rectangle. */
            else {
                label.setText(token);

                /* If we don't fit, advance to the next line. */
                if (x + label.getWidth() > bounds.getX() + bounds.getWidth()) {
                    /* Write out the line we have so far. */
                    computedLines.push_back(make_tuple(GPoint(bounds.getX(), y), currLine));
                    currLine.clear();

                    x = bounds.getX();
                    y += lineHeight * kLineSpacing;
                }

                currLine += token;

                /* Shift over by the width of what we just added. */
                x += label.getWidth();

                /* Update our bounding rectangle. */
                renderedWidth  = max(renderedWidth, x - bounds.getX());
                renderedHeight = max(y + label.getFontDescent() - bounds.getY(), renderedHeight);

                /* If we're out of bounds, report an error. */
                if (renderedWidth > bounds.getWidth() || renderedHeight > bounds.getHeight()) return false;
            }
        }

        /* Process any remaining tokens. */
        if (!currLine.empty()) {
            computedLines.push_back(make_tuple(GPoint(bounds.getX(), y), currLine));
        }

        lines = computedLines;
        computedBounds = { bounds.getX(), bounds.getY(), renderedWidth, renderedHeight };
        return true;
    }

    /* Given a font string, reduces the size of that font by one unit. */
    string reduceFont(const string& font) {
        /* Find the last dash. */
        auto dash = font.rfind('-');
        if (dash == string::npos) error("Font string [" + font + "] missing a dash?");

        /* Extract the existing font size. */
        int fontSize = stringToInteger(font.substr(dash + 1));

        /* If the font size is one or lower, we can't shrink it. */
        if (fontSize <= 1) {
            error("Cannot reduce font size any further!");
        }

        /* Rebuild the font string with the size reduced. */
        return font.substr(0, dash + 1) + to_string(fontSize - 1);
    }
}

/***************************/
/******* TextRender ********/
/***************************/

shared_ptr<TextRender> TextRender::construct(const string& text,
                                             const GRectangle& bounds,
                                             const string& color,
                                             const string& font) {
    /* Split the text apart into individual tokens. */
    auto tokens = tokenize(text);

    /* Keep trying to get things to fit, shrinking the font until we succeed.
     *
     * TODO: This is a silly O(n)-time algorithm. Use binary search instead?
     */
    shared_ptr<TextRender> result(new TextRender());
    result->mBounds = bounds;
    result->mColor  = color;
    result->mFont   = font;
    while (true) {
        /* See if this works. */
        if (lineWrapWithin(tokens, bounds, result->mFont, result->mLines, result->mComputedBounds)) return result;

        /* Oh fiddlesticks. Back off the font size a bit and try again. */
        result->mFont = reduceFont(result->mFont);
    }
}

void TextRender::draw(GWindow& window) {
    GText label;
    label.setFont(mFont);
    label.setColor(mColor);

    for (const auto& line: mLines) {
        label.setLocation(get<0>(line));
        label.setText(get<1>(line));
        window.draw(label);
    }
}

GRectangle TextRender::bounds() const {
    return mComputedBounds;
}

/***************************/
/****** LegendRender *******/
/***************************/

namespace {
    const double kBulletSize    = 10;
    const double kBulletPadding = 10;
    const double kItemPadding   = 5;
}

shared_ptr<LegendRender> LegendRender::construct(const std::vector<string>& strings,
                                                 const std::vector<string>& colors,
                                                 const GRectangle& bounds,
                                                 const string& textColor,
                                                 const string& font,
                                                 const string& borderColor) {
    /* Validate input. */
    if (strings.size() > colors.size()) error("Not enough colors to draw legend.");

    /* Set up initial fields. */
    shared_ptr<LegendRender> result(new LegendRender());
    result->mBounds = bounds;
    result->mBorderColor = borderColor;
    result->mBulletColors = colors;

    /* Attempt to use the current font size to make everything work. If it does work, great! If not, back off
     * the font size and try again.
     *
     * TODO: This uses a silly O(n)-time algorithm. Switch to binary search?
     */
    string currFont = font;
    while (true) {
        /* Current x and y coordinates, for the purposes of laying out text. */
        double x = bounds.getX();
        double y = bounds.getY() + kItemPadding;

        vector<shared_ptr<TextRender>> entries;

        /* TODO: Short-circuit if we fail early? */
        const double bulletSpacing = kBulletSize + 2 * kBulletPadding;
        double width = 0;
        for (int i = 0; i < strings.size(); i++) {
            /* Construct a text render for this item, tracking where it ended up. */


            /* Give unbounded vertical space. We'll do adjustments for height later on. */
            auto render = TextRender::construct(strings[i], {
                                                    x + bulletSpacing, y,
                                                    bounds.getWidth() - bulletSpacing - kItemPadding,
                                                    numeric_limits<double>::max()
                                                }, textColor, currFont);

            y = render->bounds().getY() + render->bounds().getHeight() + kItemPadding;
            entries.push_back(render);

            width = max(width, render->bounds().getWidth());
        }

        /* Get the net bounding box. We'll force this to be as wide as the space that we're given
         * for consistency's sake.
         */
        GRectangle computedBounds = {
            bounds.getX(), bounds.getY(), bounds.getWidth(), y - bounds.getY()
        };

        if (computedBounds.getHeight() <= bounds.getHeight()) {
            result->mComputedBounds = computedBounds;
            result->mLines = entries;
            return result;
        }

        /* Oops, didn't fit. Shrink the font and try again. */
        currFont = reduceFont(currFont);
    }
}

void LegendRender::draw(GWindow& window) {
    /* First, draw all the text items and their corresponding bullets. */
    for (int i = 0; i < mLines.size(); i++) {
        mLines[i]->draw(window);

        /* Draw the bullet, vertically-centered. */
        window.setColor(mBulletColors[i]);
        window.fillRect(mComputedBounds.getX() + kBulletPadding,
                        mLines[i]->bounds().getY() + mLines[i]->bounds().getHeight() / 2.0 - kBulletSize / 2.0,
                        kBulletSize, kBulletSize);
    }

    /* Draw the overall bounding box. */
    window.setColor(mBorderColor);
    window.drawRect(mComputedBounds);
}

GRectangle LegendRender::bounds() const {
    return mComputedBounds;
}

/***************************/
/***** LineGraphRender *****/
/***************************/

namespace {
    /* Axis line constants. */
    const double kAxisLineWidth = 2;
    const double kSmallTickSize = 8;
    const double kLargeTickSize = 16;

    /* Padding between tick marks and labels. */
    const double kTickPadding = 2;

    /* Plotted line constants. */
    const double kPlottedLineWidth = 5;

    /* Given a list of labels and a label font, returns a bounding rectangle big
     * enough to hold any of those labels.
     */
    GDimension labelDimensionsFor(const vector<string>& labels,
                                  const string& font) {
        GText measurer;
        measurer.setFont(font);

        /* Find the maximum extents across all labels. */
        double maxWidth = 0;
        double maxHeight = 0;

        for (const auto& label: labels) {
            measurer.setLabel(label);

            /* For height, have to factor in ascent and descent. */
            maxHeight = max(maxHeight, measurer.getFontAscent() + measurer.getFontDescent());
            maxWidth = max(maxWidth, measurer.getWidth());
        }

        return { maxWidth, maxHeight };
    }

    /* Given all the X and Y axis labels and their fonts, returns the three control points
     * (origin, x end, y end) that define the axes
     */
    tuple<GPoint, GPoint, GPoint>
    axesFor(const GRectangle& bounds,
            const vector<string>& xLabels,
            const vector<string>& yLabels,
            const string& xLabelFont,
            const string& yLabelFont) {

        double originX = bounds.getX();
        double originY = bounds.getY() + bounds.getHeight();

        double xEnd    = bounds.getX() + bounds.getWidth();
        double yEnd    = bounds.getY();

        /* The labels are drawn centered on all the vertical and horizontal tick marks.
         * That means that half of a label will hang off in each of the X and Y directions on
         * each endpoint of the axis, which means that we need to buffer the full width of
         * one x-axis label on the x-axis and one full y-axis label on the y-axis.
         */
        auto xSize = labelDimensionsFor(xLabels, xLabelFont);
        auto ySize = labelDimensionsFor(yLabels, yLabelFont);
        xEnd    -= xSize.getWidth()  / 2;
        yEnd    += ySize.getHeight() / 2;

        /* Additionally, we need to account for the width of the y-axis labels and the
         * height of the x-axis labels, since they're drawn to the left of and underneath
         * the y-axis, respectively.
         *
         * We also need to consider the possibility that there's a large half-label hanging
         * off from the origin point on either the X or Y axis.
         */
        originX += max(ySize.getWidth(),  xSize.getWidth()  / 2.0);
        originY -= max(xSize.getHeight(), ySize.getHeight() / 2.0);

        /* Don't forget tick mark padding! */
        originX += kTickPadding;
        originY -= kTickPadding;

        /* Finally, we have to buffer half of a tick mark in each direction, since those
         * ticks overdraw the lines.
         */
        originX += kLargeTickSize / 2.0;
        originY -= kLargeTickSize / 2.0;

        return make_tuple(GPoint(originX, originY), GPoint(xEnd, originY), GPoint(originX, yEnd));
    }
}

shared_ptr<LineGraphRender>
LineGraphRender::construct(const vector<vector<GPoint>>& lines,
                           const vector<string>& xLabels,
                           const vector<string>& yLabels,
                           size_t xMinorTicks,
                           size_t yMinorTicks,
                           const GRectangle& bounds,
                           const string& xLabelFont,
                           const string& yLabelFont,
                           const vector<string>& lineColors,
                           const string& axisColor) {
    /* Boundary case: We can't draw a line graph if we don't have at least two ticks
     * in each of the X and Y directions.
     */
    if (xLabels.size() < 2 || yLabels.size() < 2) error("Insufficiently many ticks.");

    /* Boundary case: We need one color per line. */
    if (lineColors.size() < lines.size()) error("Too few line colors (have " + to_string(lineColors.size()) + ", need " + to_string(lines.size()) + ").");

    shared_ptr<LineGraphRender> result(new LineGraphRender());

    /* Copy over basic information. */
    result->mXLabelFont  = xLabelFont;
    result->mYLabelFont  = yLabelFont;
    result->mXLabels     = xLabels;
    result->mYLabels     = yLabels;
    result->mAxisColor   = axisColor;
    result->mLineColors  = lineColors;
    result->mXMinorTicks = xMinorTicks;
    result->mYMinorTicks = yMinorTicks;
    result->mBounds      = bounds;

    /* First order of business: figure out how much space we need for the labels
     * on the axes. This is necessrary in order for us to figure out where the
     * axes go.
     */
    tie(result->mOrigin, result->mXEnd, result->mYEnd) =
            axesFor(bounds, xLabels, yLabels, xLabelFont, yLabelFont);

    /* Now that we have these coordinate points, we can remap the lines so that they use
     * the line graph coordinate space instead of the virtual [0, 1] coordinate space.
     */
    double baseX  = result->mOrigin.getX();
    double baseY  = result->mOrigin.getY();
    double width  = result->mXEnd.getX()   - result->mOrigin.getX();
    double height = result->mOrigin.getY() - result->mYEnd.getY();
    for (const auto& line: lines) {
        vector<GPoint> physical;
        for (const auto& pt: line) {
            physical.push_back({ baseX + width * pt.getX(), baseY - height * pt.getY() });
        }
        result->mLines.push_back(physical);
    }

    return result;
}

void LineGraphRender::drawYAxis(GWindow& window) const {
    /* Draw the axis line. */
    GLine axisLine(mOrigin, mYEnd);
    axisLine.setColor(mAxisColor);
    axisLine.setLineWidth(kAxisLineWidth);
    window.draw(&axisLine);

    /* Draw tick marks. */
    GText tickLabel("");
    tickLabel.setColor(mAxisColor);
    tickLabel.setFont(mYLabelFont);

    /* Useful constants. */
    const double height  = mOrigin.getY() - mYEnd.getY();
    const double spacing = height / (mYLabels.size() - 1);

    for (size_t i = 0; i < mYLabels.size(); i++) {
        /* Major tick mark. */
        double y = mOrigin.getY() - i * spacing;
        double x = mOrigin.getX();
        window.setColor(mAxisColor);
        window.drawLine(x - kLargeTickSize / 2.0, y, x + kLargeTickSize / 2.0, y);

        /* Tick mark label. */
        tickLabel.setLabel(mYLabels[i]);
        window.draw(&tickLabel, x - kLargeTickSize / 2.0 - kTickPadding - tickLabel.getWidth(), y + tickLabel.getFontDescent());

        /* Minor tick marks. */
        if (i + 1 != mYLabels.size()) {
            for (size_t tick = 0; tick < mYMinorTicks; tick++) {
                /* Curious where this comes from? We're going to pretend that we're drawing minor
                 * tick marks on top of the major tick marks to get the spacing right.
                 */
                double minorY = y - (tick + 1) * (spacing / (mYMinorTicks + 1));
                window.drawLine(x - kSmallTickSize / 2.0, minorY, x + kSmallTickSize / 2.0, minorY);
            }
        }
    }
}

void LineGraphRender::drawXAxis(GWindow& window) const {
    /* Draw the axis line. */
    GLine axisLine(mOrigin, mXEnd);
    axisLine.setColor(mAxisColor);
    axisLine.setLineWidth(kAxisLineWidth);
    window.draw(&axisLine);

    /* Draw tick marks. */
    GText tickLabel("");
    tickLabel.setColor(mAxisColor);
    tickLabel.setFont(mXLabelFont);

    /* Useful constants. */
    const double width  = mXEnd.getX() - mOrigin.getX();
    const double spacing = width / (mXLabels.size() - 1);

    for (size_t i = 0; i < mXLabels.size(); i++) {
        double x = mOrigin.getX() + i * spacing;
        double y = mOrigin.getY();

        window.setColor(mAxisColor);
        window.drawLine(x, y - kLargeTickSize / 2.0, x, y + kLargeTickSize / 2.0);

        /* Tick mark label. */
        tickLabel.setLabel(mXLabels[i]);
        window.draw(&tickLabel, x - tickLabel.getWidth() / 2.0, y + kLargeTickSize / 2.0 + kTickPadding + tickLabel.getFontAscent());

        /* Minor tick marks. */
        if (i + 1 != mXLabels.size()) {
            for (size_t tick = 0; tick < mXMinorTicks; tick++) {
                /* Curious where this comes from? We're going to pretend that we're drawing minor
                 * tick marks on top of the major tick marks to get the spacing right.
                 */
                double minorX = x + (tick + 1) * (spacing / (mXMinorTicks + 1));
                window.drawLine(minorX, y - kSmallTickSize / 2.0, minorX, y + kSmallTickSize / 2.0);
            }
        }
    }
}

void LineGraphRender::drawLines(GWindow& window) const {
    /* Draw lines from back to front. */
    for (size_t j = mLines.size(); j > 0; --j) {
        GLine line(0, 0, 0, 0);
        line.setLineWidth(kPlottedLineWidth);
        line.setColor(mLineColors[j - 1]);

        for (size_t i = 0; i + 1 < mLines[j - 1].size(); i++) {
            auto src = mLines[j - 1][i];
            auto dst = mLines[j - 1][i + 1];
            line.setStartPoint(src.getX(), src.getY());
            line.setEndPoint(dst.getX(), dst.getY());
            window.draw(&line);
        }
    }
}

GRectangle LineGraphRender::bounds() const {
    return mBounds;
}

void LineGraphRender::draw(GWindow& window) {
    drawXAxis(window);
    drawYAxis(window);
    drawLines(window);
}




/***************************/
/*******  Helpers   ********/
/***************************/

void clearDisplay(GWindow& window, const std::string& backgroundColor) {
    window.setColor(backgroundColor);
    window.fillRect(0, 0, window.getCanvasWidth(), window.getCanvasHeight());
}

/* The math behind this function is shamelessly lifted (with love) from Wikipedia:
 *
 *   https://en.wikipedia.org/wiki/Mollweide_projection#Mathematical_formulation
 *
 * The inverse formula is way easier than the forward formula. :-)
 *
 * The output of this function is a pair of real numbers in the range [-2, +2] x [-1, 1],
 * which then needs to be scaled and translated based on the window size.
 */
tuple<double, double> mollweideProjectionOf(double latitude, double longitude,
                                            double longitudeOffset, double latitudeOffset) {
    /* Adjust longitude to fit map. */
    longitude -= longitudeOffset;
    if (longitude < -180) longitude += 360;
    if (longitude > 180)  longitude -= 360;

    latitude -= latitudeOffset;
    if (latitude < -90)   latitude += 180;
    if (latitude >  90)   latitude  -= 180;

    /* Convert from degrees (what we get back from USGS) to radians. */
    longitude *= M_PI / 180;
    latitude  *= M_PI / 180;

    /* There isn't a closed-form solution to work out the coordinates, so we'll
     * use Newton's method to try to get close to one. Thanks, calculus!
     */
    static const size_t kNumIterations = 100;
    double theta = latitude;
    for (size_t i = 0; i < kNumIterations; i++) {
        theta = theta - (2 * theta + sin(2 * theta) - M_PI * sin(latitude)) / (2 + 2 * cos(2 * theta));
    }

    /* Armed with theta, we can work out the x and y coordinates. */
    double x = 2 * cos(theta) * longitude / M_PI;
    double y = sin(theta);

    return make_tuple(x, y);
}

Temporary<GSlider> makeYearSlider(GWindow& window, int startYear, int endYear) {
    Temporary<GSlider> result(new GSlider(startYear, endYear, startYear),
                              window,
                              "SOUTH");
    result->setPaintLabels(true);
    result->setPaintTicks(true);
    result->setSnapToTicks(true);
    result->setMajorTickSpacing(10);
    result->setMinorTickSpacing(1);
    result->setBounds(0, 0, window.getCanvasWidth(), result->getHeight());
    return result;
}
