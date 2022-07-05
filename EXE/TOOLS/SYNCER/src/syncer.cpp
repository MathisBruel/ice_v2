#include "syncer.h"

Syncer::Syncer(QWidget *parent) :
    QMainWindow(parent)
{
    Poco::Logger::root().setChannel(new Poco::ConsoleChannel());
    Poco::Logger::root().setLevel(Poco::Message::PRIO_DEBUG);

    tab = new QTabWidget();
    tab->resize(1280, 720);
    tab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    merge = new MergeWidget(tab);
    gen = new GenerateSync(tab);
    sync = new SynchroWidget(tab);
    time = new TimeBaseWidget(tab);
    tab->addTab(merge, "MERGE");
    tab->addTab(gen, "GENERATE");
    tab->addTab(sync, "SYNCHRONIZE");
    tab->addTab(time, "TIME");
    tab->show();
}

Syncer::~Syncer()
{
    
}
