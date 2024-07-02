const express = require('express');
const session = require('express-session');
const { exec, execSync } = require('child_process');
const IceAPI = require('./api-ice/IceAPI');
const jwt = require('jsonwebtoken');
//const bcrypt = require('bcrypt');
const path = require('path');
// -- for parsing XML
const xml2js = require('xml2js');
const { readFileSync } = require('fs');
const fs = require('fs');
const formidable = require('formidable');

const SECRET_TOKEN = 'CentralTokenSecret';
const SECRET_SESSION = 'CentralSessionSecret';

const deleteFolderRecursive = function (directoryPath) {
    if (fs.existsSync(directoryPath)) {
        fs.readdirSync(directoryPath).forEach((file, index) => {
            const curPath = path.join(directoryPath, file);
            if (fs.lstatSync(curPath).isDirectory()) {
                // recurse
                deleteFolderRecursive(curPath);
            } else {
                // delete file
                fs.unlinkSync(curPath);
            }
        });
        fs.rmdirSync(directoryPath);
    }
};

class Main_API {

    constructor(configFile){
        this.config = JSON.parse(readFileSync(configFile, 'utf-8'));
        this.app = express();
        this.builder = new xml2js.Builder({headless: true});
        this.parser = new xml2js.Parser();
        this.client = new IceAPI(this.config.ICE_API_URL, this.config.ICE_API_PORT);
        this.logCount = 0;
        this.try_to_connect = false;
        this.users = [];
    }

    async start(){
        await this.addUser('admin', 'admin');
        await this.connectClient();

        // -- enable ejs
        this.app.set('view engine', 'ejs');

        // -- link views
        this.app.set('views', path.join(__dirname, '../views'));
        this.app.use(express.static('./images'));

        // -- permit to parse body of request (call at each route redirection)
        this.app.use(express.json());
        this.app.use(express.urlencoded({extended: true}));

        // -- definition of session (call at each route redirection)
        this.app.use(session({
            resave: false, // don't save session if unmodified
            saveUninitialized: false, // don't create session until something stored
            secret:SECRET_SESSION
        }));

        this.app.use(function(request, response, next){ this.logRoute(request, response, next); }.bind(this));

        this.init_routes();
        this.server = this.app.listen({port : this.config.API_PORT});

        this.server.on('listening', function(){
            const server = this.server.address();
            console.log(`API (${server.address}:${server.port}) started`);
        }.bind(this));

        this.server.on('error', function(error){
            console.error(`Error while initializing API :\n${error}`)
        });

        this.server.on('close', function(){
            console.log(`API closed`);
        });
    }

    async addUser(username, password){
        try {
            const saltRounds = 10;
            //bcrypt.hash(password, saltRounds, function(err, hash){
            //    if (err){ throw new Error(err); }
            //    this.users.push({ user_id:this.users.length+1, name: username, password: hash });
            //}.bind(this));
            this.users.push({ user_id:this.users.length+1, name: username, password: password });
        }
        catch(err){
            throw new Error(`Error while adding user ${username}:\n${err}`);
        }
    }

    async connectClient(id_connection = 0){
        try {
            if (!this.try_to_connect || (this.try_to_connect && id_connection === this.id_connection)){
                this.try_to_connect = true;
                await this.client.login();
                this.try_to_connect = false;
                this.id_connection = 0;
                console.log('Connected to client');
            }
        }
        catch(err){
            console.error(`Can not login to client:\n${err}`);
            this.id_connection = setTimeout(function(){
                this.connectClient(this.id_connection);
            }.bind(this), 5000);
        }
    }

    restrictPage(request, response, next){
        try {
            const token = request.session.token;
            const decodedToken = jwt.verify(token, SECRET_TOKEN);
            const userId = decodedToken.userId;
            const user = this.findUserId(userId);
            if (!user){ throw new Error(); }
            next();
        }
        catch(err){
            request.session.error = "Need to be authenticated to access this page";
            response.redirect('/login');
        }
    }

    restrictData(request, response, next){
        try {
            const token = request.session.token;
            const decodedToken = jwt.verify(token, SECRET_TOKEN);
            const userId = decodedToken.userId;
            const user = this.findUserId(userId);
            if (!user){ throw new Error(); }
            next();
        }
        catch(err){
            response.json({err: "Need to be authenticated, please reload page"});
        }
    }

