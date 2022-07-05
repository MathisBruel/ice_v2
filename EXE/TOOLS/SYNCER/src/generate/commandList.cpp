#include "generate/commandList.h"

CommandList::CommandList(QWidget *parent)
    : QWidget(parent)
{
    addCommand = new QPushButton(QIcon("images/add.png"), "");
    addCommand->setIconSize(QSize(50, 50));
    connect(addCommand, SIGNAL(released()), this, SLOT(addingCommand()));
    removeCommand = new QPushButton(QIcon("images/remove.png"), "");
    removeCommand->setIconSize(QSize(50, 50));
    connect(removeCommand, SIGNAL(released()), this, SLOT(removingCommand()));

    listCommand = new QTableWidget(0, 2, this);
    QStringList header;
    header.append("Type");
    header.append("Parameters");
    listCommand->setHorizontalHeaderLabels(header);
    listCommand->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listCommand->setSelectionBehavior(QAbstractItemView::SelectRows);
    listCommand->setSelectionMode(QAbstractItemView::SingleSelection);
    listCommand->setShowGrid(false);
    listCommand->setColumnWidth(0, 300);
    QHeaderView* headerView = listCommand->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Interactive);
    headerView->setStretchLastSection(true);
    connect(listCommand, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(viewCommand(int, int)));

    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->addWidget(addCommand);
    controlLayout->addWidget(removeCommand);
    controlLayout->addStretch();

    QVBoxLayout* generalLayout = new QVBoxLayout();
    generalLayout->addItem(controlLayout);
    generalLayout->addWidget(listCommand);
    generalLayout->addStretch();

    setLayout(generalLayout);
}
    
CommandList::~CommandList()
{

}

void CommandList::addingCommand()
{
    EditCommand* edit = new EditCommand(listCommand, listCommand->rowCount(), true);
    edit->show();
}

void CommandList::removingCommand()
{
    if (listCommand->rowCount() > 0) {
        listCommand->removeRow(listCommand->currentRow());
    }
}
void CommandList::viewCommand(int row, int column)
{
    EditCommand* edit = new EditCommand(listCommand, row, false);
    edit->setData(listCommand->item(row, 0)->text(), listCommand->item(row, 1)->text());
    edit->show();
}