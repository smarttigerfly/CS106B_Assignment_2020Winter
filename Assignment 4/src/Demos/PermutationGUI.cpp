#include "ProblemHandler.h"
#include "TemporaryComponent.h"
#include "GColorConsole.h"
#include "Permutations.h"
#include "GUIMain.h"
#include "set.h"
#include "ginteractors.h"
#include "tokenscanner.h"
#include <cctype>
using namespace std;

namespace {
    class PermutationGUI: public ProblemHandler {
    public:
        PermutationGUI(GWindow& window);

        void actionPerformed(GObservable* source) override;

    private:
        Temporary<GColorConsole> mConsole;
        Temporary<GLabel>        mDescription;
        Temporary<GTextField>    mInput;
        Temporary<GButton>       mGo;
        Temporary<GButton>       mClear;

        void showPermutationsOf(const string& str);
    };
}

PermutationGUI::PermutationGUI(GWindow& window) {
    mConsole     = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");
    mDescription = Temporary<GLabel>(new GLabel("Enter string: "), window, "SOUTH");
    mInput       = Temporary<GTextField>(new GTextField(""), window, "SOUTH");
    mGo          = Temporary<GButton>(new GButton("Go!"), window, "SOUTH");
    mClear       = Temporary<GButton>(new GButton("Clear"), window, "WEST");
}

void PermutationGUI::actionPerformed(GObservable* source) {
    if (source == mGo || source == mInput) {
        setDemoOptionsEnabled(false);
        mInput->setEnabled(false);
        mGo->setEnabled(false);

        mConsole->doWithStyle("#808080", GColorConsole::ITALIC, [&] {
            *mConsole << "Computing Permutations... " << flush;
        });
        showPermutationsOf(mInput->getText());

        mGo->setEnabled(true);
        mInput->setEnabled(true);
        setDemoOptionsEnabled(true);

    } else if (source == mClear) {
        mConsole->clear();
    }
}

void PermutationGUI::showPermutationsOf(const string& str) {
    auto permutations = permutationsOf(str);
    Set<string> sortedPermutations;

    /* Just to be nice, let's print the permutations in sorted order. The
     * regular Set<string> type represents a sorted set.
     */
    for (string permutation: permutations) {
        sortedPermutations += permutation;
    }

    mConsole->doWithStyle("#000080", GColorConsole::BOLD_ITALIC, [&] {
        *mConsole << "Done!" << endl;
    });
    mConsole->doWithStyle("#000000", GColorConsole::NORMAL, [&] {
        for (string permutation: sortedPermutations) {
            *mConsole << "  \"" << permutation << '"' << endl;
        }
    });
    *mConsole << endl;
}

shared_ptr<ProblemHandler> makePermutationsGUI(GWindow& window) {
    return make_shared<PermutationGUI>(window);
}
