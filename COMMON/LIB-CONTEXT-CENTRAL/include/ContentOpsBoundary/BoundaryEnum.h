#pragma once
namespace ContentOpsBoundaryEnum {
    enum ContentOps_BoundaryEnum {

        // ------------------------ 
        // DATABASE HANDLE
        // ------------------------ 
        // -- GROUPS
        GET_GROUPS,
        INSERT_GROUP,
        UPDATE_GROUP,
        DELETE_GROUP,
        // -- CINEMA
        GET_CINEMAS,
        INSERT_CINEMA,
        UPDATE_CINEMA,
        DELETE_CINEMA,
        // -- AUDITORIUM
        GET_AUDITORIUMS,
        INSERT_AUDITORIUM,
        UPDATE_AUDITORIUM,
        DELETE_AUDITORIUM,
        // -- SERVER
        GET_SERVERS,
        INSERT_SERVER,
        UPDATE_SERVER,
        DELETE_SERVER,
        // -- SCRIPTS
        GET_SCRIPTS,
        INSERT_SCRIPT,
        UPDATE_SCRIPT,
        DELETE_SCRIPT,
        // -- FEATURES
        GET_FEATURES,
        INSERT_FEATURE,
        UPDATE_FEATURE,
        DELETE_FEATURE,
        // -- RELEASES
        GET_RELEASES,
        INSERT_RELEASE,
        UPDATE_RELEASE,
        DELETE_RELEASE,
        // -- CPLS
        GET_CPLS,
        INSERT_CPL,
        UPDATE_CPL,
        DELETE_CPL,
        // -- CUTS
        GET_CUT,
        INSERT_CUT,
        UPDATE_CUT,
        DELETE_CUT,

        // -- ASSOCIATIONS GROUP/CINEMA
        ADD_CINEMA_TO_GROUP,
        REMOVE_CINEMA_TO_GROUP,
        // -- ASSOCIATIONS RELEASE
        ADD_CPL_TO_RELEASE,
        REMOVE_CPL_TO_RELEASE,
        ADD_CPL_TO_SCRIPT,
        REMOVE_CPL_TO_SCRIPT,
        ADD_SCRIPT_TO_RELEASE,
        REMOVE_SCRIPT_TO_RELEASE,
        ADD_CINEMA_TO_RELEASE,
        REMOVE_CINEMA_TO_RELEASE,
        ADD_GROUP_TO_RELEASE,
        REMOVE_GROUP_TO_RELEASE,

        // ------------------------ 
        // INFOS FOR WEB INTERFACE
        // ------------------------ 
        GET_STATE_SERVER,
        GET_STATE_AUDITORIUM,
        GET_SCRIPTS_FOR_RELEASE,
        GET_RESULT_SYNCHRO,

        // ------------------------ 
        // -- ICE destination
        // ------------------------ 
        GET_SCRIPTS_FOR_SERVER,
        // -- specific REPO service
        GET_FILE,

        // ------------------------
        // -- State Machine Interaction
        // ------------------------
        CREATE_CONTENT,
        CREATE_RELEASE,
        RELEASE_CREATED,
        CIS_CREATED,
        CREATE_CPL,
        CREATE_SYNCLOOP,
        CPL_CREATED,
        SYNC_CREATED,
        SYNCLOOP_CREATED,
        IMPORT_TO_PROD,
        CANCEL,

        // ------------------------
        // -- Getters State Machine 
        // ------------------------
        GET_CONTENT,
        GET_RELEASES_CONTENT,
        DELETE_RELEASE_CONTENT,
        GET_GROUPS_FILTER,
        GET_SITES,
        GET_CPLS_SITE,
        UPDATE_RELEASE_CONTENT,
        UPDATE_CIS,
        GET_RELEASE_CPLS,
        GET_RELEASE_SYNCS,
        GET_RELEASE_SYNCLOOPS,
        GET_SERVER_PAIR,
        DELETE_RELEASE_CPL,
        DELETE_RELEASE_SYNC,
        DELETE_RELEASE_SYNCLOOP,
        
        // -- DEFAULT
        UNKNOW_COMMAND
    };
}