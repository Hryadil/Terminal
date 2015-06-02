#ifndef INPUTBUFFER_HPP
#define INPUTBUFFER_HPP

#include <vector>
#include <iostream>
#include <iomanip>
#include <functional>

namespace cli {
using namespace std;
using namespace sf;

class InputBuffer {
public:
    InputBuffer() {
        endInput();
    }

    void beginInput() {
        mUserIsEnteringText = true;
    }

    bool isInput() {
        return mUserIsEnteringText;
    }

    bool checkIfInputEnded() {
        return mUserWantsToEndInput;
    }

    void endInput() {
        mUserIsEnteringText = false;
        mUserWantsToEndInput = false;
        mInputData = "";
    }

    void breakInput() {
        mUserWantsToEndInput = true;
    }

    bool deleteLastChar() {
        if (mInputData.getSize() > 0) {
            mInputData.erase (mInputData.getSize() - 1);
            return true;
        } else return false;
    }

    void addChar (Uint32 ch) {
        mInputData += ch;
    }

    bool checkIfKeyWasPressed() {
        return mInputData.getSize() == 1;
    }

    String getData() {
        cout << mInputData.toAnsiString() << "\n";
        //delete last enter
        if (mInputData.getSize() > 1)
            mInputData.erase (mInputData.getSize() - 1);
        return mInputData;
    }

private:
    String mInputData;
    bool mUserIsEnteringText;
    bool mUserWantsToEndInput;
};
}

#endif // INPUTBUFFER_HPP
