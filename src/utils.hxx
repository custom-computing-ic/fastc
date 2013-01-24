#ifndef UTILS_HXX
#define UTILS_HXX

#include <string>

//#define DEBUG

#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif


#define LOG_CERR() (cerr << __FILE__ ": " << __LINE__ << ":")

std::string* get_pragma_param(std::string pragma, std::string param);

#endif
