#include "precompiled.hxx"
#include "utils.hxx"

std::string* get_pragma_param(std::string pragma, std::string param) {
    boost::regex s(param + " : (\\w*)");
    boost::cmatch sm;
    if ( regex_search(pragma.c_str(), sm, s) )
        return new std::string(sm[1]);
    return NULL;
}