    async logRoute(request, response, next){
        this.logCount++;
        if (this.logCount >= Number.MAX_VALUE -1){
            this.logCount = 1;
        }
        const logInfo = {
            id : this.logCount,
            prefixMsg: `#${this.logCount} ${request.method} ${request.url}`,
            ended: false
        }
        console.log(`${new Date(Date.now()).toISOString()} : Request ${logInfo.prefixMsg} -query:${JSON.stringify(request.query)} -body:${JSON.stringify(request.body)}`);

        const oldWrite = response.write;
        const oldEnd = response.end;
        const chunks = [];

        response.write = function(...restArgs){
            chunks.push(Buffer.from(restArgs[0]));
            return oldWrite.apply(response, restArgs);
        };

        response.end = (...restArgs) => {
            // preventing multiple calls to response.end(), like response.json().end() or response.send().end()
            // indeed, response.send() already calls end(), and json() actually call send()
            // and keeping explicit calls to .end() make it clear that the end of the response was reached
            if (logInfo.ended === false) {
                logInfo.ended = true;
                if (restArgs[0]) {
                    chunks.push(Buffer.from(restArgs[0]));
                }
                const respBody = Buffer.concat(chunks).toString('utf8');
                if (!respBody.includes('DOCTYPE')){
                    console.log(`${new Date(Date.now()).toISOString()} : Response to request ${logInfo.prefixMsg} -status:${response.statusCode} -body:${respBody}\n`);
                }
                else {
                    console.log(`${new Date(Date.now()).toISOString()} : Response to request ${logInfo.prefixMsg} -status:${response.statusCode} -body:***HTML Page***\n`);
                }
            }
            oldEnd.apply(response, restArgs);
        };

        next();
    }

    async shutdown(request, response){
        if (this.server !== undefined){
            this.server.close();
        }
        response.status(200).end();
        return;
    }

