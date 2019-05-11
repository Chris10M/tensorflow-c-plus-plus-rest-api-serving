#include <opencv2/opencv.hpp>
#include <boost/program_options.hpp>
#include<atomic>
#include <future>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <boost/algorithm/string.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp> 
#include <typeinfo>
#include <vector>
#include <string>

#include "pistache/endpoint.h"
#include "json.hpp"
#include "inputvalidator.cpp"
#include "requests.cpp"
#include "app_args_parser.cpp"
#include "logger.hpp"
#include "utils.hpp"
#include "model.cpp"

using namespace Pistache;
using json = nlohmann::json;
using namespace cv;
using namespace std;

struct {
    std::string IpAddress;
    int portAddress;
    int workers;
    int timeout;

    std::string ROUTE = "/test-tensorflow-serving-api";
    Logger logger;

    TensorFlowModel *model;
} APP;

class BadRequest {
    public:
    std::string operator()(std::string msg) {

        json responseJson = {
                {
                    "error",{ 
                                {"msg", msg},
                                {"status", 400},
                            }
                }
        };

        return responseJson.dump();
    }
};

class ResponseFunction{
    public:

    std::pair<int, std::string> get(InputValidator inputValidator){
        cv::Mat image;

        Requests requests;
        if (inputValidator.request_type() == InputValidator::URL) {
            image = requests.downloadImageFromURL(inputValidator.url());
        }
        else {
            image = requests.base64_image2Mat(inputValidator.base64_image());
        }

        if(image.empty()) {
            std::string message;

            if (inputValidator.request_type() == InputValidator::URL)
                message = "Invalid image url";
            else
                message = "Invalid base64 image";

            APP.logger.warn(
                    [=](Log log) mutable { 
                                        log.module_path=__FILE__;
                                        log.function_name="ResponseFunction.get";
                                        log.line_number= std::to_string(__LINE__);
                                        //log.request_id = request_id;
                                        log.message=message;
                                    
                                        return log; 
                                  }(Log())
                            );
            return std::make_pair(0, message);
        }

        try{
            int result = APP.model->infer(APP.logger, image);

            if (result == -1) {
                throw std::runtime_error("Model inference error");
            }

            std::string model_ouput = APP.model->process_result(result);

            json output;
            output["class"] = model_ouput;

            return std::make_pair(1, output.dump());           
        }
        catch(...) {
            APP.logger.error(
                    [=](Log log) mutable { 
                                        log.module_path=__FILE__;
                                        log.function_name="ResponseFunction.get";
                                        log.line_number= std::to_string(__LINE__);
                                        //log.request_id = request_id;
                                        log.message="Model Inference error";
                                    
                                        return log; 
                                  }(Log())
                            );

            return std::make_pair(0, "Invalid image input");
        }
    }
};

