#ifndef TAB_H
#define TAB_H

#include <QWidget>

#define MAX_COLUMN 3

namespace Ui {
class Tab;
}

class Tab : public QWidget
{
    Q_OBJECT

public:
    explicit Tab(QWidget *parent = nullptr);
    ~Tab();

    void addWidget(QWidget *widget);

signals:
    void signalAdd();
    void signalRemove();

private:
    Ui::Tab *ui;
};

#endif // TAB_H
