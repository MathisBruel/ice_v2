#include "generate/cutSceneList.h"

CutSceneList::CutSceneList(CutScenes* cuts, QWidget *parent)
    : QWidget(parent)
{
    this->cuts = cuts;
    addCut = new QPushButton(QIcon("images/add.png"), "");
    addCut->setIconSize(QSize(50, 50));
    connect(addCut, SIGNAL(released()), this, SLOT(addingCut()));
    removeCut = new QPushButton(QIcon("images/remove.png"), "");
    removeCut->setIconSize(QSize(50, 50));
    connect(removeCut, SIGNAL(released()), this, SLOT(removingCut()));

    listCuts = new QTableWidget(0, 3, this);
    QStringList header;
    header.append("Start frame");
    header.append("End frame");
    header.append("Description");
    listCuts->setHorizontalHeaderLabels(header);
    listCuts->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listCuts->setSelectionBehavior(QAbstractItemView::SelectRows);
    listCuts->setSelectionMode(QAbstractItemView::SingleSelection);
    listCuts->setShowGrid(false);
    listCuts->setColumnWidth(0, 100);
    listCuts->setColumnWidth(1, 100);
    QHeaderView* headerView = listCuts->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Interactive);
    headerView->setStretchLastSection(true);
    connect(listCuts, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(viewCut(int, int)));

    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->addWidget(addCut);
    controlLayout->addWidget(removeCut);
    controlLayout->addStretch();

    QVBoxLayout* generalLayout = new QVBoxLayout();
    generalLayout->addItem(controlLayout);
    generalLayout->addWidget(listCuts);
    generalLayout->addStretch();

    setLayout(generalLayout);
}
    
CutSceneList::~CutSceneList()
{

}

void CutSceneList::addingCut()
{
    EditCut* edit = new EditCut(cuts, listCuts, listCuts->rowCount(), true);
    connect(edit, SIGNAL(validateCuts()), this, SIGNAL(updateCuts()));
    edit->show();
}

void CutSceneList::removingCut()
{
    if (listCuts->rowCount() > 0) {
        cuts->removeCut(listCuts->currentRow());
        listCuts->removeRow(listCuts->currentRow());
        emit updateCuts();
    }
}
void CutSceneList::viewCut(int row, int column)
{
    EditCut* edit = new EditCut(cuts, listCuts, row, false);
    edit->setData(cuts->getListOfCuts().at(row).startFrame, cuts->getListOfCuts().at(row).endFrame, 
                    cuts->getListOfCuts().at(row).description);
    connect(edit, SIGNAL(validateCuts()), this, SIGNAL(updateCuts()));
    edit->show();
}