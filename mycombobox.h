#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H

#include <QComboBox>



class MyComboBox : public QComboBox
{
    Q_OBJECT
public:
    MyComboBox(QWidget* parent);
    void mousePressEvent(QMouseEvent *e) override;

signals:
    void on_comboBox_clicked();
};

#endif // MYCOMBOBOX_H
