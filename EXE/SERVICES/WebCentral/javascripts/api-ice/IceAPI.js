const { randomInt } = require("crypto");
const HttpClient = require("./httpClient");

class IceAPI {

    constructor(url, port){
        this.client = new HttpClient(url, port);
        this.username = 'admin';
        this.password = 'ice17180';
    }

    async login() {
        this.token = await this.client.login(this.username, this.password);
    }

    async getGroups(values) {
        return await this.client.sendGetCommands(this.token, 'GET_GROUPS', values);
    }
    async getCinemas(values) {
        return await this.client.sendGetCommands(this.token, 'GET_CINEMAS', values);
    }
    async getAuditoriums(values) {
        return await this.client.sendGetCommands(this.token, 'GET_AUDITORIUMS', values);
    }
    async getServers(values) {
        return await this.client.sendGetCommands(this.token, 'GET_SERVERS', values);
    }
    async getFeatures(values) {
        return await this.client.sendGetCommands(this.token, 'GET_FEATURES', values);
    }
    async getReleases(values) {
        return await this.client.sendGetCommands(this.token, 'GET_RELEASES', values);
    }
    async getScripts(values) {
        return await this.client.sendGetCommands(this.token, 'GET_SCRIPTS', values);
    }
    async getCpls(values) {
        return await this.client.sendGetCommands(this.token, 'GET_CPLS', values);
    }
    async getSites(values) {
        return await this.client.sendGetCommands(this.token, 'GET_SITES', values);
    }
    async getCplsSite(values) {
        return await this.client.sendGetCommands(this.token, 'GET_CPLS_SITE', values);
    }
    async getGroupsFilter(values) {
        return await this.client.sendGetCommands(this.token, 'GET_GROUPS_FILTER', values);
    }

    async createGroup(values) {
        return await this.client.sendPostCommands(this.token, 'INSERT_GROUP', values);
    }
    async updateGroup(values) {
        return await this.client.sendPostCommands(this.token, 'UPDATE_GROUP', values);
    }
    async deleteGroup(values) {
        return await this.client.sendPostCommands(this.token, 'DELETE_GROUP', values);
    }

    async linkCinemaToGroup(values) {
        return await this.client.sendPostCommands(this.token, 'ADD_CINEMA_TO_GROUP', values);
    }
    async unlinkCinemaToGroup(values) {
        return await this.client.sendPostCommands(this.token, 'REMOVE_CINEMA_TO_GROUP', values);
    }
    async linkReleaseToGroup(values) {
        return await this.client.sendPostCommands(this.token, 'ADD_GROUP_TO_RELEASE', values);
    }
    async unlinkReleaseToGroup(values) {
        return await this.client.sendPostCommands(this.token, 'REMOVE_GROUP_TO_RELEASE', values);
    }
    async linkCinemaToRelease(values) {
        return await this.client.sendPostCommands(this.token, 'ADD_CINEMA_TO_RELEASE', values);
    }
    async unlinkCinemaToRelease(values) {
        return await this.client.sendPostCommands(this.token, 'REMOVE_CINEMA_TO_RELEASE', values);
    }
    async linkCplToRelease(values) {
        return await this.client.sendPostCommands(this.token, 'ADD_CPL_TO_RELEASE', values);
    }
    async unlinkCplToRelease(values) {
        return await this.client.sendPostCommands(this.token, 'REMOVE_CPL_TO_RELEASE', values);
    }
    async linkCplToScript(values) {
        return await this.client.sendPostCommands(this.token, 'ADD_CPL_TO_SCRIPT', values);
    }
    async unlinkCplToScript(values) {
        return await this.client.sendPostCommands(this.token, 'REMOVE_CPL_TO_SCRIPT', values);
    }
    async linkScriptToRelease(values) {
        return await this.client.sendPostCommands(this.token, 'ADD_SCRIPT_TO_RELEASE', values);
    }
    async unlinkScriptToRelease(values) {
        return await this.client.sendPostCommands(this.token, 'REMOVE_SCRIPT_TO_RELEASE', values);
    }

    // -- CINEMAS
    async createCinema(values) {
        return await this.client.sendPostCommands(this.token, 'INSERT_CINEMA', values);
    }
    async updateCinema(values) {
        return await this.client.sendPostCommands(this.token, 'UPDATE_CINEMA', values);
    }
    async deleteCinema(values) {
        return await this.client.sendPostCommands(this.token, 'DELETE_CINEMA', values);
    }

    // -- AUDITORIUMS
    async createAuditorium(values) {
        return await this.client.sendPostCommands(this.token, 'INSERT_AUDITORIUM', values);
    }
    async updateAuditorium(values) {
        return await this.client.sendPostCommands(this.token, 'UPDATE_AUDITORIUM', values);
    }
    async deleteAuditorium(values) {
        return await this.client.sendPostCommands(this.token, 'DELETE_AUDITORIUM', values);
    }

