#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vector>
#include <iostream>
#include <iomanip>
#include <functional>

namespace cli {
using namespace std;
using namespace sf;

class OutputBuffer {
public:
    typedef Uint32 Value;
    enum { ToFront, ToBack };

    Value& operator() (int x, int y) {
        return mData.at (y).at (x);
    }

    Value& atCursor() {
        return operator() (getCursor().x, getCursor().y);
    }

    void clear() {
        forEach ([] (int x, int y, Value& val) {
            val = ' ';
        });
        mCursor.x = mCursor.y = 0;
    }

    Vector2i getSize() {
        return mData.size() ? Vector2i (mData[0].size(), mData.size()) : Vector2i (0, 0);
    }

    void setSize (int x_, int y_) {
        /*TODO: check if sz if greater or less*/
        clear();
        for (int y = 0; y < y_; y++)
            addLine (x_, ToBack);
    }

    void setSize (const Vector2i& sz) {
        setSize (sz.x, sz.y);
    }

    void forEach (function <void (int, int, Value&)> callback) {
        for (int y = 0; y < getSize().y; y++)
            for (int x = 0; x < getSize().x; x++)
                callback (x, y, operator() (x, y));
    }

    void print (const String& str) {
        Vector2i offset;
        offset.x = mCursor.x + str.getSize();

        if (offset.x == 0)
            return;

        //if the string is not over the width
        if (offset.x < getSize().x) {
            mPrintN (str, str.getSize());
        // else we must go to next line
        } else {
            int pos = str.getSize() - (offset.x - getSize().x);
            mPrintN (str, pos);
            mCursor.y++;
            mCursor.x = 0;
            return print (str.toAnsiString().substr(pos).c_str());
        }
    }

    void addLine (int sz, int where) {
        vector <Value> temp;
        for (int i = 0; i < sz; i++)
            temp.push_back (' ');

        if (where == ToBack)
            mData.push_back (temp);
        else
            mData.insert (mData.begin(), temp);
    }

    void deleteLine (int y, int from) {
        operator() (0, y);
        mData.erase (mData.begin() + y);
    }

    Vector2i& getCursor() {
        return mCursor;
    }

private:
    void mPrintN (const String& str, int num) {
        for (int i = 0; i < num; i++) {
            operator() (mCursor.x, mCursor.y) = str[i];
            mCursor.x++;
        }
    }

    Vector2i mCursor;
    vector <vector <Value>> mData;
};
}

std::ostream& operator<< (std::ostream& os, cli::OutputBuffer& buff) {
    os << "Size (" << buff.getSize().x << ", " << buff.getSize().y << ")\n";
    buff.forEach ([&] (int x, int y, cli::OutputBuffer::Value& val) {
        //os << "  (" << x << ", " << y << ")\n";
        os << std::setw (3) << val;
        if (buff.getSize().x - 1 == x)
            os << std::endl;
    });
    os << "\n";
    return os;
}

#endif // BUFFER_HPP
