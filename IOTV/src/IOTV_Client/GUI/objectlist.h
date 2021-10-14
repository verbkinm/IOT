#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include <QDialog>
#include <QListWidgetItem>
#include <QStandardItemModel>

namespace Ui {
class ObjectList;
}

class ObjectList : public QDialog
{
    Q_OBJECT

public:
    ObjectList(QObjectList objectlist, QWidget *parent = nullptr);
    ~ObjectList();

    QObjectList checkedObject() const;

private:
    Ui::ObjectList *ui;

    QStandardItemModel *_model;
    QObjectList _objectlist;

    void fillModel(QStandardItemModel* model);

private slots:
    void slotClickItem(const QModelIndex& index);
};

#endif // OBJECTLIST_H
