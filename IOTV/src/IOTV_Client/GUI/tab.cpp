#include "tab.h"
#include "ui_tab.h"

Tab::Tab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab)
{
    ui->setupUi(this);

    connect(ui->addBtn, &QPushButton::clicked, this, &Tab::signalAdd);
    connect(ui->removeBtn, &QPushButton::clicked, this, &Tab::signalRemove);

    eventFilterOn();
}

Tab::~Tab()
{
    delete ui;
}

void Tab::addWidget(QWidget *widget)
{
    int count = ui->scrollContentLayout->count();
    int row = count / MAX_COLUMN;
    int column = count % MAX_COLUMN;

    ui->scrollContentLayout->addWidget(widget, row, column);

    ui->removeBtn->setEnabled(true);
}

QObjectList Tab::childrenPointerList() const
{
    QObjectList result;
    for (const auto &elem : ui->scrollAreaWidgetContents->children())
    {
        if(elem->objectName() != "scrollContentLayout")
            result << elem;
    }

    return result;
}

void Tab::list()
{
    for(int i = 1; i < ui->scrollAreaWidgetContents->children().length(); i++)
    {
        QWidget *widget = qobject_cast<QWidget*>(ui->scrollAreaWidgetContents->children().at(i));
        addWidget(widget);
    }

    if(ui->scrollAreaWidgetContents->children().length() == 1)
        ui->removeBtn->setEnabled(false);
}

bool Tab::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    if(event->type() == QEvent::ChildRemoved /*&& this->isVisible()*/ &&  ui->scrollAreaWidgetContents->children().length() > 0)
    {
        eventFilterOff();
        layoutClear();
        list();
        eventFilterOn();

        return false;
    }
    return true;
}

void Tab::clear()
{
    eventFilterOff();
    layoutClear();

    for(auto &elem : childrenPointerList())
        delete elem;

    eventFilterOn();
}

void Tab::eventFilterOn()
{
    ui->scrollAreaWidgetContents->installEventFilter(this);
}

void Tab::eventFilterOff()
{
    ui->scrollAreaWidgetContents->removeEventFilter(this);
}

void Tab::layoutClear()
{
    QLayoutItem *item;
    while( (item = ui->scrollContentLayout->itemAt(0)) )
    {
        ui->scrollContentLayout->removeItem( item );
        ui->scrollContentLayout->removeWidget(item->widget());
        delete item;
    }
}

void Tab::closeEvent(QCloseEvent *event)
{
    ui->scrollAreaWidgetContents->removeEventFilter(this);
    layoutClear();
    emit signalClose();

    event->accept();
}

void Tab::hideButtons()
{
    ui->addBtn->hide();
    ui->removeBtn->hide();
}
