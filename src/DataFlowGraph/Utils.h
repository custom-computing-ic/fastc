#ifndef DATAFLOWGRAPH_UTILS_H
#define DATAFLOWGRAPH_UTILS_H_

#define DBG(x) std::cout << #x << ": " << x << std::endl;

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#define foreach_   BOOST_FOREACH
#define foreach_r_ BOOST_REVERSE_FOREACH

#endif // DATAFLOWGRAPH_UTILS_H_