    // -- SERVERS
    async createServer(values) {
        return await this.client.sendPostCommands(this.token, 'INSERT_SERVER', values);
    }
    async updateServer(values) {
        return await this.client.sendPostCommands(this.token, 'UPDATE_SERVER', values);
    }
    async deleteServer(values) {
        return await this.client.sendPostCommands(this.token, 'DELETE_SERVER', values);
    }

    // -- FEATURES
    async createFeature(values) {
        return await this.client.sendPostCommands(this.token, 'INSERT_FEATURE', values);
    }
    async updateFeature(values) {
        return await this.client.sendPostCommands(this.token, 'UPDATE_FEATURE', values);
    }
    async deleteFeature(values) {
        return await this.client.sendPostCommands(this.token, 'DELETE_FEATURE', values);
    }

    // -- RELEASES
    async createRelease(values) {
        return await this.client.sendPostCommands(this.token, 'INSERT_RELEASE', values);
    }
    async updateRelease(values) {
        return await this.client.sendPostCommands(this.token, 'UPDATE_RELEASE', values);
    }
    async deleteRelease(values) {
        return await this.client.sendPostCommands(this.token, 'DELETE_RELEASE', values);
    }

    // -- SCRIPTS
    async createScript(values) {
        return await this.client.sendPostCommands(this.token, 'INSERT_SCRIPT', values);
    }
    async updateScript(values) {
        return await this.client.sendPostCommands(this.token, 'UPDATE_SCRIPT', values);
    }
    async deleteScript(values) {
        return await this.client.sendPostCommands(this.token, 'DELETE_SCRIPT', values);
    }

    // -- CPLS
    async createCpl(values) {
        return await this.client.sendPostCommands(this.token, 'INSERT_CPL', values);
    }
    async updateCpl(values) {
        return await this.client.sendPostCommands(this.token, 'UPDATE_CPL', values);
    }
    async deleteCpl(values) {
        return await this.client.sendPostCommands(this.token, 'DELETE_CPL', values);
    }

    // -- CUTS
    async createCut(values) {
        return await this.client.sendPostCommands(this.token, 'INSERT_CUT', values);
    }
    async updateCut(values) {
        return await this.client.sendPostCommands(this.token, 'UPDATE_CUT', values);
    }
    async deleteCut(values) {
        return await this.client.sendPostCommands(this.token, 'DELETE_CUT', values);
    }

    // -- CONTENT

    async createContent (values) { 
        return await this.client.sendPostCommands(this.token, 'CREATE_CONTENT', values);
    }
    async getContents (values) { 
        return await this.client.sendGetCommands(this.token, 'GET_CONTENT', values);
    }
    async createReleases (values) { 
        return await this.client.sendPostCommands(this.token, 'CREATE_RELEASE', values);    
    }
    async getReleasesContent (values) { 

        return await this.client.sendGetCommands(this.token, 'GET_RELEASES_CONTENT', values);
    }
    async deleteReleases (values) { 
        
        return await this.client.sendPostCommands(this.token, 'DELETE_RELEASE_CONTENT', values);
    }
    async updateReleaseContent (values) {
        return await this.client.sendPostCommands(this.token, 'UPDATE_RELEASE_CONTENT', values);
    }
    async insertCIS (values) {
        return await this.client.sendPostCommands(this.token, 'CIS_CREATED', values);
    }
    async updateCIS (values) {
        return await this.client.sendPostCommands(this.token, 'UPDATE_CIS', values);
    }
   async addSyncLoopToRelease (values) {
        return await this.client.sendPostCommands(this.token, 'CREATE_SYNCLOOP', values);
    }
    async getSyncLoop (values) {
        return await this.client.sendGetCommands(this.token, 'GET_RELEASE_SYNCLOOPS', values);
    }
    async deleteSyncLoop (values) {
        return await this.client.sendPostCommands(this.token, 'DELETE_RELEASE_SYNCLOOP', values);
    }
    async addCplToRelease (values) {
        return await this.client.sendPostCommands(this.token, 'CREATE_CPL', values);
    }
    async getCpl (values) {
        return await this.client.sendGetCommands(this.token, 'GET_RELEASE_CPLS', values);
    }
    async deleteCpl (values) {
        return await this.client.sendPostCommands(this.token, 'DELETE_RELEASE_CPL', values);
    }
    async addSyncToCpl (values) {
        return await this.client.sendPostCommands(this.token, 'SYNC_CREATED', values);
    }
    async getSync (values) {
        return await this.client.sendGetCommands(this.token, 'GET_RELEASE_SYNCS', values);
    }
    async deleteSync (values) {
        return await this.client.sendPostCommands(this.token, 'DELETE_RELEASE_SYNC', values);
    }
    async getServPair (values) {
        return await this.client.sendGetCommands(this.token, 'GET_SERVER_PAIR', values);
    }
}

module.exports = IceAPI;