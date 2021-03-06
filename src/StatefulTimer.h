// Copyright Hugh Perkins 2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>

#if (_MSC_VER == 1500) // visual studio 2008
#define MSVC2008
#include <ctime>
#else
#include <chrono>
#endif

#include <vector>
#include <map>
#include <string>

class StatefulTimer {
public:
    static StatefulTimer *instance() {
        static StatefulTimer *_instance = new StatefulTimer();
        return _instance;
    }
    #ifdef MSVC2008
    float last;
    #else
    std::chrono::time_point<std::chrono::high_resolution_clock> last;
    #endif
    std::map< std::string, float > timeByState;
    std::string prefix; // = "";
    StatefulTimer() : prefix("") {
        #ifdef MSVC2008
        time_t thistime;
        time(&thistime);
        last = (float)thistime;
        #else
         last = std::chrono::high_resolution_clock::now();
        #endif
    }
    ~StatefulTimer() {
        std::cout << "StatefulTimer readings:" << std::endl;
        for( std::map< std::string, float >::iterator it = timeByState.begin(); it != timeByState.end(); it++ ) {
            std::cout << "   " << it->first << ": " << it->second << std::endl;
        }
    }
    void _dump(bool force = false) {
        double totalTimings = 0;
        for( std::map< std::string, float >::iterator it = timeByState.begin(); it != timeByState.end(); it++ ) {
//            std::cout << "   " << it->first << ": " << it->second << std::endl;
            totalTimings += it->second;
        }
        if( !force && totalTimings < 800 ) {
            return;
        }
        std::cout << "StatefulTimer readings:" << std::endl;
        for( std::map< std::string, float >::iterator it = timeByState.begin(); it != timeByState.end(); it++ ) {
            if( it->second > 0 ) {
                std::cout << "   " << it->first << ": " << it->second << "ms" << std::endl;
            }
        }
        timeByState.clear();
    }
    static void setPrefix( std::string _prefix ) {
        instance()->prefix = _prefix;
    }
    static void dump(bool force = false) {
        instance()->_dump(force);
    }
    static void timeCheck( std::string state ) {
        instance()->_timeCheck( state );
    }
    void _timeCheck( std::string state ) {
        state = prefix + state;
        #ifdef MSVC2008
        time_t thistime;
        time(&thistime);
        float timemilliseconds = thistime * 1000.0f;
        #else
       std::chrono::time_point<std::chrono::high_resolution_clock> thistime = std::chrono::high_resolution_clock::now();
       std::chrono::duration<float> change = thistime - last;
       float timemilliseconds = static_cast<float>( std::chrono::duration_cast<std::chrono::milliseconds> ( change ).count() );
        #endif
//        if( timeByState.has_key( state ) ) {
            timeByState[state] += timemilliseconds;
//        } else {
//            timeByState[state] = timemilliseconds;
//        }
        #ifdef MSVC2008
        last = (float)thistime;
        #else
        last = thistime;
        #endif
    }
};

