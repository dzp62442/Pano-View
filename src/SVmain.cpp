#include <SVApp.hpp>

int main(int argc, char* argv[])
{           
    SVLogger::getInstance().setLogLevel(LogLevel::DEBUG);
    SVApp svapp;
    svapp.showPanorama();
    
    
	return 0;

}