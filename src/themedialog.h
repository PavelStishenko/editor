#ifndef THEME_H
#define THEME_H

#include <Tui/ZButton.h>
#include <Tui/ZListView.h>

#include <testtui_lib.h>

class Editor;

class ThemeDialog : public Dialog {
    Q_OBJECT

public:
    ThemeDialog(Editor *edit);

public slots:
    void open();
    void close();

private:
    Tui::ZListView *_lv = nullptr;
    Tui::ZButton *_cancelButton = nullptr;
    Tui::ZButton *_okButton = nullptr;
};

#endif // THEME_H
