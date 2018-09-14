#include "statusbar.h"

StatusBar::StatusBar(Tui::ZWidget *parent) : Tui::ZWidget(parent) {
    setMaximumSize(Tui::tuiMaxSize, 1);
    setSizePolicyH(Tui::SizePolicy::Expanding);
    setSizePolicyV(Tui::SizePolicy::Fixed);
}

QSize StatusBar::sizeHint() const {
    return { 20, 1 };
}

void StatusBar::cursorPosition(int x, int y) {
    _cursorPositionX = x;
    _cursorPositionY = y;
}

void StatusBar::scrollPosition(int x, int y)
{
    _scrollPositionX = x;
    _scrollPositionY = y;
}

void StatusBar::paintEvent(Tui::ZPaintEvent *event) {

    auto *painter = event->painter();
    painter->clear({0, 0, 0}, {0, 0xaa, 0xaa});

    QString text = "| " + QString::number(_cursorPositionY +1) +":"+ QString::number(_cursorPositionX +1) + " | "+
            QString::number(_scrollPositionY +1)  +":"+ QString::number(_scrollPositionX +1);

    painter->writeWithColors(50, 0, text.toUtf8(), {0, 0, 0}, {0, 0xaa, 0xaa});
}
