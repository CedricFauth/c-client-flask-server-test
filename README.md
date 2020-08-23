# A Web Tutorial: C Client and Python Web Server

A Tutorial/Example of how you can write a web client in C that connects to any web server using an encrypted https connection. 
Furthermore, we're going to write a Flask web server and deploy it using Nginx. We'll also generate self-signed certificates for test purposes.

![alt text](https://github.com/CedricFauth/c-client-flask-server-test/blob/master/test.gif "Example Output")

## Contents
* [Important Notes](#Important-Notes)
* [File Structure](#File-Structure)  
* [Building a web server for testing the client](#Build-the-web-server) 
(if you don't need a web server for testing you can jump to [the next step](#Write-the-web-client))
  * [Write a tiny web service with the Python Flask library](#Write-a-tiny-web-service-using-Flask)
  * [Deploy the service using uWSGI and Nginx](#Deploy-the-service-using-uWSGI-and-Nginx)
  * [Create your own CA (Certificate Authority) and generate self-signed certificates for the https connection](#Create-your-own-CA-self-signed-certificates)
* [Write the web client](#Write-the-web-client)
  * [Curl and Libcurl](#Curl-and-Libcurl)
  * [Implement the client](#Implement-the-client)
 * [What's next?](#What-to-do-next)

## Important Notes

* I'm using Linux (Ubuntu) to run the server and the client.
* I'll show you how to create self-signed certificates. If you build a web service for production don't use this method. Instead use a CA like Let's Encrypt (it's also free).
* This tutorial is based on the current repository. The file structure of the repo is explained below. <br />
* There are 2 possible ways you can test out the code/build something upon it:
  1. Downloading this repo (using it as a template) and following the instructions/explanations below to run it (there are some files you need to create for the server such as certificates, otherwise it will not work). I'll explain how to configure everything by using this repo as some kind of template. If you only want the client then you just need to compile the 'client.c' file.
  2. Writing everything by yourself without downloading the repo (more work to do).
* Before you start setting up the server (web service) please think where you want it to run. Later you will need the hostname of the computer where you want to run the server (for configurations and sending requests). If you want it to run on your local machine (same as client) your hostname will be localhost or 127.0.0.1 (ip address). If you want the server to run on a different machine than the client (but still in your home network) you need to use either that machines local ip address or the local hostname. I'll run the server on my homeserver whose hostname is 'api' or 'api.fritz.box' (fritz.box is the local domain of my router which will forward the requests to the api homeserver). 

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
Let's start by writing the web service and test it over a normal web browser. Later we'll connect to it via the C client. My idea of the service is that we can request a certain URL-path and it responds with JSON giving us the request method (e.g. GET) and the specified request path. For example, if the server address is 'localhost' a request like http://localhost/hello-world should give the following response: ``` {method: "GET", path: "hello-world"} ```
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
The server's source code is located here: src/sever/testserver.py
Let's have a look at the imports:
```
from flask import Flask
from flask import request
from flask import jsonify
from markupsafe import escape
```
```Flask``` is the class that will help us creating the web server. With ```request``` we can receive information about the current request which is made by someone connecting to our service. ```Jsonify``` helps us converting text or datastructures to JSON. JSON is a standard for exchanging information through the web.
```Escape``` is just a tool that formats any text into html readable text. We're just using this to prevent bugs.
```
app = Flask(__name__)

@app.route('/<yourtext>',methods=['GET','POST'])
def hello_world(yourtext):
    return jsonify(req_text=escape(yourtext), req_method=request.method)

if __name__ == '__main__':
    app.run(host='0.0.0.0',debug=False)
```

### Deploy the service using uWSGI and Nginx
### Create your own CA self-signed certificates

## Write the web client
### Curl and Libcurl
### Implement the client
## What to do next
