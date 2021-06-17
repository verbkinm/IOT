#include "tab.h"
#include "ui_tab.h"

Tab::Tab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab)
{
    ui->setupUi(this);

    connect(ui->addBtn, &QPushButton::clicked, this, &Tab::signalAdd);
    connect(ui->removeBtn, &QPushButton::clicked, this, &Tab::signalRemove);
}

Tab::~Tab()
{
    delete ui;
}

void Tab::addWidget(QWidget *widget)
{
    int count = ui->layout->count();
    int row = count / MAX_COLUMN;
    int column = count % MAX_COLUMN;

    ui->layout->addWidget(widget, row, column);
}
