// SPDX-License-Identifier: BSL-1.0

#include "catchwrapper.h"

#include "Tui/ZRoot.h"
#include "Tui/ZTerminal.h"
#include "Tui/ZTest.h"
#include "Tui/ZTextMetrics.h"
#include "Tui/ZWindow.h"

#include "clipboard.h"
#include "document.h"
#include "file.h"
#include "eventrecorder.h"

class DocumentTestHelper {
public:
    Document &getDoc(File *f) {
        return f->_doc;
    }
};

class Rootabgeletiet : public Tui::ZRoot {
public:
    QObject *facet(const QMetaObject &metaObject) const override {
        if (metaObject.className()  == Clipboard::staticMetaObject.className()) {
            return &_clipboard;
        } else {
            return ZRoot::facet(metaObject);
        }
    }

private:
    mutable Clipboard _clipboard;
};


TEST_CASE("file") {
    Tui::ZTerminal::OffScreen of(80, 24);
    Tui::ZTerminal terminal(of);
    Rootabgeletiet root;
    Tui::ZWindow *w = new Tui::ZWindow(&root);
    terminal.setMainWidget(&root);
    w->setGeometry({0, 0, 80, 24});

    File *f = new File(w);
    f->setFocus();
    f->setGeometry({0, 0, 80, 24});

    DocumentTestHelper t;
    Document &doc = t.getDoc(f);
    TextCursor cursor{&doc, f, [&terminal,&doc](int line, bool wrappingAllowed) { Tui::ZTextLayout lay(terminal.textMetrics(), doc.getLines()[line]); lay.doLayout(65000); return lay; }};

    //OHNE TEXT
    CHECK(f->getCursorPosition() == QPoint{0,0});
    CHECK(f->isSelect() == false);

    auto testCase = GENERATE(as<Qt::KeyboardModifiers>{}, Qt::KeyboardModifier::NoModifier, Qt::KeyboardModifier::AltModifier, Qt::KeyboardModifier::MetaModifier,
                             Qt::KeyboardModifier::ShiftModifier, Qt::KeyboardModifier::KeypadModifier, Qt::KeyboardModifier::ControlModifier,
                             Qt::KeyboardModifier::GroupSwitchModifier, Qt::KeyboardModifier::KeyboardModifierMask,
                             Qt::KeyboardModifier::ShiftModifier | Qt::KeyboardModifier::ControlModifier
                             //Qt::KeyboardModifier::ShiftModifier | Qt::KeyboardModifier::AltModifier //TODO: multi select
                             //Qt::KeyboardModifier::ShiftModifier | Qt::KeyboardModifier::AltModifier | Qt::KeyboardModifier::ControlModifier
                             );
    CAPTURE(testCase);

    SECTION("key-left") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Left, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
    }
    SECTION("key-right") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Right, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
    }
    SECTION("key-up") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Up, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
    }
    SECTION("key-down") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Down, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
    }
    SECTION("key-home") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
    }
    SECTION("key-end") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_End, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
    }
    SECTION("key-page-up") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_PageUp, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
    }
    SECTION("key-page-down") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_PageDown, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
    }
    SECTION("key-enter") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Enter, testCase);
        if (Qt::KeyboardModifier::NoModifier == testCase || Qt::KeyboardModifier::KeypadModifier == testCase) {
            CHECK(f->getCursorPosition() == QPoint{0,1});
        } else {
            CHECK(f->getCursorPosition() == QPoint{0,0});
        }
    }
    SECTION("key-tab-space") {
        f->setTabOption(false);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Tab, testCase);
        if (Qt::KeyboardModifier::NoModifier == testCase) {
            CHECK(f->getCursorPosition() == QPoint{8,0});
            CHECK(doc.getLines()[0] == "        ");
        } else {
            CHECK(f->getCursorPosition() == QPoint{0,0});
        }
    }
    SECTION("key-tab") {
        f->setTabOption(true);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Tab, testCase);
        if (Qt::KeyboardModifier::NoModifier == testCase) {
            CHECK(f->getCursorPosition() == QPoint{1,0});
            CHECK(doc.getLines()[0] == "\t");
        } else {
            CHECK(f->getCursorPosition() == QPoint{0,0});
        }
    }
    SECTION("key-insert") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Insert, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
    }
    SECTION("key-delete") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Delete, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
    }
    SECTION("key-backspace") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Backspace, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
    }
    SECTION("key-escape") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Escape, testCase);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
    }
}

