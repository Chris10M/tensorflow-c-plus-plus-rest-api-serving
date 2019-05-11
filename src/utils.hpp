# ifndef H_UTILS
# define H_UTILS

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <random>

class Process {
    public:

    static std::string executeCommand(const char* cmd) {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }
};


class TimeIt {
    std::chrono::steady_clock::time_point begin;

    public:
    TimeIt() {
        begin = std::chrono::steady_clock::now();
    }

    long getDurationInSeconds() {
        std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();

        return std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
    }

    long getDurationInMilliSeconds() {
        std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();

        return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    }

    void resetTimer() {
        begin = std::chrono::steady_clock::now();
    }
};

namespace StringUtils {
    
    std::vector<std::string> split(std::string input_string, char delimiter) {
        std::string temp;
        std::stringstream ss(input_string);

        std::vector<std::string> words;

        while(std::getline(ss, temp, delimiter)){
            words.push_back(temp);
        }
    
        return words;
    }   

}

namespace FileUtils {
    std::vector<std::pair<std::string, std::string>> get_path_list(std::string root_path) {
        
    boost::filesystem::path path(root_path);
 
    boost::filesystem::recursive_directory_iterator itr(path);
    
    std::vector<std::pair<std::string, std::string>> path_list;

    while (itr != boost::filesystem::recursive_directory_iterator())
    {
        std::string path = itr->path().string();
        std::string filename = StringUtils::split(path, '/').back();

        path_list.push_back(std::make_pair(path, filename));
        ++itr;
    }

        return path_list;
    }
}

namespace RandomUtils {

    std::string string(std::string::size_type length)
    {
        static auto& chrs = "0123456789"
                            "abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

        thread_local static std::mt19937 rg{std::random_device{}()};
        thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

        std::string s;

        s.reserve(length);

        while(length--)
            s += chrs[pick(rg)];

        return s;
    }
}

# endif H_UTILS