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

* I'll show you how to create self-signed certificates. If you build a web service for production don't use this method. Instead use a CA like Let's Encrypt (it's also free).
* This tutorial is based on the current repository. The file structure of the repo is explained below. <br />
* There are 2 possible ways you can test out the code/build something upon it:
  1. Downloading this repo and following the instructions/explanations below to run it (there are some files you need to create for the server such as certificates, otherwise it will not work). If you only want the client then you just need to compile the 'client.c' file.
  2. Just following the instructions below without downloading the repo (more work to do).

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
### Write a tiny web service using Flask
### Deploy the service using uWSGI and Nginx
### Create your own CA self-signed certificates

## Write the web client
### Curl and Libcurl
### Implement the client
## What to do next
