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

    async getMode() {
        return await this.client.sendGetCommands(this.token, 'GET_MODE', {});
    }

    async setMode(values) {
        return await this.client.sendPostCommands(this.token, 'SWITCH_MODE', values);
    }

    async getProjection(){
        return await this.client.sendGetCommands(this.token, 'GET_CURRENT_PROJECTION', {});
    }

    async setProjection(values){
        return await this.client.sendPostCommands(this.token, 'SWITCH_PROJECTION', values);
    }

    async getStatusIMS() {
        return await this.client.sendGetCommands(this.token, 'STATUS_IMS', {});
    }

    async getCurrentSync(){
        return await this.client.sendGetCommands(this.token, 'GET_CURRENT_SYNC', {});
    }

    async getListDevices(){
        return await this.client.sendGetCommands(this.token, 'LIST_DEVICES', {});
    }

    async setColor(values){
        return await this.client.sendPostCommands(this.token, 'SET_COLOR', values);
    }

    async setPointer(values){
        return await this.client.sendPostCommands(this.token, 'SET_POINTER', values);
    }

    async setLut(values){
        return await this.client.sendPostCommands(this.token, 'SWITCH_LUT', values);
    }

    async unforce(values){
        return await this.client.sendPostCommands(this.token, 'UNFORCE_DEVICE', values);
    }

    async unforceAllDevices(){
        return await this.client.sendPostCommands(this.token, 'UNFORCE_ALL_DEVICES', {});
    }

    async getConfiguration(){
        return await this.client.sendPostCommands(this.token, 'GET_CONFIGURATION', {});
    }

    async getScenes(){
        return await this.client.sendPostCommands(this.token, 'GET_SCENES', {});
    }

    async addScene(values){
        return await this.client.sendPostCommands(this.token, 'CREATE_UPDATE_SCENE', values);
    }

    async playScene(values){
        return await this.client.sendPostCommands(this.token, 'PLAY_SCENE', values);
    }

    async getScripts(){
        return await this.client.sendPostCommands(this.token, 'GET_DOWNLOADED_SCRIPTS', {});
    }

    async playScript(values){
        return await this.client.sendPostCommands(this.token, 'LAUNCH_SCRIPT', values);
    }

    async getCPL(){
        return await this.client.sendPostCommands(this.token, 'GET_CPL_INFOS', {});
    }

    async getCPLContent(values){
        return await this.client.sendPostCommands(this.token, 'GET_CPL_CONTENT', values);
    }

    async getSyncContent(values){
        return await this.client.sendPostCommands(this.token, 'GET_SYNC_CONTENT', values);
    }

    async synchronizeCpl(values){
        return await this.client.sendPostCommands(this.token, 'SYNCHRONIZE_SCRIPT', values);
    }

    async changePriorityScript(values){
        return await this.client.sendPostCommands(this.token, 'CHANGE_PRIORITY_SCRIPT', values);
    }
}

module.exports = IceAPI;