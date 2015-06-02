#ifndef TERMINAL
#define TERMINAL

#include <SFML/Graphics.hpp>
#include <functional>
#include <iostream>
#include <thread>
#include <mutex>
#include "OutputBuffer.hpp"
#include "InputBuffer.hpp"
#include "IOFormat.hpp"
#include "Util.hpp"

namespace cli {
using namespace std;
using namespace sf;
typedef lock_guard <mutex> lock;
mutex mtx;

class Terminal {
public:
    Terminal() {
        mIsAllocated = false;
        mRenderWindow = nullptr;
    }

    Terminal (int w, int h, const string& cap = "Terminal") : Terminal() {
        create (w, h, cap);
    }

    ~Terminal() {
        mClear();
    }

    void create (int w, int h, const string& cap = "Terminal") {
        mClear();
        mRenderWindow = new RenderWindow (VideoMode (w, h), cap);
        //set framerate and keyboards
        mRenderWindow->setFramerateLimit (10);
        mIsAllocated = true;
    }

    void setWindow (RenderWindow& t) {
        mRenderWindow = &t;
    }

    void run (function <void (Terminal&)> runnable) {
        thread th (runnable, ref(*this));
        mRunImpl();
        th.detach();
    }

    void update() {
    }

    void draw() {
        mRenderWindow->clear();
        for (int i = 0; i < mOBuffer.getSize().y; i++) {
            mDrawText.setPosition (sf::Vector2f (10, i * mCharacterSize));
            String str = mCreatePrintableLine (i);
            mDrawText.setString (str);
            if (!isBlank (str))
                mCursorShape.setPosition (sf::Vector2f (10 + mDrawText.getLocalBounds().width, mDrawText.getPosition().y));
            mRenderWindow->draw (mDrawText);
        }

        //mRenderWindow->draw(mCursorShape);
        mRenderWindow->display();
    }

    void handleEvent (sf::Event& e) {
        if (e.type == sf::Event::Closed)
            quit();

        if (e.type == sf::Event::KeyPressed) {
            if (mIBuffer.isInput())
                if (e.key.code == sf::Keyboard::Return)
                    mIBuffer.breakInput();
        }

        if (e.type == sf::Event::TextEntered && mIBuffer.isInput()) {
            if (e.text.unicode == 8) {
                if (mIBuffer.deleteLastChar()) {
                    mOBuffer.getCursor().x--;
                    mOBuffer.atCursor() = ' ';
                }
            } else {
                mIBuffer.addChar (e.text.unicode);
                print (String (e.text.unicode).toAnsiString());
            }
        }
    }

    void print (const string& val) {
        mOBuffer.print (val);
    }

    void print (const Format& format) {
        if (format.mName == "newline") {
            mOBuffer.getCursor().x = 0;
            mOBuffer.getCursor().y++;
        } else if (format.mName == "tab") {
            mOBuffer.print ("    ");
        }
    }

    void input (string& val) {
        mIBuffer.beginInput();
        while (!mIBuffer.checkIfInputEnded())
            this_thread::yield();
        val = mIBuffer.getData();
        mOBuffer.getCursor().x--;
        mOBuffer.atCursor() = ' ';
        mIBuffer.endInput();
    }

    char getch() {
        mIBuffer.beginInput();
        while (!mIBuffer.checkIfKeyWasPressed())
            this_thread::yield();
        char ch = mIBuffer.getData()[0];
        mIBuffer.endInput();
        return ch;
    }

    template <typename T>
    Terminal& operator<< (const T& val) {
        print (val);
        return *this;
    }

    template <typename T>
    Terminal& operator>> (T& val) {
        input (val);
        return *this;
    }

    void setFont (const string& fname) {
        if (!mFont.loadFromFile (fname))
            throw runtime_error ("Couldn't find font '" + fname + "'");
        mDrawText.setFont (mFont);
    }

    void setCharacterSize (int sz) {
        mOBuffer.clear();
        mOBuffer.setSize (mRenderWindow->getSize().x / sz * 2, mRenderWindow->getSize().y / sz);
        //mBuffer.setSize (mRenderWindow->getSize().x / sz, mRenderWindow->getSize().y / sz);
        mDrawText.setCharacterSize (sz);
        mCharacterSize = sz;
        mCursorShape.setFillColor (sf::Color::Red);
        auto bounds = mFont.getGlyph ('L', sz, false).bounds;
        mCursorShape.setSize (Vector2f (bounds.width / 4, bounds.height));
    }

    void cls() {
        mOBuffer.clear();
    }

    void quit() {
        mRenderWindow->close();
    }

private:
    void mRunImpl() {
        lock l (mtx);
        while (mRenderWindow->isOpen()) {
            sf::Event event;
            while (mRenderWindow->pollEvent (event))
                handleEvent (event);
            update();
            draw();
        }
    }

    void mClear() {
        if (mIsAllocated)
            delete mRenderWindow;
    }

    string mCreatePrintableLine (int linePos) {
        String str;
        for (int i = 0; i < mOBuffer.getSize().x; i++)
            str += mOBuffer (i, linePos);
        string s = str.toAnsiString();
        rtrim (s);
        return s;
    }

    bool mIsAllocated;
    Font mFont;
    Text mDrawText;
    RenderWindow* mRenderWindow;

    OutputBuffer mOBuffer;
    InputBuffer mIBuffer;

    int mCharacterSize;
    RectangleShape mCursorShape;
};
}

#endif // TERMINAL