    init_routes(){

        // -- BASE
        this.app.get('/', function(request, response){ response.redirect('/groups'); });
        this.app.get('/login', function(request, response){ response.render('login', {error : request.session.error}); });
        this.app.post('/login', function(request, response){ this.login(request, response); }.bind(this));

        // -- PAGES
        this.app.get('/groups', this.restrictPage.bind(this), function(request, response){ response.render('model/groups'); }.bind(this));
        this.app.get('/group', this.restrictPage.bind(this), function(request, response){ response.render('model/group'); }.bind(this));
        this.app.get('/cinemas', this.restrictPage.bind(this), function(request, response){ response.render('model/cinemas'); }.bind(this));
        this.app.get('/cinema', this.restrictPage.bind(this), function(request, response){ response.render('model/cinema'); }.bind(this));
        this.app.get('/auditorium', this.restrictPage.bind(this), function(request, response){ response.render('model/auditorium'); }.bind(this));
        this.app.get('/server', this.restrictPage.bind(this), function(request, response){ response.render('model/server'); }.bind(this));

        this.app.get('/features', this.restrictPage.bind(this), function(request, response){ response.render('model/features'); }.bind(this));
        this.app.get('/feature', this.restrictPage.bind(this), function(request, response){ response.render('model/feature'); }.bind(this));
        this.app.get('/release', this.restrictPage.bind(this), function(request, response){ response.render('model/release'); }.bind(this));
        this.app.get('/scripts', this.restrictPage.bind(this), function(request, response){ response.render('model/scripts'); }.bind(this));
        this.app.get('/script', this.restrictPage.bind(this), function(request, response){ response.render('model/script'); }.bind(this));
        this.app.get('/cpls', this.restrictPage.bind(this), function(request, response){ response.render('model/cpls'); }.bind(this));
        this.app.get('/cpl', this.restrictPage.bind(this), function(request, response){ response.render('model/cpl'); }.bind(this));
        this.app.get('/cut', this.restrictPage.bind(this), function(request, response){ response.render('model/cut'); }.bind(this));
        this.app.get('/delivery', this.restrictPage.bind(this), function(request, response){ response.render('model/delivery'); }.bind(this));
        this.app.get('/synchro', this.restrictPage.bind(this), function(request, response){ response.render('model/synchro'); }.bind(this));
        this.app.get('/sites', this.restrictPage.bind(this), function(request, response){ response.render('model/sites'); }.bind(this));
        this.app.get('/movies', this.restrictPage.bind(this), function(request, response){ response.render('model/movies'); }.bind(this));

        // -- GET API
        this.app.post('/getGroups', this.restrictPage.bind(this), function(request, response){ this.giveGroups(request, response); }.bind(this));
        this.app.post('/getCinemas', this.restrictPage.bind(this), function(request, response){ this.giveCinemas(request, response); }.bind(this));
        this.app.post('/getAuditoriums', this.restrictPage.bind(this), function(request, response){ this.giveAuditoriums(request, response); }.bind(this));
        this.app.post('/getServers', this.restrictPage.bind(this), function(request, response){ this.giveServers(request, response); }.bind(this));
        this.app.post('/getFeatures', this.restrictPage.bind(this), function(request, response){ this.giveFeatures(request, response); }.bind(this));
        this.app.post('/getReleases', this.restrictPage.bind(this), function(request, response){ this.giveReleases(request, response); }.bind(this));
        this.app.post('/getScripts', this.restrictPage.bind(this), function(request, response){ this.giveScripts(request, response); }.bind(this));
        this.app.post('/getCpls', this.restrictPage.bind(this), function(request, response){ this.giveCpls(request, response); }.bind(this));
        this.app.post('/getSites', this.restrictPage.bind(this), function(request, response){ this.giveSites(request, response); }.bind(this));
        this.app.post('/getCplsSite', this.restrictPage.bind(this), function(request, response){ this.giveCplsSite(request, response); }.bind(this));
        this.app.post('/getGroupsFilter', this.restrictPage.bind(this), function(request, response){ this.giveGroupsFilter(request, response); }.bind(this));

        // -- GROUPS
        this.app.post('/createGroup', this.restrictPage.bind(this), function(request, response){ this.createGroup(request, response); }.bind(this));
        this.app.post('/updateGroup', this.restrictPage.bind(this), function(request, response){ this.updateGroup(request, response); }.bind(this));
        this.app.post('/deleteGroup', this.restrictPage.bind(this), function(request, response){ this.deleteGroup(request, response); }.bind(this));

        // -- CINEMAS
        this.app.post('/createCinema', this.restrictPage.bind(this), function(request, response){ this.createCinema(request, response); }.bind(this));
        this.app.post('/updateCinema', this.restrictPage.bind(this), function(request, response){ this.updateCinema(request, response); }.bind(this));
        this.app.post('/deleteCinema', this.restrictPage.bind(this), function(request, response){ this.deleteCinema(request, response); }.bind(this));

        // -- AUDITORIUMS
        this.app.post('/createAuditorium', this.restrictPage.bind(this), function(request, response){ this.createAuditorium(request, response); }.bind(this));
        this.app.post('/updateAuditorium', this.restrictPage.bind(this), function(request, response){ this.updateAuditorium(request, response); }.bind(this));
        this.app.post('/deleteAuditorium', this.restrictPage.bind(this), function(request, response){ this.deleteAuditorium(request, response); }.bind(this));

        // -- SERVERS
        this.app.post('/createServer', this.restrictPage.bind(this), function(request, response){ this.createServer(request, response); }.bind(this));
        this.app.post('/updateServer', this.restrictPage.bind(this), function(request, response){ this.updateServer(request, response); }.bind(this));
        this.app.post('/deleteServer', this.restrictPage.bind(this), function(request, response){ this.deleteServer(request, response); }.bind(this));

        // -- FEATURES
        this.app.post('/createFeature', this.restrictPage.bind(this), function(request, response){ this.createFeature(request, response); }.bind(this));
        this.app.post('/updateFeature', this.restrictPage.bind(this), function(request, response){ this.updateFeature(request, response); }.bind(this));
        this.app.post('/deleteFeature', this.restrictPage.bind(this), function(request, response){ this.deleteFeature(request, response); }.bind(this));

        this.app.post('/createRelease', this.restrictPage.bind(this), function(request, response){ this.createRelease(request, response); }.bind(this));
        this.app.post('/updateRelease', this.restrictPage.bind(this), function(request, response){ this.updateRelease(request, response); }.bind(this));
        this.app.post('/deleteRelease', this.restrictPage.bind(this), function(request, response){ this.deleteRelease(request, response); }.bind(this));

        // -- SCRIPTS
        this.app.post('/createScript', this.restrictPage.bind(this), function(request, response){ this.createScript(request, response); }.bind(this));
        this.app.post('/updateScript', this.restrictPage.bind(this), function(request, response){ this.updateScript(request, response); }.bind(this));
        this.app.post('/deleteScript', this.restrictPage.bind(this), function(request, response){ this.deleteScript(request, response); }.bind(this));

        // -- CPLS
        this.app.post('/createCpl', this.restrictPage.bind(this), function(request, response){ this.createCpl(request, response); }.bind(this));
        this.app.post('/updateCpl', this.restrictPage.bind(this), function(request, response){ this.updateCpl(request, response); }.bind(this));
        this.app.post('/deleteCpl', this.restrictPage.bind(this), function(request, response){ this.deleteCpl(request, response); }.bind(this));

        // -- CUTS
        this.app.post('/createCut', this.restrictPage.bind(this), function(request, response){ this.createCut(request, response); }.bind(this));
        this.app.post('/updateCut', this.restrictPage.bind(this), function(request, response){ this.updateCut(request, response); }.bind(this));
        this.app.post('/deleteCut', this.restrictPage.bind(this), function(request, response){ this.deleteCut(request, response); }.bind(this));
        
        // -- CONTENT
        this.app.post('/createContent', this.restrictPage.bind(this), function(request, response){ this.createContent(request, response); }.bind(this));
        this.app.post('/getContents', this.restrictPage.bind(this), function(request, response){ this.getContents(request, response); }.bind(this));
        this.app.post('/createRelease', this.restrictPage.bind(this), function(request, response){ this.createRelease(request, response); }.bind(this));
        this.app.post('/getRelease', this.restrictPage.bind(this), function(request, response){ this.getRelease(request, response); }.bind(this));
        this.app.post('/deleteRelease', this.restrictPage.bind(this), function(request, response){ this.deleteRelease(request, response); }.bind(this));

        // -- LINKS
        this.app.post('/linkCinemaToGroup', this.restrictPage.bind(this), function(request, response){ this.linkCinemaToGroup(request, response); }.bind(this));
        this.app.post('/unlinkCinemaToGroup', this.restrictPage.bind(this), function(request, response){ this.unlinkCinemaToGroup(request, response); }.bind(this));
        this.app.post('/linkReleaseToGroup', this.restrictPage.bind(this), function(request, response){ this.linkReleaseToGroup(request, response); }.bind(this));
        this.app.post('/unlinkReleaseToGroup', this.restrictPage.bind(this), function(request, response){ this.unlinkReleaseToGroup(request, response); }.bind(this));
        this.app.post('/linkCinemaToRelease', this.restrictPage.bind(this), function(request, response){ this.linkCinemaToRelease(request, response); }.bind(this));
        this.app.post('/unlinkCinemaToRelease', this.restrictPage.bind(this), function(request, response){ this.unlinkCinemaToRelease(request, response); }.bind(this));
        this.app.post('/linkCplToRelease', this.restrictPage.bind(this), function(request, response){ this.linkCplToRelease(request, response); }.bind(this));
        this.app.post('/unlinkCplToRelease', this.restrictPage.bind(this), function(request, response){ this.unlinkCplToRelease(request, response); }.bind(this));
        this.app.post('/linkCplToScript', this.restrictPage.bind(this), function(request, response){ this.linkCplToScript(request, response); }.bind(this));
        this.app.post('/unlinkCplToScript', this.restrictPage.bind(this), function(request, response){ this.unlinkCplToScript(request, response); }.bind(this));
        this.app.post('/linkScriptToRelease', this.restrictPage.bind(this), function(request, response){ this.linkScriptToRelease(request, response); }.bind(this));
        this.app.post('/unlinkScriptToRelease', this.restrictPage.bind(this), function(request, response){ this.unlinkScriptToRelease(request, response); }.bind(this));

        // -- SETTINGS
        this.app.get('/settings', this.restrictPage.bind(this), function(request, response){ response.render('settings/settings'); });
        this.app.get('/settings/settings', this.restrictData.bind(this), function(request, response){ this.giveCurrentSettings(request, response); }.bind(this));
        this.app.post('/settings/start-service', this.restrictData.bind(this), function(request, response){ this.startService(request, response); }.bind(this));
        this.app.post('/settings/stop-service', this.restrictData.bind(this), function(request, response){ this.stopService(request, response); }.bind(this));
        this.app.post('/shutdown', this.restrictData.bind(this), function(request, response){ this.shutdown(request, response); }.bind(this));
    }