TEST_CASE("actions") {
    Tui::ZTerminal::OffScreen of(80, 24);
    Tui::ZTerminal terminal(of);
    Rootabgeletiet root;
    Tui::ZWindow *w = new Tui::ZWindow(&root);
    terminal.setMainWidget(&root);
    w->setGeometry({0, 0, 80, 24});

    File *f = new File(w);

    DocumentTestHelper t;

    Document &doc = t.getDoc(f);
    TextCursor cursor{&doc, f, [&terminal,&doc](int line, bool wrappingAllowed) { Tui::ZTextLayout lay(terminal.textMetrics(), doc.getLines()[line]); lay.doLayout(65000); return lay; }};

    f->setFocus();
    f->setGeometry({0, 0, 80, 24});

    CHECK(f->getCursorPosition() == QPoint{0,0});
    Tui::ZTest::sendText(&terminal, "    text", Qt::KeyboardModifier::NoModifier);

    Tui::ZTest::sendKey(&terminal, Qt::Key_Enter, Qt::KeyboardModifier::NoModifier);
    Tui::ZTest::sendText(&terminal, "    new1", Qt::KeyboardModifier::NoModifier);
    CHECK(doc.getLines().size() == 2);

    SECTION("acv") {
        Tui::ZTest::sendText(&terminal, "a", Qt::KeyboardModifier::ControlModifier);
        Tui::ZTest::sendText(&terminal, "c", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{8,1});
        CHECK(t.getDoc(f).getLines().size() == 2);
        Tui::ZTest::sendText(&terminal, "v", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{8,1});
        CHECK(t.getDoc(f).getLines().size() == 2);
    }
    SECTION("acv") {
        Tui::ZTest::sendText(&terminal, "a", Qt::KeyboardModifier::ControlModifier);
        Tui::ZTest::sendText(&terminal, "c", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{8,1});
        CHECK(t.getDoc(f).getLines().size() == 2);
        Tui::ZTest::sendText(&terminal, "v", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{8,1});
        CHECK(t.getDoc(f).getLines().size() == 2);
    }
    SECTION("ac-right-v") {
        Tui::ZTest::sendText(&terminal, "a", Qt::KeyboardModifier::ControlModifier);
        Tui::ZTest::sendText(&terminal, "c", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{8,1});
        CHECK(t.getDoc(f).getLines().size() == 2);
        CHECK(root.findFacet<Clipboard>()->getClipboard() == "    text\n    new1");
        Tui::ZTest::sendKey(&terminal, Tui::Key_Right, Qt::KeyboardModifier::NoModifier);
        Tui::ZTest::sendText(&terminal, "v", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{8,2});
        CHECK(t.getDoc(f).getLines().size() == 3);
    }
    SECTION("axv") {
        Tui::ZTest::sendText(&terminal, "a", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{8,1});
        CHECK(f->isSelect() == true);
        Tui::ZTest::sendText(&terminal, "x", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == false);
        Tui::ZTest::sendText(&terminal, "v", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->isSelect() == false);
        CHECK(f->getCursorPosition() == QPoint{8,1});
    }
    SECTION("cv-newline") {
        CHECK(f->getCursorPosition() == QPoint{8,1});
        Tui::ZTest::sendKey(&terminal, Tui::Key_Up, Qt::KeyboardModifier::NoModifier);
        CHECK(f->getCursorPosition() == QPoint{8,0});
        Tui::ZTest::sendKey(&terminal, Tui::Key_Right, Qt::KeyboardModifier::ShiftModifier);
        CHECK(f->getCursorPosition() == QPoint{0,1});
        Tui::ZTest::sendText(&terminal, "c", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->isSelect() == true);
        CHECK(root.findFacet<Clipboard>()->getClipboard() == "\n");

        CHECK(t.getDoc(f).getLines().size() == 2);
        Tui::ZTest::sendText(&terminal, "v", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{0,1});
        CHECK(t.getDoc(f).getLines().size() == 2);
        CHECK(f->isSelect() == false);

        Tui::ZTest::sendText(&terminal, "v", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{0,2});
        CHECK(t.getDoc(f).getLines().size() == 3);
        CHECK(doc.getLines()[1] == "");
    }

    SECTION("sort") {
        // Alt + Shift + s sort selected lines
        //CAPTURE(doc.getLines()[1]);
        CHECK(doc.getLines().size() == 2);
        CHECK(doc.getLines()[1] == "    new1");
        Tui::ZTest::sendText(&terminal, "a", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->isSelect() == true);
        Tui::ZTest::sendText(&terminal, "S", Qt::KeyboardModifier::AltModifier | Qt::KeyboardModifier::ShiftModifier);
        CHECK(doc.getLines()[0] == "    new1");
        CHECK(doc.getLines().size() == 2);
        CHECK(f->isSelect() == true);
    }
    //delete
    SECTION("key-delete") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::NoModifier);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Delete, Qt::KeyboardModifier::NoModifier);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Delete, Qt::KeyboardModifier::NoModifier);
        CHECK(doc.getLines()[1] == "  new1");
        CHECK(f->getCursorPosition() == QPoint{0,1});
    }
    SECTION("key-delete-remove-line") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Up, Qt::KeyboardModifier::NoModifier);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Delete, Qt::KeyboardModifier::NoModifier);
        CHECK(doc.getLines()[0] == "    text    new1");
        CHECK(f->getCursorPosition() == QPoint{8,0});
    }
    SECTION("ctrl+key-delete") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::NoModifier);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Delete, Qt::KeyboardModifier::ControlModifier);
        CHECK(doc.getLines()[1] == "");
        CHECK(f->getCursorPosition() == QPoint{0,1});
    }
    SECTION("shift+key-delete") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::NoModifier);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Delete, Qt::KeyboardModifier::ShiftModifier);
        CHECK(doc.getLines()[1] == "    new1");
        CHECK(f->getCursorPosition() == QPoint{0,1});
    }
    SECTION("alt+key-delete") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::NoModifier);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Delete, Qt::KeyboardModifier::AltModifier);
        CHECK(doc.getLines()[1] == "    new1");
        CHECK(f->getCursorPosition() == QPoint{0,1});
    }
    //backspace
    SECTION("key-backspace") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Backspace, Qt::KeyboardModifier::NoModifier);
        CHECK(f->getCursorPosition() == QPoint{7,1});
        CHECK(doc.getLines()[1] == "    new");
    }
    SECTION("ctrl+key-backspace") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Backspace, Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{4,1});
        CHECK(doc.getLines()[1] == "    ");
    }
    SECTION("alt+key-backspace") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Backspace, Qt::KeyboardModifier::AltModifier);
        CHECK(f->getCursorPosition() == QPoint{8,1});
        CHECK(doc.getLines()[1] == "    new1");
    }
    SECTION("key-left") {
        for(int i = 0; i <= 8; i++) {
            CHECK(f->getCursorPosition() == QPoint{8 - i, 1});
            Tui::ZTest::sendKey(&terminal, Qt::Key_Left, Qt::KeyboardModifier::NoModifier);
        }
        CHECK(f->getCursorPosition() == QPoint{8, 0});
    }

    //left
    SECTION("crl+key-left") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Left, Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{4, 1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Left, Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{0, 1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Left, Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{8, 0});
    }

    SECTION("crl+shift+key-left") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Left, Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::ShiftModifier);
        CHECK(f->getCursorPosition() == QPoint{4, 1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Left, Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::ShiftModifier);
        CHECK(f->getCursorPosition() == QPoint{0, 1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Left, Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::ShiftModifier);
        CHECK(f->getCursorPosition() == QPoint{8, 0});
    }

    //right
    SECTION("crl+key-right") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::ControlModifier);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Right, Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{8, 0});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Right, Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{0, 1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Right, Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{8, 1});
    }
    SECTION("crl+key-right") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::ControlModifier);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Right, Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::ShiftModifier);
        CHECK(f->getCursorPosition() == QPoint{8, 0});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Right, Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::ShiftModifier);
        CHECK(f->getCursorPosition() == QPoint{0, 1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Right, Qt::KeyboardModifier::ControlModifier | Qt::KeyboardModifier::ShiftModifier);
        CHECK(f->getCursorPosition() == QPoint{8, 1});
    }


    SECTION("scroll") {
        auto testCase = GENERATE(Tui::ZTextOption::WrapMode::NoWrap, Tui::ZTextOption::WrapMode::WordWrap, Tui::ZTextOption::WrapMode::WrapAnywhere);
        f->setWrapOption(testCase);
        CHECK(f->getScrollPosition() == QPoint{0,0});

        //TODO: #193 scrollup with Crl+Up and wraped lines.
        //Tui::ZTest::sendText(&terminal, QString("a").repeated(100), Qt::KeyboardModifier::NoModifier);

        for (int i = 0; i < 48; i++) {
            Tui::ZTest::sendKey(&terminal, Qt::Key_Enter, Qt::KeyboardModifier::NoModifier);
        }
        CHECK(f->getScrollPosition() == QPoint{0,27});
        CHECK(f->getCursorPosition() == QPoint{0,49});
        for (int i = 0; i <= 26; i++) {
            Tui::ZTest::sendKey(&terminal, Qt::Key_Up, Qt::KeyboardModifier::ControlModifier);
            CHECK(f->getScrollPosition() == QPoint{0,26 - i});
            CHECK(f->getCursorPosition() == QPoint{0,48 - i});
        }
    }

    SECTION("move-lines-up") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Up, (Qt::KeyboardModifier::ShiftModifier | Qt::KeyboardModifier::ControlModifier));
        CHECK(doc.getLines()[0] == "    new1");
    }
    SECTION("move-lines-down") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Up, Qt::KeyboardModifier::NoModifier);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Down, (Qt::KeyboardModifier::ShiftModifier | Qt::KeyboardModifier::ControlModifier));
        CHECK(doc.getLines()[0] == "    new1");
    }

    SECTION("select-tab") {
        Tui::ZTest::sendText(&terminal, "a", Qt::KeyboardModifier::ControlModifier);
        CHECK(f->isSelect() == true);
        Tui::ZTest::sendKey(&terminal, Qt::Key_Tab, Qt::KeyboardModifier::ShiftModifier);
        CHECK(doc.getLines()[0] == "text");
        CHECK(doc.getLines()[1] == "new1");
        Tui::ZTest::sendKey(&terminal, Qt::Key_Tab, Qt::KeyboardModifier::NoModifier);
        CHECK(doc.getLines()[0] == "        text");
        CHECK(doc.getLines()[1] == "        new1");
    }
    SECTION("tab") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Tab, Qt::KeyboardModifier::ShiftModifier);
        CHECK(doc.getLines()[0] == "    text");
        CHECK(doc.getLines()[1] == "new1");
        CHECK(f->getCursorPosition() == QPoint{4, 1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Tab, Qt::KeyboardModifier::NoModifier);
        CHECK(doc.getLines()[1] == "new1    ");
        CHECK(f->getCursorPosition() == QPoint{8, 1});
    }

    //home end
    SECTION("home-home") {
        Qt::KeyboardModifier shift = GENERATE(Qt::KeyboardModifier::NoModifier, Qt::KeyboardModifier::ShiftModifier);
        CAPTURE(shift);

        CHECK(f->getCursorPosition() == QPoint{8,1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, shift);
        CHECK(f->getCursorPosition() == QPoint{0,1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, shift);
        CHECK(f->getCursorPosition() == QPoint{4,1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, shift);
        CHECK(f->getCursorPosition() == QPoint{0,1});
    }

    SECTION("home-end") {
        CHECK(f->getCursorPosition() == QPoint{8,1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::NoModifier);
        CHECK(f->getCursorPosition() == QPoint{0,1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_End, Qt::KeyboardModifier::NoModifier);
        CHECK(f->getCursorPosition() == QPoint{8,1});
    }
    SECTION("shift+home-end") {
        CHECK(f->getCursorPosition() == QPoint{8,1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::ShiftModifier);
        CHECK(f->getCursorPosition() == QPoint{0,1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_End, Qt::KeyboardModifier::ShiftModifier);
        CHECK(f->getCursorPosition() == QPoint{8,1});
    }
    SECTION("crl+home-end") {
        CHECK(f->getCursorPosition() == QPoint{8,1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        Tui::ZTest::sendKey(&terminal, Qt::Key_End, Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{8,1});
    }
    SECTION("crl+shift+home-end") {
        CHECK(f->getCursorPosition() == QPoint{8,1});
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, (Qt::KeyboardModifier::ShiftModifier | Qt::KeyboardModifier::ControlModifier));
        CHECK(f->getCursorPosition() == QPoint{0,0});
        Tui::ZTest::sendKey(&terminal, Qt::Key_End, (Qt::KeyboardModifier::ShiftModifier | Qt::KeyboardModifier::ControlModifier));
        CHECK(f->getCursorPosition() == QPoint{8,1});
    }

    //up down
    SECTION("up-down-page-up-page-down") {
        //is this case up and page-up are the same
        Qt::KeyboardModifier shift = GENERATE(Qt::KeyboardModifier::NoModifier, Qt::KeyboardModifier::ShiftModifier);
        CAPTURE(shift);

        struct TestCase { int line; Qt::Key up; Qt::Key down; };
        auto testCase = GENERATE( TestCase{__LINE__, Qt::Key_Up, Qt::Key_Down},
                                  TestCase{__LINE__, Qt::Key_PageUp, Qt::Key_PageDown});

        CHECK(f->getCursorPosition() == QPoint{8,1});
        Tui::ZTest::sendKey(&terminal, testCase.up, shift);
        CHECK(f->getCursorPosition() == QPoint{8,0});
        Tui::ZTest::sendKey(&terminal, testCase.down, shift);
        CHECK(f->getCursorPosition() == QPoint{8,1});

        Tui::ZTest::sendKey(&terminal, Qt::Key_Left, Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{4,1});
        Tui::ZTest::sendKey(&terminal, testCase.up, shift);
        CHECK(f->getCursorPosition() == QPoint{4,0});
        Tui::ZTest::sendKey(&terminal, testCase.down, shift);
        CHECK(f->getCursorPosition() == QPoint{4,1});

        Tui::ZTest::sendKey(&terminal, Qt::Key_Left, Qt::KeyboardModifier::ControlModifier);
        CHECK(f->getCursorPosition() == QPoint{0,1});
        Tui::ZTest::sendKey(&terminal, testCase.up, shift);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        Tui::ZTest::sendKey(&terminal, testCase.down, shift);
        CHECK(f->getCursorPosition() == QPoint{0,1});
    }
    SECTION("up-down-page-up-page-down") {
        Qt::KeyboardModifier shift = GENERATE(Qt::KeyboardModifier::NoModifier, Qt::KeyboardModifier::ShiftModifier);
        CAPTURE(shift);

        Tui::ZTest::sendKey(&terminal, Qt::Key_Enter, Qt::KeyboardModifier::NoModifier);
        Tui::ZTest::sendText(&terminal, "    new2", Qt::KeyboardModifier::NoModifier);
        CHECK(doc.getLines().size() == 3);

        CHECK(f->getCursorPosition() == QPoint{8,2});
        Tui::ZTest::sendKey(&terminal, Qt::Key_PageUp, shift);
        CHECK(f->getCursorPosition() == QPoint{8,0});
        Tui::ZTest::sendKey(&terminal, Qt::Key_PageDown, shift);
        CHECK(f->getCursorPosition() == QPoint{8,2});

    }

    //esc
    SECTION("esc") {
        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, (Qt::KeyboardModifier::ShiftModifier | Qt::KeyboardModifier::ControlModifier));
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == true);

        Tui::ZTest::sendKey(&terminal, Qt::Key_Escape, Qt::KeyboardModifier::NoModifier);
        CHECK(f->getCursorPosition() == QPoint{0,0});
        CHECK(f->isSelect() == true);
    }

    //eat space
    SECTION("eat space") {
        f->setTabsize(4);
        Tui::ZTest::sendText(&terminal, "a", Qt::KeyboardModifier::ControlModifier);

        //prepare enviromend
        for(int i = 1; i <= 6; i++) {
            for(int space = 1; space < i; space++) {
                Tui::ZTest::sendKey(&terminal, Qt::Key_Space, Qt::KeyboardModifier::NoModifier);
            }
            Tui::ZTest::sendText(&terminal, "a", Qt::KeyboardModifier::NoModifier);
            Tui::ZTest::sendKey(&terminal, Qt::Key_Enter, Qt::KeyboardModifier::NoModifier);
        }
        CHECK(doc.getLines()[0] == "a");
        CHECK(doc.getLines()[1] == " a");
        CHECK(doc.getLines()[2] == "  a");
        CHECK(doc.getLines()[3] == "   a");
        CHECK(doc.getLines()[4] == "    a");
        CHECK(doc.getLines()[5] == "     a");

        SECTION("tab") {
            Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::ControlModifier);
            for(int i = 1; i <= 6; i++) {
                Tui::ZTest::sendKey(&terminal, Qt::Key_Tab, Qt::KeyboardModifier::NoModifier);
                Tui::ZTest::sendKey(&terminal, Qt::Key_Down, Qt::KeyboardModifier::NoModifier);
                Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::NoModifier);
            }
            CHECK(doc.getLines()[0] == "    a");
            CHECK(doc.getLines()[1] == "    a");
            CHECK(doc.getLines()[2] == "    a");
            CHECK(doc.getLines()[3] == "    a");
            CHECK(doc.getLines()[4] == "        a");
            CHECK(doc.getLines()[5] == "        a");
        }
        SECTION("shift+tab") {
            Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::ControlModifier);
            for(int i = 1; i <= 6; i++) {
                Tui::ZTest::sendKey(&terminal, Qt::Key_Tab, Qt::KeyboardModifier::ShiftModifier);
                Tui::ZTest::sendKey(&terminal, Qt::Key_Down, Qt::KeyboardModifier::NoModifier);
            }
            CHECK(doc.getLines()[0] == "a");
            CHECK(doc.getLines()[1] == "a");
            CHECK(doc.getLines()[2] == "a");
            CHECK(doc.getLines()[3] == "a");
            CHECK(doc.getLines()[4] == "a");
            CHECK(doc.getLines()[5] == " a");
        }
    }

    //page up down
    SECTION("page") {
        //80x24 terminal
        //f->setGeometry({0, 0, 80, 24});

        Tui::ZTest::sendText(&terminal, "a", Qt::KeyboardModifier::ControlModifier);
        Qt::KeyboardModifier shift = GENERATE(Qt::KeyboardModifier::NoModifier, Qt::KeyboardModifier::ShiftModifier);
        CAPTURE(shift);

        //prepare enviromend
        for(int i = 1; i <= 50; i++) {
            Tui::ZTest::sendKey(&terminal, Qt::Key_Enter, Qt::KeyboardModifier::NoModifier);
        }

        Tui::ZTest::sendKey(&terminal, Qt::Key_Home, Qt::KeyboardModifier::ControlModifier);
        Tui::ZTest::sendKey(&terminal, Qt::Key_PageDown, shift);
        CHECK(f->getCursorPosition() == QPoint{0,23});
        Tui::ZTest::sendKey(&terminal, Qt::Key_PageDown, shift);
        CHECK(f->getCursorPosition() == QPoint{0,46});
        Tui::ZTest::sendKey(&terminal, Qt::Key_PageDown, shift);
        CHECK(f->getCursorPosition() == QPoint{0,50});

        Tui::ZTest::sendKey(&terminal, Qt::Key_PageUp, shift);
        CHECK(f->getCursorPosition() == QPoint{0,27});
        Tui::ZTest::sendKey(&terminal, Qt::Key_PageUp, shift);
        CHECK(f->getCursorPosition() == QPoint{0,4});
        Tui::ZTest::sendKey(&terminal, Qt::Key_PageUp, shift);
        CHECK(f->getCursorPosition() == QPoint{0,0});
    }

    SECTION("search") {
        EventRecorder recorder;
        auto cursorSignal = recorder.watchSignal(f, RECORDER_SIGNAL(&File::cursorPositionChanged));

        CHECK(f->getCursorPosition() == QPoint{8,1});
        f->setSearchText("t");

        Tui::ZTest::sendKey(&terminal, Qt::Key_F3, Qt::KeyboardModifier::NoModifier);
        recorder.waitForEvent(cursorSignal);
        CHECK(f->getCursorPosition() == QPoint{5,0});

        Tui::ZTest::sendKey(&terminal, Qt::Key_F3, Qt::KeyboardModifier::NoModifier);
        recorder.waitForEvent(cursorSignal);
        CHECK(f->getCursorPosition() == QPoint{8,0});

        Tui::ZTest::sendKey(&terminal, Qt::Key_F3, Qt::KeyboardModifier::NoModifier);
        recorder.waitForEvent(cursorSignal);
        CHECK(f->getCursorPosition() == QPoint{5,0});

    }
    SECTION("replace") {
        CHECK(f->getCursorPosition() == QPoint{8,1});
        f->replaceAll("1","2");
        CHECK(f->getCursorPosition() == QPoint{8,1});
        CHECK(doc.getLines()[1] == "    new2");

        f->replaceAll("e","E");
        CHECK(f->getCursorPosition() == QPoint{6,1});

        f->replaceAll(" ","   ");
        CHECK(doc.getLines()[0] == "            tExt");
        CHECK(doc.getLines()[1] == "            nEw2");
        CHECK(f->getCursorPosition() == QPoint{12,1});


    }
}

