const xml2js = require('xml2js');
const http = require('http');
class HttpClient {

    constructor(url, port){
        this.builder = new xml2js.Builder({ headless : true});
        this.parser = new xml2js.Parser();
        this.api_host = url;
        this.port = port;
    }

    async login(username, password) {
        const json = {
            command:{
                username: username,
                password: password
            }
        }
        const bodyReqXml = this.builder.buildObject(json);

        const options = {
            host:this.api_host,
            path:'/login',
            method: 'POST',
            port:this.port,
            headers: { 'Content-Type': 'application/xml' }
        };

        const res = await new Promise(function(resolve, reject){
            const request = http.request(options, function(response){
                resolve(response);
            });
            request.on('error', function(err){
                console.log(err);
                reject(err);
            })
            request.write(bodyReqXml);
            request.end();
        });

        if (res.statusCode !== 200){
            return Promise.reject(`Error: ${res.statusCode}`);
        }
        const cookies = res.headers['set-cookie'];
        if (cookies.length == 0) {
            return Promise.reject('The session cookie is not present !');
        }
        const cookieSession = cookies[0].split('; ')[0].split("=");
        if (!cookieSession || cookieSession[0] != 'sessionId') {
            return Promise.reject('The session cookie is not sessionId !');
        }
        return Promise.resolve(cookieSession[1]);
    }

    async sendGetCommands(token, commandType, params){
        const json = { command: { type: commandType, parameters: params}};
        const bodyReqXml = this.builder.buildObject(json);
        const commandOptions = {
            host:this.api_host,
            port:this.port,
            path:'/',
            method:'POST',
            headers:{
                'Content-Type': 'application/xml',
                'Cookie': `sessionId=${token}; path=/; HttpOnly`
            }
        };

        const data = await new Promise(function(resolve, reject){
            const request = http.request(commandOptions, function(response){
                let d = '';
                response.on('data', function(chunk){
                    d += chunk;
                });
                response.on('end', function(){
                    if (!response.complete){
                        reject({ok: false, err: 'Connection was terminated while message still being send'});
                        return;
                    }
                    if (response.statusCode !== 200){
                        reject({ok: false, err: response.statusMessage});
                        return;
                    }
                    console.log(d);
                    resolve(d);
                });
            });
            request.on('error', function(err){
                console.log(err);
                reject(err);
            })
            request.write(bodyReqXml);
            console.log(bodyReqXml);
            request.end();
        });
        const result = await new Promise(function(resolve, reject){
            this.parser.parseString(data, function(err, result){
                if (err){
                    reject({ok: false, err:'Parsing response body failed'});
                }
                else {
                    resolve({ok: true, data:result});
                }
            });
        }.bind(this));
        return Promise.resolve(result);
    }

    async sendPostCommands(token, commandType, params){
        const json = { command: { type: commandType, parameters: params}};
        const bodyReqXml = this.builder.buildObject(json);
        const commandOptions = {
            host:this.api_host,
            port:this.port,
            path:'/',
            method:'POST',
            headers:{
                'Content-Type': 'application/xml',
                'Cookie': `sessionId=${token}`
            }
        };

        return await new Promise(function(resolve, reject){
            const request = http.request(commandOptions, function(response){
                let data = "";
                response.on('data', function(chunk){
                    data = data + chunk;
                });
                response.on('error', function(err){
                    reject(err);
                });
                response.on('end', function(){
                    this.parser.parseString(data, function(err, res){
                        if (err){
                            const result = {
                                code:-1,
                                message:'Parsing response body failed'
                            }
                            reject(result);
                        }
                        else {
                            const result = {
                                code: response.statusCode,
                                message: response.statusMessage,
                                data: res
                            }
                            resolve(result);
                        }
                    });
                }.bind(this));
            }.bind(this));
            request.on('error', function(err){
                console.log(err);
                reject(err);
            })
            console.log(bodyReqXml);
            request.write(bodyReqXml);
            request.end();
        }.bind(this));
    }

}

module.exports = HttpClient;