namespace postRequest {

void onRequest(const Rest::Request& request, Http::ResponseWriter response)  {
        if(request.method() == Http::Method::Post) {
            json *requestJsonPtr = nullptr;
            std::string request_id = RandomUtils::string(64);

            try {
                requestJsonPtr = new json(json::parse(request.body()));
            }
            catch (...){
                std::string message = "Invalid request json";
                APP.logger.warn(
                    [=](Log log) mutable { 
                                        log.module_path=__FILE__;
                                        log.function_name="postRequest.onRequest";
                                        log.line_number= std::to_string(__LINE__);
                                        log.request_id = request_id;
                                        log.message=message;
                                    
                                        return log; 
                                  }(Log())
                );

                response.send(Pistache::Http::Code::Bad_Request, BadRequest()(message));
                return;
            }

            if (requestJsonPtr->find("requestId") != requestJsonPtr->end()) {
                request_id = (*requestJsonPtr)["requestId"].get<std::string>();
            }

            APP.logger.info(
                    [=](Log log) mutable { 
                                        log.module_path=__FILE__;
                                        log.function_name="postRequest.onRequest";
                                        log.line_number= std::to_string(__LINE__);
                                        log.request_id = request_id;

                                        json outputLogJson;
                                        for (json::iterator it = requestJsonPtr->begin(); it != requestJsonPtr->end(); ++it) {
                                            std::string field = it.value().get<std::string>();

                                            if (field.length() > 250) {
                                                outputLogJson[it.key()] = field.substr(0, 250) + "  ... " + field.substr(field.length() - 5, field.length());
                                            }
                                            else {
                                                outputLogJson[it.key()] = field;
                                            }
                                        }
                                        std::string message = "Request Input: " + outputLogJson.dump();
                                        log.message=message;
                                    
                                        return log; 
                                  }(Log())
                            );

            InputValidator inputValidator(*requestJsonPtr);
            
            if(!inputValidator.valid()) {
                std::string message = "Invalid input parameters";

                APP.logger.warn(
                    [=](Log log) mutable { 
                                        log.module_path=__FILE__;
                                        log.function_name="postRequest.onRequest";
                                        log.line_number= std::to_string(__LINE__);
                                        //log.request_id = request_id;
                                        log.message=message;
                                    
                                        return log; 
                                  }(Log())
                            );
                response.send(Pistache::Http::Code::Bad_Request, BadRequest()(message));
                return;
            }

            json responseJson = {
                {
                    "result",{ 
                                {"msg", "success"},
                                {"status", 200},
                            }
                }
            };

            auto responseAsync = std::async(std::launch::async, &ResponseFunction::get, ResponseFunction(), inputValidator);            
            auto functionTimerCheck = responseAsync.wait_for(std::chrono::seconds(APP.timeout));

            if(functionTimerCheck == std::future_status::timeout) {
                std::string message = "Request has been timeout";

                APP.logger.warn(
                    [=](Log log) mutable { 
                                        log.module_path=__FILE__;
                                        log.function_name="postRequest.onRequest";
                                        log.line_number= std::to_string(__LINE__);
                                        //log.request_id = request_id;
                                        log.message=message;
                                    
                                        return log; 
                                  }(Log())
                            );

                response.send(Pistache::Http::Code::Bad_Request, BadRequest()(message));
                return;
            }
            else {
                std::pair<int,std::string> Outputresponse =  responseAsync.get();
                if(!Outputresponse.first) {
                    APP.logger.warn(
                    [=](Log log) mutable { 
                                        log.module_path=__FILE__;
                                        log.function_name="postRequest.onRequest";
                                        log.line_number= std::to_string(__LINE__);
                                        //log.request_id = request_id;
                                        log.message=Outputresponse.second;
                                    
                                        return log; 
                                  }(Log())
                            );

                    response.send(Pistache::Http::Code::Bad_Request, BadRequest()(Outputresponse.second));
                    return;
                }

                responseJson["result"]["output"] = json::parse(Outputresponse.second);

                APP.logger.info(
                    [=](Log log) mutable { 
                                        log.module_path=__FILE__;
                                        log.function_name="postRequest.onRequest";
                                        log.line_number= std::to_string(__LINE__);
                                        //log.request_id = request_id;
                                        log.message="Output Response: " + responseJson.dump();
                                    
                                        return log; 
                                  }(Log())
                            );  

                Http::Mime::MediaType mime_json(Http::Mime::Type::Application, Http::Mime::Subtype::Json);
                response.send(Pistache::Http::Code::Ok, responseJson.dump(), mime_json);
            }
        }
        else {
            response.send(Pistache::Http::Code::Method_Not_Allowed, "Not Allowed");
        }
        UNUSED(request);
    }
}

int main(int ac, char** av){

    AppArgsParser argsParser(ac, av);

    APP.IpAddress = argsParser.getIpAddress();
    APP.portAddress = argsParser.getPortAddress();
    APP.workers = argsParser.getNumberOfWorkers();
    APP.timeout = argsParser.getTimeoutInSeconds();

    std::vector<std::string> input_nodes{"input_2"};
    std::vector<std::string> output_nodes{"fc1000/Softmax"};

    APP.model = new TensorFlowModel("./model/resnet50_imagenet.pb", input_nodes, output_nodes);

    Rest::Router router;
    Pistache::Address socketAddress(APP.IpAddress, Pistache::Port(APP.portAddress));
    auto opts = Pistache::Http::Endpoint::options()
        .threads(APP.workers)
        .maxPayload(40960000);

    Http::Endpoint server(socketAddress);
    server.init(opts);
    Rest::Routes::Post(router, APP.ROUTE, Rest::Routes::bind(&postRequest::onRequest));
    server.setHandler(router.handler());
    server.serve();
        
    server.shutdown();

    delete APP.model;
    return 0;
}