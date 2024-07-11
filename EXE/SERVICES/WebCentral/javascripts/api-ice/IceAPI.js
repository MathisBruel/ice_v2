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
        const id_group = parseInt(values.id_group);
        const site = [randomInt(0,10), randomInt(0,10)];
        return {"ok":true,"data":{"sites":{"site":[{"$":{"id_site":site[0], "name":"Site " + site[0].toString(), "id_group":id_group}},
                                                   {"$":{"id_site":site[1], "name":"Site " + site[1].toString(), "id_group":id_group}}]}}};
    }
    async getCplsSite(values) {
        return {"ok":true,"data":{"cpls":{"cpl":[
            {"$":{"CPL_UUID":"2b79b259-384d-478c-adec-f19f01bbc574", "id_movie":randomInt(0,5), "id_type":randomInt(0,15), "id_localisation":randomInt(0,6), "Lorem": "Lorem ipsum dolor sit amet"}},
            {"$":{"CPL_UUID":"2b79b259-384d-478c-adec-f19f01bbc574", "id_movie":randomInt(0,5), "id_type":randomInt(0,15), "id_localisation":randomInt(0,6), "Lorem": "Lorem ipsum dolor sit amet"}},
            {"$":{"CPL_UUID":"2b79b259-384d-478c-adec-f19f01bbc574", "id_movie":randomInt(0,5), "id_type":randomInt(0,15), "id_localisation":randomInt(0,6), "Lorem": "Lorem ipsum dolor sit amet"}}]}}};
        return await this.client.sendGetCommands(this.token, 'GET_CPLS_SITE', values);
    }
    async getGroupsFilter(values) {
        return {"ok":true,"data":{"groups":{"group":[
                {"$":{"id_group":1, "id_group_parent":"","name":"France"}},
                {"$":{"id_group":2, "id_group_parent":"","name":"USA"}},
                {"$":{"id_group":3, "id_group_parent": 1,"name":"Espagne"}},
                {"$":{"id_group":4, "id_group_parent":"","name":"Estonie"}},
                {"$":{"id_group":5, "id_group_parent": 2,"name":"Inde"}},
                {"$":{"id_group":6, "id_group_parent": 4,"name":"Teaser"}},
                {"$":{"id_group":7, "id_group_parent":"","name":"Promo"}}]}}};
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
        // return {"ok":true, code:200, message:"Content created", data:{"contentId":values.id_movie}};
        return await this.client.sendPostCommands(this.token, 'CREATE_CONTENT', values);
    }
    async getContents (values) { 
        return {"ok":true,"data":{"contents":{"content":[{"$":{"id_movie":1,"content":"Furiosa","State":"Publishing","Locked":true}},
        {"$":{"id_movie":2,"content":"Apes","State":"createMovie","Locked":false}},{"$":{"id_movie":3,"content":"Apes","State":"createMovie","Locked":true}},
        {"$":{"id_movie":4,"content":"Apes","State":"createMovie","Locked":false}},{"$":{"id_movie":5,"content":"Apes","State":"createMovie","Locked":false}}]}}};
        return await this.client.sendPostCommands(this.token, 'GET_CONTENT', values);
    }
    async createRelease (values) { 
        return {"ok":true, code:200, message:"Release created", data:{"id_movie": values.id_movie, "id_type": values.id_type,"id_localisation": values.id_localisation}};
        return await this.client.sendPostCommands(this.token, 'INSERT_RELEASE', values);
    }
    async getRelease (values) { 
        return {"ok":true,"data":{"releases":{"release":[{"$":{"id_movie": values.id_movie ,"id_type":randomInt(0,15),"id_localisation":randomInt(0,6)}},
        {"$":{"id_movie": values.id_movie ,"id_type":randomInt(0,15),"id_localisation":randomInt(0,6),"release_path":"/bin"}}]}}}
        return await this.client.sendPostCommands(this.token, 'GET_RELEASE', values);
    }
    async deleteRelease (values) { 
        return {"ok":true, code:200, message:"Release deleted", data:{}};
        return await this.client.sendPostCommands(this.token, 'DELETE_RELEASE', values);
    }

}

module.exports = IceAPI;