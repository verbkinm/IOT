#ifndef TAB_H
#define TAB_H

#include <QWidget>
#include <QCloseEvent>

#define MAX_COLUMN 3

namespace Ui {
class Tab;
}

class Tab : public QWidget
{
    Q_OBJECT

public:
    Tab(QWidget *parent = nullptr);
    ~Tab();

    void addWidget(QWidget *widget);
    QObjectList childrenPointerList() const;
    virtual void list();

    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    void clear();

private:
    Ui::Tab *ui;

protected:
    void eventFilterOn();
    void eventFilterOff();

    virtual void layoutClear();
    virtual void closeEvent(QCloseEvent *event) override;
//    QLayout *scrollContentLayout();

    void hideButtons();

signals:
    void signalAdd();
    void signalRemove();
    void signalClose();
};

#endif // TAB_H
