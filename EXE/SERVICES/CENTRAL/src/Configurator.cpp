#include "Configurator.h"
#include "App/StateMachine.h"
Configurator::Configurator(MySQLDBConnection* DBconnection) {
    this->_dbConnection = DBconnection;
    HTTPContentInteraction* pfHTTPContentInteraction = new HTTPContentInteraction();
    HTTPPublishingInteraction* pfHTTPPublishingInteraction = new HTTPPublishingInteraction();
    HTTPReleaseInteraction* pfHTTPReleaseInteraction = new HTTPReleaseInteraction();
    HTTPCISInteraction* pfHTTPCISInteraction = new HTTPCISInteraction();
    HTTPIdleSyncInteraction* pfHTTPIdleSyncInteraction = new HTTPIdleSyncInteraction();
    HTTPCPLInteraction* pfHTTPCPLInteraction = new HTTPCPLInteraction();
    HTTPSyncInteraction* pfHTTPSyncInteraction = new HTTPSyncInteraction();
    HTTPSyncLoopInteraction* pfHTTPSyncLoopInteraction = new HTTPSyncLoopInteraction();
    HTTPInProdInteraction* pfHTTPInProdInteraction = new HTTPInProdInteraction();
    this->_context = new Context(pfHTTPContentInteraction,
                                pfHTTPPublishingInteraction,
                                pfHTTPReleaseInteraction,
                                pfHTTPCISInteraction,
                                pfHTTPIdleSyncInteraction,
                                pfHTTPCPLInteraction,
                                pfHTTPSyncInteraction,
                                pfHTTPSyncLoopInteraction,
                                pfHTTPInProdInteraction,
                                this->_dbConnection);
    this->_stateMachine = new StateMachine(this->_context);
    this->fsmMachine = this->_stateMachine->GetFSM();
    this->_httpInteractions [CommandCentral::CREATE_CONTENT] = pfHTTPContentInteraction;
    this->_httpInteractions [CommandCentral::CREATE_RELEASE] = pfHTTPPublishingInteraction;
    this->_httpInteractions [CommandCentral::DELETE_RELEASE_CONTENT] = pfHTTPPublishingInteraction;
    this->_httpInteractions [CommandCentral::DELETE_RELEASE_CPL] = pfHTTPPublishingInteraction;
    this->_httpInteractions [CommandCentral::DELETE_RELEASE_SYNC] = pfHTTPPublishingInteraction;
    this->_httpInteractions [CommandCentral::DELETE_RELEASE_SYNCLOOP] = pfHTTPPublishingInteraction;
    this->_httpInteractions [CommandCentral::RELEASE_CREATED] = pfHTTPReleaseInteraction;
    this->_httpInteractions [CommandCentral::CIS_CREATED] = pfHTTPCISInteraction;
    this->_httpInteractions [CommandCentral::CREATE_CPL] = pfHTTPIdleSyncInteraction;
    this->_httpInteractions [CommandCentral::CREATE_SYNCLOOP] = pfHTTPIdleSyncInteraction;
    this->_httpInteractions [CommandCentral::CPL_CREATED] = pfHTTPCPLInteraction;
    this->_httpInteractions [CommandCentral::SYNC_CREATED] = pfHTTPSyncInteraction;
    this->_httpInteractions [CommandCentral::SYNCLOOP_CREATED] = pfHTTPSyncLoopInteraction;
    this->_httpInteractions [CommandCentral::IMPORT_TO_PROD] = pfHTTPInProdInteraction;
}