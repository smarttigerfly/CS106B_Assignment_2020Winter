#include "ExploreArrays.h"
#include "GUI/ProblemHandler.h"
#include "GUI/TemporaryComponent.h"
#include "GUI/GUIUtils.h"
#include "ginteractors.h"
#include "random.h"
using namespace std;

namespace {
    /* Message for the user. */
    const string kMessage = "Make sure you're running your code in debug mode before "
                            "hitting the 'Go!' button below. Otherwise, you won't be "
                            "able to do the array exploration you need to do.";
    const string kBackgroundColor = "white";
    const string kMessageColor = "#400080";
    const string kMessageFont  = "Serif-ITALIC-24";

    /* Utility function that fills a large stack frame with random values. This
     * is used to make the values of locals in future calls look even more
     * random than they might otherwise be. :-)
     */
    const int kLargeSize = 1024;
    int floodTheStack() {
        char buffer[kLargeSize];
        for (int i = 0; i < kLargeSize; i++) {
            buffer[i] = randomInteger(0, 255);
        }

        int result = 0;
        for (int i = 0; i < kLargeSize; i++) {
            result += buffer[i];
        }

        return result;
    }

    class ArrayExploreGUI: public ProblemHandler {
    public:
        ArrayExploreGUI(GWindow& window);

        void actionPerformed(GObservable* source) override;

    protected:
        void repaint(GWindow& window) override;

    private:
        Temporary<GButton> mOutOfBounds,
                           mUninitializedPointer,
                           mDeallocatedPointer;
    };

    ArrayExploreGUI::ArrayExploreGUI(GWindow& window) {
        mOutOfBounds          = make_temporary<GButton>(window, "SOUTH", "Go!");
    }

    void ArrayExploreGUI::actionPerformed(GObservable* source) {
        (void) floodTheStack();
        if (source == mOutOfBounds) {
            exploreArrays();
        }
    }

    void ArrayExploreGUI::repaint(GWindow& window) {
        clearDisplay(window, kBackgroundColor);
        try {
            auto render = TextRender::construct(kMessage, {
                                                    0, 0,
                                                    window.getCanvasWidth(), window.getCanvasHeight()
                                                }, kMessageColor, kMessageFont);
            render->alignCenterVertically();
            render->alignCenterHorizontally();
            render->draw(window);
        } catch (...) {
            /* Oops, no space !*/
        }
    }
}

shared_ptr<ProblemHandler> makeExploreArraysGUI(GWindow& window) {
    return make_shared<ArrayExploreGUI>(window);
}
