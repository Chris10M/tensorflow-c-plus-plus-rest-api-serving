# A Simple Tensorflow C++ Rest API Serving

This repo contains the boiler-plate code for serving any tensoflow model(frozen graph). The code base is written on C++.

### Introduction
There may be many repos for python-serving, but there are only a modicum of repos that are based on C++ serving.

The features of the model are summarized below:
+ Tensorflow inference Code.
+ Logging for each request
+ Opencv backed image processing
+ Fast Webserver 


The code is based on [pistache](http://pistache.io/) REST AP server, [OpenCV](https://opencv.org/), [spdlog](https://github.com/gabime/spdlog) Logging, [json](https://github.com/nlohmann/json), [Boost libraries](https://www.boost.org/)

**The model provided is a resnet50 model, trained on imagenet1000.**

### Speed Comparision
+ CPU: Intel i7-8550U CPU @ 1.80GHz
+ RAM: 16GB
 
**Python Implementation**
+ Inference Time: 300 secs/ 500 requests
+ Workers: 6
+ Ram consumption: 1.5GB
    
**C++ Implementation**
+ Inference Time: 275 secs/ 500 requests
+ Workers: 6
+ Ram consumption: 0.5GB
    
### Contents
1. [Installation](#installation)
2. [Download Pre-Trained Model](#download)
2. [Demo](#demo)

### Installation
Run,
```
./dependencies/install_dependencies.sh
```

to install boost, pistache, opencv, and all other dependancies.

Then run,
```
./startup.sh
```
to build and run the webserver.

**To set the ip address and the port number modify the ./startup.sh. and to change the route modify it in src/app.cpp**

### Download
A **Pre-trained test model** is available at this Link [GoogleDrive](https://drive.google.com/open?id=1EC98US1ck0wF0lDE4HFjySxzlWCv75jh), dowload it and place it in models/ path.

### Demo
If you've cloned the repo and then downloaded the pre-trained model, run 
```
./dependencies/install_dependencies.sh
./startup.sh
```

Now you could acess the websever using **POST** Reqeust using the endpoint, 
```
http://0.0.0.0:52118/test-tensorflow-serving-api
```
with content type as "application/json"
```
{
  "url": "some-image-url"
}
```
or if you want to give a base64Image input then,
```
{
  "base64Image": "base64_image_data"
}
```
to facilitate a response.

### Issues
If you encounter any issues, please create an issue tracker.
