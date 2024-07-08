#include "Configurator.h"
#include "App/StateMachine.h"
Configurator::Configurator() {
 
    HTTPIdleInteraction* pfHTTPIdleInteraction = new HTTPIdleInteraction();
    HTTPContentInteraction* pfHTTPContentInteraction = new HTTPContentInteraction();
    HTTPReleaseInteraction* pfHTTPReleaseInteraction = new HTTPReleaseInteraction();
    HTTPPublishingInteraction* pfHTTPPublishingInteraction = new HTTPPublishingInteraction();
    HTTPCISInteraction* pfHTTPCISInteraction = new HTTPCISInteraction();
    HTTPIdleSyncInteraction* pfHTTPIdleSyncInteraction = new HTTPIdleSyncInteraction();
    HTTPCPLInteraction* pfHTTPCPLInteraction = new HTTPCPLInteraction();
    HTTPSyncInteraction* pfHTTPSyncInteraction = new HTTPSyncInteraction();
    HTTPSyncLoopInteraction* pfHTTPSyncLoopInteraction = new HTTPSyncLoopInteraction();
    HTTPInProdInteraction* pfHTTPInProdInteraction = new HTTPInProdInteraction();
    this->_context = new Context(pfHTTPIdleInteraction, 
                                pfHTTPContentInteraction, 
                                pfHTTPReleaseInteraction, 
                                pfHTTPPublishingInteraction, 
                                pfHTTPCISInteraction, 
                                pfHTTPIdleSyncInteraction, 
                                pfHTTPCPLInteraction, 
                                pfHTTPSyncInteraction, 
                                pfHTTPSyncLoopInteraction, 
                                pfHTTPInProdInteraction);
    this->_stateMachine = new StateMachine(this->_context);
    this->fsmMachine = this->_stateMachine->GetFSM();
    this->_httpInteractions [CommandCentral::CREATE_CONTENT] = pfHTTPIdleInteraction;
    this->_httpInteractions [CommandCentral::CONTENT_CREATED] = pfHTTPContentInteraction;
    this->_httpInteractions [CommandCentral::RELEASE_CREATED] = pfHTTPReleaseInteraction;
    this->_httpInteractions [CommandCentral::CREATE_RELEASE] = pfHTTPPublishingInteraction;
    this->_httpInteractions [CommandCentral::CIS_CREATED] = pfHTTPCISInteraction;
    this->_httpInteractions [CommandCentral::CREATE_CPL] = pfHTTPIdleSyncInteraction;
    this->_httpInteractions [CommandCentral::CREATE_SYNCLOOP] = pfHTTPIdleSyncInteraction;
    this->_httpInteractions [CommandCentral::CPL_CREATED] = pfHTTPCPLInteraction;
    this->_httpInteractions [CommandCentral::SYNC_CREATED] = pfHTTPSyncInteraction;
    this->_httpInteractions [CommandCentral::SYNCLOOP_CREATED] = pfHTTPSyncLoopInteraction;
    this->_httpInteractions [CommandCentral::IMPORT_TO_PROD] = pfHTTPInProdInteraction;
}
 