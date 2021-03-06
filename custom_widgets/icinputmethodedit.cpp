#include "icinputmethodedit.h"
//#include <QApplication>


ICInputMethodEdit::ICInputMethodEdit(QWidget *parent) :
    QLineEdit(parent)
{
    keyboard_ = new ICInputMethodKeyboard;
    keyboard_->SetTextEditor(this);
}

void ICInputMethodEdit::mousePressEvent(QMouseEvent *e)
{
    if(!keyboard_->isVisible())
    {
        keyboard_->move(0, 75);
        keyboard_->show();
    }
    QLineEdit::mousePressEvent(e);
}
