#include <cstdlib>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curl/curl.h>

#include <string>
#include <iostream>
#include <memory>
#include <cstring>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "json.hpp"
#include "base64.h"
#include "logger.hpp"

#define MAX_FILE_LENGTH 20000

using json = nlohmann::json;
#define CV_LOAD_IMAGE_UNCHANGED -1

class WriterMemoryClass
{
    public:
	// Helper Class for reading result from remote host
	WriterMemoryClass()
	{
		this->m_pBuffer = NULL;
		this->m_pBuffer = (char*) malloc(MAX_FILE_LENGTH * sizeof(char));
		this->m_Size = 0;
	};

	~WriterMemoryClass()
	{
		if (this->m_pBuffer)
			free(this->m_pBuffer);
	};

	void* Realloc(void* ptr, size_t size)
	{
		if(ptr)
			return realloc(ptr, size);
		else
			return malloc(size);
	};

	// Callback must be declared static, otherwise it won't link...
	size_t WriteMemoryCallback(char* ptr, size_t size, size_t nmemb)
	{
		// Calculate the real size of the incoming buffer
		size_t realsize = size * nmemb;

		// (Re)Allocate memory for the buffer
		m_pBuffer = (char*) Realloc(m_pBuffer, m_Size + realsize);
        
		// Test if Buffer is initialized correctly & copy memory
		if (m_pBuffer == NULL) {
			realsize = 0;
		}

		memcpy(&(m_pBuffer[m_Size]), ptr, realsize);
		m_Size += realsize;


		// return the real size of the buffer...
		return realsize;
	};


	cv::Mat getMatFromCURLpp() 
	{
        cv::Mat mat = cv::imdecode(cv::Mat(1, m_Size, CV_8UC1, m_pBuffer), CV_LOAD_IMAGE_UNCHANGED);
        return mat;
	}

    std::pair<char*, long> getResponseBuffer() {
        char *buffer = new char[m_Size];
        memcpy((void *)buffer, (void *)m_pBuffer, m_Size);

        return std::make_pair(buffer, m_Size);
    }  

	// Public member vars
	char* m_pBuffer;
	size_t m_Size;
};


class Requests {
    private:
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    public:
    cv::Mat downloadImageFromURL(std::string url) {
        WriterMemoryClass mWriterChunk;
        try
        {
            curlpp::Cleanup cleaner;
            curlpp::Easy request;

            using namespace curlpp::Options;
            using namespace std::placeholders;

            curlpp::types::WriteFunctionFunctor functor = std::bind(&WriterMemoryClass::WriteMemoryCallback, &mWriterChunk, _1, _2, _3);

            curlpp::options::WriteFunction *writeFunction = new curlpp::options::WriteFunction(functor);
            request.setOpt(writeFunction);
            request.setOpt(Url(url));

            request.perform();
        }

        catch ( curlpp::LogicError & e )
        {
            std::cout << e.what() << std::endl;

            return cv::Mat();
        }

        catch ( curlpp::RuntimeError & e )
        {
            std::cout << e.what() << std::endl;

            return cv::Mat();
        }

        catch ( std::runtime_error &e )
        {
            std::cout << e.what() << std::endl;

            return cv::Mat();
        }
        return mWriterChunk.getMatFromCURLpp();
    }

    cv::Mat base64_image2Mat(std::string base64_string) {
        std::string image_data = base64_decode(base64_string);

        std::vector<char> data(image_data.begin(), image_data.end());
        cv::Mat image = cv::imdecode(cv::Mat(data), 1);

        return image;
    }
};
