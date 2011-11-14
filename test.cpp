/* 
 * File:   test.cpp
 * Author: jchen
 *
 * Created on May 2, 2011, 2:47 PM
 */
#include <iostream>
#include <boost/xpressive/xpressive.hpp>

using namespace boost::xpressive;

int main()
{
    std::string hello( "Arc 2336538702 from node 300396225 to node 300396226" );

    sregex rex = sregex::compile( "Arc (\\d+) from node (\\d+) to node (\\d+)" );
    smatch what;

    if( regex_match( hello, what, rex ) )
    {
        std::cout << what[0] << '\n'; // whole match
        std::cout << what[1] << '\n'; // first capture
        std::cout << atoi(what[2].str().c_str()) << '\n'; // second capture
        std::cout << what[3] << '\n'; // second capture
    }

    return 0;
}
