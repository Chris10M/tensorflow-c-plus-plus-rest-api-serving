#include "json.hpp"
#include <iostream>
#include <vector>

using json = nlohmann::json;

class InputValidator {
    public:
    const static int BASE64_IMAGE = 1;
    const static int URL = 0;
    
    private:
    json requestJson;
    std::vector<std::string> requestParameters;
    struct {
        std::string url;
        std::string base64_image;

        int type;
        bool valid;
    } requestObject;

    void populateRequestJson(json &requestJson) {
        for(json::iterator it = requestJson.begin(); it != requestJson.end(); ++it) {
            requestParameters.push_back(it.key());
        }
    }
    
    bool validateJson(std::vector<std::string> validParameters) {
        
        for(auto requestKey: validParameters) {
            auto foundIterator = std::find(requestParameters.begin(), requestParameters.end(), requestKey);
            
            if(foundIterator == requestParameters.end())
                return false;
        }
        return true;
    }

    public:
    InputValidator(json &requestJson) {
        std::vector<std::string> validParameters_1{"url"}; 
        std::vector<std::string> validParameters_2{"base64Image"}; 

        populateRequestJson(requestJson);
        requestObject.valid = validateJson(validParameters_1) || validateJson(validParameters_2);
        
        if(requestObject.valid) {
            if (requestJson.find("url") != requestJson.end()) {
                requestObject.type = URL;
                requestObject.url = requestJson["url"].get<std::string>();
            }
            else {
                requestObject.type = BASE64_IMAGE;
                requestObject.base64_image = requestJson["base64Image"].get<std::string>();
            }
        }
    }

    bool valid() {
        return requestObject.valid;
    }

    int request_type() {
        return requestObject.type;
    }

    std::string url() {
        return requestObject.url;
    }

    std::string base64_image() {
        return requestObject.base64_image;
    }
};