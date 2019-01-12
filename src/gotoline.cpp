#include "gotoline.h"
#include <file.h>

GotoLine::GotoLine(Tui::ZWidget *parent, File *file) : Dialog(parent) {

    setWindowTitle("Goto Line");
    setContentsMargins({ 1, 1, 1, 1});

    VBoxLayout *vbox = new VBoxLayout();
    setLayout(vbox);
    vbox->setSpacing(1);

    HBoxLayout* hbox1 = new HBoxLayout();

    //file_goto_line = new Dialog(this);
    //file_goto_line->setGeometry({20, 2, 40, 8});
    //file_goto_line->focus();

    Label *l1 = new Label(this);
    l1->setText("Goto line: ");
    //l1->setGeometry({1,2,12,1});
    hbox1->addWidget(l1);

    InputBox *i1 = new InputBox(this);
    //i1->setGeometry({15,2,3,1});
    i1->setFocus();
    hbox1->addWidget(i1);

    vbox->add(hbox1);
    vbox->addStretch();

    HBoxLayout* hbox2 = new HBoxLayout();

    Button *b0 = new Button(this);
    b0->setGeometry({3, 5, 7, 7});
    b0->setText(" Cancle");
    hbox2->addWidget(b0);

    Button *b1 = new Button(this);
    b1->setGeometry({15, 5, 7, 7});
    b1->setText(" OK");
    b1->setDefault(true);
    hbox2->addWidget(b1);

    vbox->add(hbox2);

    QObject::connect(b0, &Button::clicked, this, &GotoLine::rejected);
    QObject::connect(b1, &Button::clicked, [=]{
        if(i1->text().toInt() >= 0) {
            file->gotoline(i1->text().toInt());
            this->deleteLater();
        }
        //TODO: error message
    });
}
