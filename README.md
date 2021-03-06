# Tutorial: Write your own C client and Python server

A Tutorial/Example of how you can write a web client in C that connects to any web server using an encrypted https connection. 
Furthermore, we're going to write a Flask web server and deploy it using Nginx. We'll also generate self-signed certificates for test purposes.

**If you like this kind of tutorials make sure you give it a star so I see your interest. If you've any problems please create an issue so we can help you.**

![test](https://user-images.githubusercontent.com/25117793/90982022-bdaddd80-e564-11ea-8b88-c5b13c692214.gif)

**The first part of the tutorial (setting up the server) can be quite complicated if you don't know what you're doing. If you are just interested in making the client you can skip the first part and use a service like [Random API](https://www.random.org/) to make requests with the client.** 

## Contents
* [Important Notes](#Important-Notes)
* [File Structure](#File-Structure)  
* [Building a web server for testing the client](#Build-the-web-server) 
(if you don't need a web server for testing you can jump to [the next step](#Write-the-web-client))
  * [Write a tiny web service with the Python Flask library](#Write-a-tiny-web-service-using-Flask)
  * [Deploy the service using uWSGI and Nginx](#Deploy-the-service-using-uWSGI-and-Nginx)
  * [Create your own CA (Certificate Authority) and generate self-signed certificates for the https connection](#Create-own-CA-with-self-signed-certificates)
* [Write the web client](#Write-the-web-client)
  * [Curl and Libcurl](#Curl-and-Libcurl)
  * [Implement the client](#Implement-the-client)

## Important Notes

* I'm using `Linux (Ubuntu 18.04)` to run the server and the client.
* **I'll show you how to create self-signed certificates. If you build a web service for production don't use this method.** Instead use a CA like Let's Encrypt (it's also free).
* This tutorial is based on the current repository. The file structure of the repo is explained below. <br />
* **There are 2 possible ways you can test out the code/build something upon it:**
  1. Downloading this repo (using it as a template) and following the instructions/explanations below to run it (there are some files you need to create for the server such as certificates, otherwise it will not work). I'll explain how to configure everything by using this repo as some kind of template. If you only want the client then you just need to compile the `client.c` file.
  2. Writing everything by yourself without downloading the repo (more work to do).
* Before you start setting up the server (web service) please think where you want it to run. Later you will need the hostname of the computer where you want to run the server (for configurations and sending requests). If you want it to run on your local machine (same as client) your hostname will be `localhost` or `127.0.0.1` (ip address). If you want the server to run on a different machine than the client (but still in your home network) you need to use either that machines local ip address or the local hostname. I'll run the server on my homeserver whose hostname is `api` or `api.fritz.box` (fritz.box is the local domain of my router which will forward the requests to the api homeserver). 

## File Structure

This is the file structure of the project which may help you to understand the config files:

c-client-flask-server-test/ (root) <br />
   | - requirements.txt (all python3 pip requirements you need to install) <br />
   | - config/ (config files) <br />
&nbsp;&nbsp;&nbsp;&nbsp;
      | - api (nginx backup file) <br />
&nbsp;&nbsp;&nbsp;&nbsp;
      | - api.ini (uwsgi config file) <br /> 
&nbsp;&nbsp;&nbsp;&nbsp;
      | - api.service (uwsgi service backup file) <br />
&nbsp;&nbsp;&nbsp;&nbsp;
      | - cert/ (the tutorial shows how to generate all these files) <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          | - api.crt <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          | - api.csr <br />
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          | - api.key <br />
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          | - myApiCA.key <br />
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          | - myApiCA.pem <br />
          &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          | - myApiCA.srl <br />
   | - src/ (source code) <br />
&nbsp;&nbsp;&nbsp;&nbsp;
   | - client/ <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      | - client.c <br />
&nbsp;&nbsp;&nbsp;&nbsp;
   | - server/ <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      | - \_\_init\_\_.py <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      | - testserver.py <br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
      | - wsgi.py <br />
      
## Build the web server
Since I decided to run the web service on a dedicated server I need to connect to the server via ssh. In my case:
``` ssh cedric@api.fritz.box ```
If you also use a dedicated server you need to download the repo on both machines.
Now we can jump into the project folder:
```
cd c-client-flask-server-test
```
### Write a tiny web service using Flask
Let's start by writing the web service and test it with a normal web browser. Later we'll connect to it via the C client. My idea of the service is that we can request a certain route and it responds with JSON giving us the request method (e.g. GET) and the specified request route. For example, if the server address is 'localhost' a request like http://localhost/hello-world (route is '/hello-world') should give the following response: `{method: "GET", path: "hello-world"}`
We're going to write this little echo-service in Python using a great library that becomes more and more popular - Flask. 
First, we need to set up a virtual python environment. I'll do this using virtualenv.
```
python3 -m pip install virtualenv
```
If you are not familiar with pip and virtualenv check out this link: https://www.dabapps.com/blog/introduction-to-pip-and-virtualenv-python/
I called my virtual environment 'web_test_env'. Create an environment inside the project folder:
```
virtualenv web_test_env
```
Activate the environment:
```
source web_test_env/bin/activate
```
Then we can install all requirements that we're going to need (The requirements file is called 'requirements.txt'):
```
pip install -r requirements.txt
```
#### Flask Server Source Code
The server's source code is located here: **src/sever/testserver.py**
Let's have a look at the imports:
```python
from flask import Flask
from flask import request
from flask import jsonify
from markupsafe import escape
```
`Flask` is the class that will help us creating the web server. It is a micro web framework written in Python. With `request` we can receive information about the current request which is made by someone connecting to our service. `Jsonify` helps us converting text or datastructures to JSON. JSON is a standard for exchanging information through the web.
`Escape` is just a tool that formats any text into html readable text. We're just using this to prevent bugs.
Next we take a look at the actual code (it's pretty simple):
```python
app = Flask(__name__)

@app.route('/<yourtext>',methods=['GET','POST']) # defining routes
def hello_world(yourtext):
    return jsonify(req_text=escape(yourtext), req_method=request.method)

if __name__ == '__main__':
    app.run(host='0.0.0.0',debug=False)
```
We create our Flask instance and call it app. This object represents the service. Next, we define our routes so what happens if we call something like ```http://<domain>/some-path```. Since we want to allow every route we use a placeholder and call it `<yourtext>`\*. A placeholder is like a variable that contains a requested route. Next, we define a function that gets called when a route is accessed. Every time someone accesses our service the `def hello_world(yourtext)` method gets called where `yourtext` is the route that got called (same name as the placeholder). `hello_world` just returns a JSON text with the request method of the caller and the route that was requested. Since hello_world is a route function (we annotated it with @app.route(...)) every return will not just end the function but will send the returned text as a response back to the caller (client, web browser, etc.). <br />
The last two lines are used for running the service directly. Flask is a decent web server on its own so we can test our program by running 
```python src/server/testserver.py```

![](https://user-images.githubusercontent.com/25117793/90982125-69efc400-e565-11ea-9273-3e098995bda9.png "")
Note that it will warn you that Flask isn't a production server but for the first test, it's sufficient. You can access the web service now by typing `http://<yourserveraddress>:5000/<route>` in your browser. In my case `http://api.fritz.box:5000/this-is-a-route`. The default port to access a Flask server is 5000 so it's important to specify the port in the URL with `:5000`. Later we will use the default http \[https] port 80 \[443]. If you cannot reach the server you need to make sure that your firewall allows port 5000. You can see all allowed ports with `sudo ufw status`. To allow port 5000 enter:
```
sudo ufw allow 5000
```

![](https://user-images.githubusercontent.com/25117793/90982123-68260080-e565-11ea-964c-a647939d0214.png "")
\*Note: We have not defined what happens if we call the root route which is just `/`. In this case the server will respond with an error. If you want to define a root route you need to do add a second route function like:
```python
@app.route('/')
def function_for_root_route():
    return "blablabla"

``` 
So we are done with the server, right? Not really. There are some problems we need to look at. If you just need this server for testing purposes you can skip the following steps, but at the moment our server is highly vulnerable and unstable:
1. The built-in Flask server is slow and scales up poorly since it's not capable of handling multiple connections at once. If we aim to use it in a real-world application we cannot use the Flask server.
2. The connection is not secure. We are using the HTTP protocol which does not encrypt our requests and responses. Today most services even if they don't deal with private data use an encrypted TLS connection via the HTTPS protocol (as long as we're using our service in a local network that's not a huge problem but it still can be a security risk).

In the next 2 steps, we will address these problems.
### Deploy the service using uWSGI and Nginx
This tutorial is not about all the details involving uWSGI and NGINX, but I'll give a short overview of these 2 things.
Nginx is a web server that is next to Apache2 the biggest open source web server in the world. It offers increased security, better performance, encrypted connections, and much more. So Nginx is a great choice, right?

Well, there is a downside. In our case, the problem is Nginx cannot directly talk to a Flask application. So we will use Nginx not as a pure web server but as a [reverse proxy](https://en.wikipedia.org/wiki/Reverse_proxy). It will forward all connections to an application server called uWSGI that can talk to our Flask app over the WSGI protocol. This approach is widely used and gives extra security because of multiple services having to talk to each other.
uWSGI can also be used as a standalone solution but Nginx gives us more possibilities in the future and is very popular so it won't do any harm learning about it.

#### uWSGI
The first step is to set up and configure the uWSGI server. The entry point for uWSGI is defined in **src/server/wsgi.py**:
```python
from testserver import app

if __name__ == '__main__':
	 app.run()
 ```
It's like a second main function that uWSGI uses to run the code in `testserver.py`.

Next, we test our server now by running the following command in our root directory:
```
uwsgi --socket 0.0.0.0:5000 --protocol=http --chdir src/server/ -w wsgi:app
```
Our service should be available under `http://<yourserveraddress>:5000/<route>` but now running on uWSGI instead of the built-in Flask server.

Now we can create a config file that contains all parameters instead of using the command above. My config is in config/api.ini:
```
[uwsgi]
chdir = /home/cedric/c-client-flask-server-test/src/server/ # file to the wsgi.py entrypoint file
wsgi-file = wsgi.py # the file name
callable = app # the name of the Flask app variable
master = true
processes = 5 # multiple workers for better scale
#http = 0.0.0.0:5000 # we'll not use http anymore instead we use a socket
socket = api.sock 
chmod-socket = 660
vacuum = true
 die-on-term = true
 stats=/tmp/stats.sock # here we can read our stats in realtime
```
Instead of exposing uWSGI over HTTP, we want to connect it with an Nginx reverse proxy, which handles all of the outgoing and incoming connections. Therefore we use a Linux Socket `api.sock` that can be shared between our server and the Nginx reverse proxy.
To start the uWSGI app let's create a systemd service file. That allows Linux to automatically run the app. The file needs to be created in **/etc/systemd/system/**. You can find my configuration in **config/api.service**:
```
[Unit]
Description=uWSGI instance to serve my api
After=network.target
[Service]
User=cedric # owner of all files
Group=www-data # so that nginx can access the socket
WorkingDirectory=/home/cedric/c-client-flask-server-test # root dir
Environment="PATH=/home/cedric/c-client-flask-server-test/web_test_env/bin" # where uwsgi is installed (virtualenv)
ExecStart=/home/cedric/c-client-flask-server-test/web_test_env/bin/uwsgi --ini config/api.ini # command we want to execute with api.ini file
[Install]
WantedBy=multi-user.target
``` 
The comments above explain the most important options. If you want to know more you can look up all the other options. <br />
If you want to use this service file you need to change at least the `User` option and copy the file to the right location:
```
sudo cp config/api.service /etc/systemd/system/api.service
```
Now start the service and enable (start on boot) it:
```
sudo systemctl start api.service
sudo systemctl enable api.service
```
With `sudo systemctl status api.service` we can see whether the start was successful or not.
**Note that we cannot reach our server right now because it's no longer exposed to HTTP. The next step will change that.** 

#### Nginx
The uWSGI server is now doing nothing because the socket it listens to is empty. We're going to set up Nginx to forward all requests to the socket from where our application server can handle them.
First, install Nginx and setup firewall rules: 
```
sudo apt-get install nginx 
sudo ufw delete allow 5000
sudo ufw allow 'Nginx Full'
```
Now you should see something when requesting `http://<yourhostname>/`.
Then disable the default site:
```
sudo rm /etc/nginx/sites-enabled/default
sudo systemctl restart nginx
```
Create a new file /etc/nginx/sites-available/api with the following content:
```
server {
    listen 80; # the standard http port
    server_name <your_domain>; # change that

    location / {
	include uwsgi_params;
	uwsgi_pass unix:/home/cedric/c-client-flask-server-test/src/server/api.sock;
    }
}
```
When requesting a `location` the request will be forwarded to the socket where uWSGI takes over. 
Now we create a soft link from sites-available to sites-enabled:
```
sudo ln -s /etc/nginx/sites-available/api /etc/nginx/sites-enabled
sudo systemctl restart nginx 
```
When requesting `http://<yourhostname>/<route>`you should get the same output as before using the Flask server (but without specifying the port number). If you cannot reach your side check the status of the nginx server and see the log: `sudo less /var/log/nginx/error.log`

### Create own CA with self-signed certificates

The server setup is now almost complete. The last step is to establish a secure HTTPS connection. Since we're running the server on our local network we have no public domain name and cannot get a TLS certificate from something like Let's Encrypt but we can self-generate our certificate. <br />
The problem is that web browsers and also our client will try to verify our certificate which will fail. 
Normally certificates get created by an independent CA (certificate authority) and are signed with a private key no one knows. After that, the certificate gets issued to the server that requested it. A CA also has a root certificate that can be used to verify every certificate issued by this CA. The root certificate is shared across all devices and browsers. If an HTTPS connection gets established the server's certificate gets validated by the client using the root certificate of an official CA. 

Our problem is that self-generated certificates are not signed by an official CA, so our connection cannot be trusted. The solution will be to create our own local CA and give our client the root certificate. After that, we can issue a certificate to our server using our CA's private key. This certificate can now be validated by the client since it has the root certificate.

We will store all files related to certificates in **config/cert/** (not sufficient for real production). <br />
First of all, we create a private key:
```
sudo openssl genrsa -des3 -out myApiCA.key 2048
```
Then the root certificate:
```
sudo openssl req -x509 -new -nodes -key myApiCA.key -sha256 -days 365 -out myApiCA.pem
```
The CA is 'ready' now. Next, we create a private key for our service:
```
sudo openssl genrsa -out <your-hostname>.key 2048
```
Now we need to create a CSR which is a certificate signing request. Make sure that the Common Name (CN) is <your-hostname>.

```
sudo openssl req -new -key <your-hostname>.key -out <your-hostname>.csr
```
The CA can issue a server certificate by using the CSR:
```
sudo openssl x509 -req -in mydomain.com.csr -CA myApiCA.pem -CAkey myApiCA.key -CAcreateserial -out <your-hostname>.crt -days 365 -sha256
```
The output is <your-hostname>.crt which is the certificate we can use for the Nginx server. We change the Nginx api file to the following:

```
server {
	listen 80 default_server; 
	server_name <your-hostname>; # change
	return 301 https://<your-hostname>$request_uri; #change
}
	 
server {
	listen 443 ssl http2;
	server_name <your-hostname>; # change
 
	ssl_certificate /home/cedric/c-client-flask-server-test/config/cert/api.crt; # the api certificate
	ssl_certificate_key /home/cedric/c-client-flask-server-test/config/cert/api.key; # the api private key
	ssl_protocols TLSv1.2 TLSv1.1 TLSv1;
	 
	location / {
		include uwsgi_params;
		uwsgi_pass unix:/home/cedric/c-client-flask-server-test/src/server/api.sock;
	}
}
```
We do multiple things here. The first `server` block redirects all HTTP traffic to HTTPS. The second `server` block is the HTTPS configuration and forwards the traffic to our socket. You may need to change the server names and the certificate and certificate-key names.

Let's restart the server:
```
sudo systemctl restart nginx
```
If we now connect to our server it will force us using HTTPS but still gives an error that it cannot check the certificate. This is normal because our browser does not know about our CA but we can install it.

Firefox is a great browser to test if everything works fine. [Here is a quick tutorial on how to install a root certificate](https://portswigger.net/support/installing-burp-suites-ca-certificate-in-firefox). 

`http://<your-domain>/<route>` should be redirected to `https://<your-domain>/<route>` and should also be secure.
Chrome makes more validation tests so even with the root certificate, it will give us an error but that's not a problem since we don't want to use a browser but a c client instead. 

If Firefox says our connection is secure after installing our root certificate we know everything went fine (you can ignore if it says it does not know the CA).
Now we have a quite secure server for testing clients using HTTPS on our local network.

## Write the web client

Finally, it's time to think about the client!

### Curl and Libcurl

Maybe you used cURL before. It's pre-installed on most Linux distros today. cURL implements many different protocols such as FTPS, HTTP, HTTPS, IMAPS, etc. It can be used to make requests or transfer data. If you want to know everything about it see [here](https://curl.haxx.se/). 
We can also make a cURL request to our service:
```
curl http://<your-hostname>/<route>
```
This will result in a `301 Moved Permanently` response because we'll not be redirected to https automatically.

So what happens if we try is using https:
```
curl http://<your-hostname>/<route>
```
Now we're getting something like `curl: (60) SSL certificate problem: unable to get local issuer certificate` because cURL doesn't know our CA certificate. We can add the `-k` option so that the certificate won't be checked but that is not what we want since we already have our own certificate. So let's fix that:
```
curl https://<your-hostname>/<route> --cacert myApiCA.pem
```
This time, everything went fine and we got a response: `{"req_method":"GET","req_text":"<route>"}`.

![](https://user-images.githubusercontent.com/25117793/90991517-0c319b00-e5aa-11ea-9459-12daa420fbd3.png)

As you see cURL is a great and easy tool but how can we write our own client in C. There are a few libraries out there that allow us to use HTTPS like OpenSSL. The downside is OpenSSL can be really complicated because it's a low-level library that needs a lot of boilerplate code in order to work. Luckily there is a great alternative - `libcurl`! It's a programming API that works the same as cURL. Libcurl can also get complex but in this tutorial, we'll just use the single-threaded and synchronous `easy` interface.

### Implement the client

We can install libcurl using:
```
sudo apt install libcurl4-openssl-dev
```
In this example, I just want to send an arbitrary GET request to the server and display the response: `./client <the-path>`
We have to make some imports first.
``` c
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
```
Let's define the domain/hostname of our service we want to talk to. In my case, that's `https://api.fritz.box`.
``` c
const char domain[] = "https://api.fritz.box/";
```
Now, we can enter the main method `int main(int argc, char *argv[]){`:
``` c
if(argc != 2) return 1; // check if argument count matches
unsigned url_len = strlen(domain) + strlen(argv[1]); // get full length of url
char *url; 

if(!(url = malloc(url_len + 1))) return 1; // allocate memory for the full url
strcpy(url,domain); // copy domain/hostname to url
strcat(url,argv[1]); // concat the path to the url
url[url_len] = '\0'; // termination character

printf("\n\rrequest: \t%s\n\rresponse: \t", url); // print the request
```
After the request is printed out we can send a request and wait for the response:
``` c
curl_global_init(CURL_GLOBAL_ALL);	 
CURL *curl = curl_easy_init();

if(curl) { // always check for NULL
	CURLcode res;
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_CAINFO, "/home/progfix/myApiCA.pem");
 
	res = curl_easy_perform(curl);
	if(res != CURLE_OK)
	fprintf(stderr, "error: %s\n", curl_easy_strerror(res));
	 
	curl_easy_cleanup(curl);
}
curl_global_cleanup();
free(url);
```
`curl_global_init` and `curl_easy_init();` will initialize a new curl context. You always need to call these methods before using the libcurl easy-interface. 
`CURLcode res` will be our response code which is `CURLE_OK`
 or 0 if the request was successful.
Then, some important options are made. We set the request URL so curl knows the recipient. The next option is really important. Since we use self-signed certificates we have to say the library where the root cert is located so that curl can validate our service. Otherwise, we'll get an error. With `CURLOPT_CAINFO` we set this option. If you use a certificate that's issued by an official CA you don't need this option. Curl will find the CA's root certificate on your computer like any browser does.

Now it's time to send the request. `res = curl_easy_perform(curl);` will perform our request and set res according to the response. After that, we can check the result and output corresponding errors with `curl_easy_strerror(res)`.

Curl will write the response directly to `stdout`, so we don't need to print anything now. Most of the time we want to store the output instead of printing it so we need to use other options like [CURLOPT_WRITEFUNCTION](https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html).

Finally don't forget to free allocated memory:
```
curl_easy_cleanup(curl);
curl_global_cleanup();
free(url);
```
To compile the code run `gcc src/client/client.c -o client -lcurl`. Now we can run the client: `./client hello-world!`.

![](https://user-images.githubusercontent.com/25117793/91330669-0dd9a980-e7ca-11ea-8a16-2491a7153603.png)

**Now you should have a foundation of how to implement your own client-server application using C and Python. Enjoy!**

