#include "objectlist.h"
#include "ui_objectlist.h"

ObjectList::ObjectList(QObjectList objectlist, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObjectList), _objectlist(objectlist)
{
    ui->setupUi(this);

    model = new QStandardItemModel();
    fillModel(model);
    ui->treeView->setModel(model);
    ui->treeView->setHeaderHidden(true);
    ui->treeView->resizeColumnToContents(0);

    connect(ui->treeView, SIGNAL(clicked(const QModelIndex&)), SLOT(slotClickItem(const QModelIndex&)));
}

ObjectList::~ObjectList()
{
    delete ui;
}

void ObjectList::fillModel(QStandardItemModel* model)
{
    QList<QStandardItem*> row;
    for (auto &obj : _objectlist)
    {
        QStandardItem* item = new QStandardItem();
        item->setCheckable(true);
        item->setEditable(false);
        row.append(item);

        item = new QStandardItem;
        item->setData(obj->parent()->objectName() + ": " + obj->objectName(), Qt::DisplayRole);
        item->setEditable(false);
        row.append(item);
        model->appendRow(row);
        row.clear();
    }
}

QObjectList ObjectList::checkedObject() const
{
    QObjectList result;

    for (int i = 0; i < model->rowCount(); ++i)
    {
        if(model->item(i,0)->checkState() == Qt::Checked)
            result << _objectlist.at(i);// = model->item(i,1)->data(Qt::DisplayRole).toString();
    }

    return result;
}

void ObjectList::slotClickItem(const QModelIndex &index)
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->treeView->model());
    QStandardItem* clickedItem = model->itemFromIndex(index);

    if (clickedItem->column() != 0)
    {
        QStandardItem* checkedItem = model->item(clickedItem->row(), 0);
        Qt::CheckState currentState = checkedItem->checkState();
        Qt::CheckState nextState = (currentState == Qt::Checked ? Qt::Unchecked : Qt::Checked);
        checkedItem->setCheckState(nextState);
    }
}
