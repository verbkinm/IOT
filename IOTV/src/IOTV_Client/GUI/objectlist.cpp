#include "objectlist.h"
#include "ui_objectlist.h"

ObjectList::ObjectList(QObjectList objectlist, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ObjectList), _objectlist(objectlist)
{
    ui->setupUi(this);

    _model = new QStandardItemModel();
    fillModel(_model);
    ui->treeView->setModel(_model);
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
        if(obj->parent()->objectName() == "scrollAreaWidgetContents")   //!!! заплатка
            item->setData(obj->objectName(), Qt::DisplayRole);
        else
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

    for (int i = 0; i < _model->rowCount(); ++i)
    {
        if(_model->item(i,0)->checkState() == Qt::Checked)
            result << _objectlist.at(i);
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
