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

const SECRET_TOKEN = 'mySuperStringToEncode';
const SECRET_SESSION = 'myStringToEncodeSession';

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
        this.app.get('/', function(request, response){ response.redirect('/dashboard'); });

        this.app.get('/dashboard', this.restrictPage.bind(this), function(request, response){ response.render('dashboard/dashboard'); }.bind(this));
        this.app.get('/dashboard/mode', this.restrictData.bind(this), function(request, response){ this.giveMode(request, response); }.bind(this));
        this.app.post('/dashboard/mode', this.restrictData.bind(this), function(request, response){ this.setMode(request, response); }.bind(this));
        this.app.get('/dashboard/projection', this.restrictData.bind(this), function(request, response){ this.giveProjection(request, response); }.bind(this));
        this.app.post('/dashboard/projection', this.restrictData.bind(this), function(request, response){ this.setProjection(request, response); }.bind(this));
        this.app.get('/dashboard/statusIMS', this.restrictData.bind(this), function(request, response){ this.giveStatusIMS(request, response); }.bind(this));
        this.app.get('/dashboard/sync', this.restrictData.bind(this), function(request, response){ this.giveCurrentSync(request, response); }.bind(this));

        this.app.get('/devices', this.restrictPage.bind(this), function(request, response){ response.render('devices/devices'); }.bind(this));
        this.app.post('/devices/color', this.restrictData.bind(this), function(request, response){ this.setColor(request, response); }.bind(this));
        this.app.get('/devices/devices', this.restrictData.bind(this), function(request, response){ this.giveDevices(request, response); }.bind(this));
        this.app.post('/devices/lut', this.restrictData.bind(this), function(request, response){ this.setLut(request, response); }.bind(this));
        this.app.post('/devices/pointer', this.restrictData.bind(this), function(request, response){ this.setPointer(request, response); }.bind(this));
        this.app.post('/devices/unforce', this.restrictData.bind(this), function(request, response){ this.unforce(request, response); }.bind(this));
        this.app.post('/devices/unforceAllDevices', this.restrictData.bind(this), function(request, response){ this.unforceAllDevices(request, response); }.bind(this));
        
        this.app.get('/login', function(request, response){ response.render('login', {error : request.session.error}); });
        this.app.post('/login', function(request, response){ this.login(request, response); }.bind(this));

        this.app.get('/scenes', this.restrictPage.bind(this), function(request, response){ response.render(`scenes/scenes`); }.bind(this));
        this.app.get('/scenes/configuration', this.restrictData.bind(this), function(request, response){ this.giveConfiguration(request, response); }.bind(this));
        this.app.post('/scenes/play/:name', this.restrictData.bind(this), function(request, response){ this.playScene(request, response); }.bind(this));
        this.app.get('/scenes/scenes', this.restrictData.bind(this), function(request, response){ this.giveScenes(request, response); }.bind(this));
        this.app.post('/scenes/scenes', this.restrictData.bind(this), function(request, response){ this.addScene(request, response); }.bind(this));

        this.app.get('/scripts', this.restrictPage.bind(this), function(request, response){ response.render('playlist/playlist'); }.bind(this));
        this.app.get('/scripts/scripts', function(request, response){ this.giveScripts(request, response); }.bind(this));
        this.app.post('/scripts/changePriority', this.restrictData.bind(this), function(request, response){ this.changePriorityScript(request, response); }.bind(this));
        this.app.post('/scripts/launch/:name', this.restrictData.bind(this), function(request, response){ this.playScript(request, response); }.bind(this));

        this.app.get('/settings', this.restrictPage.bind(this), function(request, response){ response.render('settings/settings'); });
        this.app.get('/settings/settings', this.restrictData.bind(this), function(request, response){ this.giveCurrentSettings(request, response); }.bind(this));
        this.app.post('/settings/start-service', this.restrictData.bind(this), function(request, response){ this.startService(request, response); }.bind(this));
        this.app.post('/settings/stop-service', this.restrictData.bind(this), function(request, response){ this.stopService(request, response); }.bind(this));

        this.app.post('/shutdown', this.restrictData.bind(this), function(request, response){ this.shutdown(request, response); }.bind(this));

        this.app.get('/synchro', this.restrictPage.bind(this), function(request, response){ response.render('synchro/synchro'); }.bind(this));
        this.app.get('/synchro/cpl', this.restrictData.bind(this), function(request, response){ this.giveCPLInfo(request, response); }.bind(this));
        this.app.post('/synchro/cplContent', this.restrictData.bind(this), function(request, response){ this.giveCPLContent(request, response); }.bind(this));
        this.app.post('/synchro/syncContent', this.restrictData.bind(this), function(request, response){ this.giveSyncContent(request, response); }.bind(this));
        this.app.post('/synchro/synchronize', this.restrictData.bind(this), function(request, response){ this.synchronize(request, response); }.bind(this));
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

    async onError(response, error, fn_name){
        const json = { function: fn_name, err: error };
        response.json(json);
        this.connectClient();
    }

    async giveMode(request, response) {
        try {
            const result = await this.client.getMode();
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveMode');
        }
    }

    async setMode(request, response) {
        try {
            const result = await this.client.setMode(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'setMode');
        }
    }

    async giveProjection(request, response) {
        try {
            const result = await this.client.getProjection();
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveProjection');
        }
    }

    async setProjection(request, response) {
        try {
            const result = await this.client.setProjection(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'setProjection');
        }
    }

    async giveStatusIMS(request, response) {
        try {
            const result = await this.client.getStatusIMS();
            response.json(result);
        }
        catch(err) {
            this.onError(response, err, 'giveStatusIMS');
        }
    }

    async giveCurrentSync(request, response) {
        try {
            const result = await this.client.getCurrentSync();
            response.json(result);
        }
        catch(err) {
            this.onError(response, err, 'giveCurrentSync');
        }
    }

    async giveDevices(request, response) {
        try {
            const result = await this.client.getListDevices();
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveDevices');
        }
    }

    async setColor(request, response) {
        try {
            const result = await this.client.setColor(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'setColor');
        }
    }

    async setPointer(request, response){
        try {
            const result = await this.client.setPointer(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'setPointer');
        }
    }

    async setLut(request, response){
        try {
            const result = await this.client.setLut(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'setLut');
        }
    }

    async unforce(request, response) {
        try {
            const result = await this.client.unforce(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'unforce');
        }
    }

    async unforceAllDevices(request, response) {
        try {
            const result = await this.client.unforceAllDevices();
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'unforceAllDevices');
        }
    }

    async giveConfiguration(request, response){
        try {
            const result = await this.client.getConfiguration();
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveConfiguration');
        }
    }

    async giveScenes(request, response){
        try {
            const scenes = await this.client.getScenes();
            const result = {
                code:scenes.code,
                message:scenes.message,
                data:scenes.data.configuration.scene
            }
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveScenes');
        }
    }

    async addScene(request, response){
        try {
            const result = await this.client.addScene(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'addScene');
        }
    }

    async playScene(request, response) {
        try {
            const obj = { scene: request.params.name };
            const result = await this.client.playScene(obj);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'playScene');
        }
    }

    async giveScripts(request, response){
        try {
            const result = await this.client.getScripts();
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveScripts');
        }
    }

    async playScript(request, response){
        try {
            const json = {
                script:request.params.name,
                force:request.body.force
            };
            const result = await this.client.playScript(json);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'playScript');
        }
    }

    async giveCPLInfo(request, response){
        try {
            const result = await this.client.getCPL();
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveCPLInfo');
        }
    }

    async giveCPLContent(request, response){
        try {
            const result = await this.client.getCPLContent(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveCPLContent');
        }
    }

    async giveSyncContent(request, response){
        try {
            const result = await this.client.getSyncContent(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'giveSyncContent');
        }
    }

    async changePriorityScript(request, response){
        try {
            const result = await this.client.changePriorityScript(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'changePriorityScript');
        }
    }

    async synchronize(request, response){
        try {
            const result = await this.client.synchronizeCpl(request.body);
            response.json(result);
        }
        catch(err){
            this.onError(response, err, 'synchronize');
        }
    }

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
            exec(`ps aux | grep ICE-SERVICE | grep -v grep`, { encoding:'utf-8'}, function(error, stdout, stderr){
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
                        if (str.includes('configPath')){
                            const tmp = str.split(':')[1];
                            config.configPath = tmp;
                        } else if (str.includes('priority')){
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
        exec(`/data/ICE2/ICE-SERVICE configPath:${json.configPath === undefined ? this.config.DEFAULT_PATH_CONFIG_FOLDER : json.configPath} ${priority} --daemon`, function(error, stdout, stderr){
            console.dir(stderr);
            console.dir(error);
            response.end();
        });
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