    async login(request, response){
        try {
            const user = this.findUserName(request.body.username);
            if (!user){ throw new Error('User not found'); }
            await new Promise((resolve, reject) => {
                //bcrypt.compare(request.body.password, user.password, function(err, result){
                //    if (err){ reject(err); }
                //    if (!result) { reject('Failed to log, invalid password'); }
                //    request.session.token = jwt.sign(
                //        { userId: user.user_id },
                //        SECRET_TOKEN,
                //        { expiresIn: '24h' }
                //    );
                //    resolve();
                //});

                if (request.body.password === user.password) {
                    request.session.token = jwt.sign(
                        { userId: user.user_id },
                        SECRET_TOKEN,
                        { expiresIn: '24h' }
                    );
                    resolve();
                }
                else {
                    reject("Login failed !");
                }
            });
            response.redirect('/');
        }
        catch(error){
            request.session.error = error.message;
            response.redirect('/login');
            return;
        }
    }
    findUserName(user_name){
        for (const user of this.users){
            if (user.name === user_name){ return user };
        }
        return undefined;
    }
    findUserId(user_id){
        for (const user of this.users){
            if (user.user_id === user_id){ return user };
        }
        return undefined;
    }

    // -- GET API
    async giveGroups(request, response) {
        try {
            const result = await this.client.getGroups(request.body);
            console.log(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveGroups');
        }
    }
    async giveCinemas(request, response) {
        try {
            const result = await this.client.getCinemas(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveCinemas');
        }
    }
    async giveAuditoriums(request, response) {
        try {
            const result = await this.client.getAuditoriums(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveAuditoriums');
        }
    }
    async giveServers(request, response) {
        try {
            const result = await this.client.getServers(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveServers');
        }
    }
    async giveFeatures(request, response) {
        try {
            const result = await this.client.getFeatures(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveFeatures');
        }
    }
    async giveReleases(request, response) {
        try {
            const result = await this.client.getReleases(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveReleases');
        }
    }
    async giveScripts(request, response) {
        try {
            const result = await this.client.getScripts(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveScripts');
        }
    }
    async giveCpls(request, response) {
        try {
            const result = await this.client.getCpls(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveCpls');
        }
    }
    async giveSites(request, response) {
        try {
            const result = await this.client.getSites(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveSites');
        }
    }
    async giveCplsSite(request, response) {
        try {
            const result = await this.client.getCplsSite(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveCplsSite');
        }
    }
    async giveGroupsFilter(request, response) {
        try {
            const result = await this.client.getGroupsFilter(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveGroupsFilter');
        }
    }

    // -- GROUPS
    async createGroup(request, response) {
        try {
            const result = await this.client.createGroup(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'createGroup');
        }
    }
    async updateGroup(request, response) {
        try {
            const result = await this.client.updateGroup(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'updateGroup');
        }
    }
    async deleteGroup(request, response) {
        try {
            const result = await this.client.deleteGroup(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'deleteGroup');
        }
    }

    // -- CINEMAS
    async createCinema(request, response) {
        try {
            const result = await this.client.createCinema(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'createCinema');
        }
    }
    async updateCinema(request, response) {
        try {
            const result = await this.client.updateCinema(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'updateCinema');
        }
    }
    async deleteCinema(request, response) {
        try {
            const result = await this.client.deleteCinema(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'deleteCinema');
        }
    }

    // -- AUDITORIUMS
    async createAuditorium(request, response) {
        try {
            const result = await this.client.createAuditorium(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'createAuditorium');
        }
    }
    async updateAuditorium(request, response) {
        try {
            const result = await this.client.updateAuditorium(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'updateAuditorium');
        }
    }
    async deleteAuditorium(request, response) {
        try {
            const result = await this.client.deleteAuditorium(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'deleteAuditorium');
        }
    }

    // -- SERVERS
    async createServer(request, response) {
        try {
            const result = await this.client.createServer(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'createServer');
        }
    }
    async updateServer(request, response) {
        try {
            const result = await this.client.updateServer(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'updateServer');
        }
    }
    async deleteServer(request, response) {
        try {
            const result = await this.client.deleteServer(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'deleteServer');
        }
    }

    // -- FEATURES
    async createFeature(request, response) {
        try {
            const result = await this.client.createFeature(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'createFeature');
        }
    }
    async updateFeature(request, response) {
        try {
            const result = await this.client.updateFeature(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'updateFeature');
        }
    }
    async deleteFeature(request, response) {
        try {
            const result = await this.client.deleteFeature(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'deleteFeature');
        }
    }

    // -- RELEASES
    async createRelease(request, response) {
        try {
            const result = await this.client.createRelease(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'createRelease');
        }
    }
    async updateRelease(request, response) {
        try {
            const result = await this.client.updateRelease(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'updateRelease');
        }
    }
    async deleteRelease(request, response) {
        try {
            const result = await this.client.deleteRelease(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'deleteRelease');
        }
    }

    // -- SCRIPTS
    async createScript(request, response) {

        try {
      
            var form = new formidable.IncomingForm();
            form.parse(request, async (err, fields, files) => {

                const json = {
                    name: fields.name,
                    path: fields.path,
                    type: fields.type,
                    version: fields.version,
                    cis_name: fields.cis_name,
                    lvi_name: fields.lvi_name
                };
                
                fs.mkdirSync(fields.path, {recursive: true});
                fs.renameSync(files.fileCis.path, fields.path + "/" + fields.cis_name);
                fs.renameSync(files.fileLvi.path, fields.path + "/" + fields.lvi_name);
                const result = await this.client.createScript(json);
                response.json(result);
            });
        }
        catch(err){
            this.onError(response, err, 'createScript');
        }
    }
    async updateScript(request, response) {
        try {
            const result = await this.client.updateScript(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'updateScript');
        }
    }
    async deleteScript(request, response) {
        try {
            const result = await this.client.deleteScript(request.body);
            deleteFolderRecursive(request.body.path, {recursive: true});
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'deleteScript');
        }
    }

    // -- CPLS
    async createCpl(request, response) {

        try {
      
            var form = new formidable.IncomingForm();
            form.parse(request, async (err, fields, files) => {

                const json = {
                    name: fields.name,
                    uuid: fields.uuid,
                    path_cpl: fields.path + "/" + fields.cpl_name,
                    path_sync: fields.path + "/" + fields.sync_name
                };
                
                fs.renameSync(files.fileCpl.path, fields.path + "/" + fields.cpl_name);
                fs.renameSync(files.fileSync.path, fields.path + "/" + fields.sync_name);
                const result = await this.client.createCpl(json);
                response.json(result);
            });
        }
        catch(err){
            this.onError(response, err, 'createCpl');
        }
    }
    async updateCpl(request, response) {
        try {
            const result = await this.client.updateCpl(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'updateCpl');
        }
    }
    async deleteCpl(request, response) {

        try {
            const result = await this.client.deleteCpl(request.body);
            deleteFolderRecursive(request.body.path_cpl, {recursive: true});
            deleteFolderRecursive(request.body.path_sync, {recursive: true});
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'deleteCpl');
        }
    }

    // -- CUTS
    async createCut(request, response) {
        try {
            const result = await this.client.createCut(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'createCut');
        }
    }
    async updateCut(request, response) {
        try {
            const result = await this.client.updateCut(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'updateCut');
        }
    }
    async deleteCut(request, response) {

        try {
            const result = await this.client.deleteCut(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'deleteCut');
        }
    }
    // -- CONTENT
    async createContent(request, response) {
        try {
            const result = await this.client.createContent(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'createContent');
        }
    }
    async getContents(request, response) {
        try {
            const result = await this.client.getContents(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'getContents');
        }
    }
    async createRelease(request, response) {
        try {
            const result = await this.client.createRelease(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'createRelease');
        }
    }
    async getRelease(request, response) {
        console.log(request.body);
        try {
            const result = await this.client.getRelease(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'getRelease');
        }
    }
    async deleteRelease(request, response) {
        try {
            const result = await this.client.deleteRelease(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'deleteRelease');
        }
    }
    // -- LINKS
    async linkCinemaToGroup(request, response) {
        try {
            const result = await this.client.linkCinemaToGroup(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'linkCinemaToGroup');
        }
    }
    async unlinkCinemaToGroup(request, response) {
        try {
            const result = await this.client.unlinkCinemaToGroup(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'unlinkCinemaToGroup');
        }
    }
    async linkReleaseToGroup(request, response) {
        try {
            const result = await this.client.linkReleaseToGroup(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'linkReleaseToGroup');
        }
    }
    async unlinkReleaseToGroup(request, response) {
        try {
            const result = await this.client.unlinkReleaseToGroup(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'unlinkReleaseToGroup');
        }
    }
    async linkCinemaToRelease(request, response) {
        try {
            const result = await this.client.linkCinemaToRelease(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'linkCinemaToRelease');
        }
    }
    async unlinkCinemaToRelease(request, response) {
        try {
            const result = await this.client.unlinkCinemaToRelease(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'unlinkCinemaToRelease');
        }
    }
    async linkCplToRelease(request, response) {
        try {
            const result = await this.client.linkCplToRelease(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'linkCplToRelease');
        }
    }
    async unlinkCplToRelease(request, response) {
        try {
            const result = await this.client.unlinkCplToRelease(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'unlinkCplToRelease');
        }
    }
    async linkCplToScript(request, response) {
        try {
            const result = await this.client.linkCplToScript(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'linkCplToScript');
        }
    }
    async unlinkCplToScript(request, response) {
        try {
            const result = await this.client.unlinkCplToScript(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'unlinkCplToScript');
        }
    }
    async linkScriptToRelease(request, response) {
        try {
            const result = await this.client.linkScriptToRelease(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'linkScriptToRelease');
        }
    }
    async unlinkScriptToRelease(request, response) {
        try {
            const result = await this.client.unlinkScriptToRelease(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'unlinkScriptToRelease');
        }
    }

    // -- SETTINGS
    async giveCurrentSettings(request, response) {
        try {
            const config = await this.getSettings();
            response.json(config);
        }
        catch(err){
            this.onError(response, err, 'giveCurrentSettings');
        }
    }
    async getSettings(){
        return new Promise(function(resolve, reject){
            const config = {}
            for (const attribute in this.config){
                config[attribute] = this.config[attribute];
            }
            exec(`ps aux | grep ICE-CENTRAL | grep -v grep`, { encoding:'utf-8'}, function(error, stdout, stderr){
                if (!error) { 
                    stdout = stdout.split('\n')[0]; // remove '\n'
                    stdout = stdout.split(' ');
                    const arr = [];
                    for (const str of stdout){
                        if (str !== ''){
                            arr.push(str);
                        }
                    }
                    config.pid = arr[1];
                    for (const str of arr){
                        if (str.includes('priority')){
                            const tmp = str.split(':')[1];
                            config.priority = tmp;
                        }
                    }
                }
                resolve(config);
            });
        }.bind(this));
    }
    async stopService(request, response){
        const config = await this.getSettings();
        if (config.pid){
            exec(`kill -15 ${config.pid}`, {encoding:'utf-8'}, function(error, stdout, stderr){
                response.end();
            });
        }
    }
    async startService(request, response){
        const json = {};
        for (const attribute in request.body){
            if (!(request.body[attribute] === "undefined" || request.body[attribute] === "")){
                json[attribute] = request.body[attribute];
            }
        }
        const priority = json.priority ? `priority:${json.priority}`: '';
        exec(`/data/ICE2/ICE-CENTRAL ${priority} --daemon`, function(error, stdout, stderr){
            console.dir(stderr);
            console.dir(error);
            response.end();
        });
    }

    async onError(response, error, fn_name){
        const json = { function: fn_name, err: error };
        response.json(json);
        this.connectClient();
    }
}
module.exports = Main_API;

async function start(argv){
    if (argv.length !== 3){
        console.dir(`Usage : node javascript/Main_API.js configFile`);
        return ;
    }
    const api = new Main_API(argv[2]);
    try {
        await api.start();
    }
    catch(err){
        console.log(`An error occured while trying to start API :\n${err}`);
    }
}

start(process.argv);
