#include <iostream>
#include <iterator>
#include <boost/program_options.hpp>
#include <exception>

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
namespace po = boost::program_options;

class AppArgsParser {
    private:
    int argumentCount;
    char **argumentVector;
    
    int portAddress;
    int workers;
    int timeout;
    std::string ipAddress;
    
    bool parserResultCode;

    private:
    int parseArguments() {
        try {

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("workers", po::value<int>(), "set the number of workers")
            ("timeout", po::value<int>(), "set the timeout in seconds")
            ("ip", po::value<std::string>(), "give the ip, in format ip:port");

        po::variables_map vm;
        po::store(po::parse_command_line(argumentCount, argumentVector, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        }

        if (vm.count("workers")) {
            workers = vm["workers"].as<int>();    
        }
        if (vm.count("timeout")) {
            timeout = vm["timeout"].as<int>();    
        }
        if (vm.count("ip")) {
            std::string socketAddress = vm["ip"].as<std::string>();
            std::string portSeperator = ":";

            auto itr = socketAddress.find(portSeperator);
            if(itr == socketAddress.length()) {
                cout<<"ip address in wrong format.\npass --help for more information";
                return -1;
            }
            else {
                ipAddress =  socketAddress.substr(0, itr);
                portAddress = std::stoi(socketAddress.substr(itr + 1, socketAddress.length()));
            }

        } else {
                cout << "pass the ip address as --ip IPADDRESS:PORT --help for more information.\n";
                return -1;
                }
        }
        catch(exception& e) {
            cerr << "error: " << e.what() << "\n";
            return -1;
        }
        catch(...) {
            cerr << "Exception of unknown type!\n";
        return -1;
        }

        return 0;
    }

    public:
    AppArgsParser(int argumentCount, char **argumentVector) {
        this->argumentCount = argumentCount;
        this->argumentVector = argumentVector;
        workers = 1;
        timeout = 180;
        parserResultCode = !parseArguments();   
    }

    bool isValidParse() {
        return parserResultCode;
    }
    std::string getIpAddress() {
        return ipAddress;
    }

    int getPortAddress() {
        return portAddress;
    }

    int getNumberOfWorkers() {
        return workers;
    }

    int getTimeoutInSeconds() {
        return timeout;
    }
};