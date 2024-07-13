#include "mycombobox.h"

#include <QMouseEvent>


MyComboBox::MyComboBox(QWidget *parent):QComboBox(parent)
{

}

void MyComboBox::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        emit on_comboBox_clicked();
    }
    QComboBox::mousePressEvent(e);
}
