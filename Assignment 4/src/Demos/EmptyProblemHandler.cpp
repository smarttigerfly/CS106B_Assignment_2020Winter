#include "Demos/EmptyProblemHandler.h"
#include "Demos/GUIUtils.h"
#include "gobjects.h"
#include <string>
using namespace std;

namespace {
    /* Graphics Constants. */
    const string kFont = "Serif-BOLDITALIC-36";
    const string kBackgroundColor = "#400040";
    const string kFontColor       = "#800080";

    const string kToDisplay = "Choose which demo to run.";
}

void EmptyProblemHandler::repaint(GWindow& window) {
    /* Clear the window. */
    clearDisplay(window, kBackgroundColor);

    /* Draw some text! */
    auto text = TextRender::construct(kToDisplay, {
                                          0, 0, window.getCanvasWidth(), window.getCanvasHeight()
                                      }, kFontColor, kFont);
    text->alignCenterVertically();
    text->alignCenterHorizontally();
    text->draw(window);
}
