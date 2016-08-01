/*
* Author: Shao Yingxia
* Create Date: 2012年12月09日 星期日 23时41分57秒
*/
#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <iostream>
#include <stdio.h>

using namespace std;

#define LOG_FATAL 3
#define LOG_WARNING 2
#define LOG_INFO 1
#define LOG_DEBUG 0

#ifndef LOG_LEVEL
#define LOG_LEVEL 0
#endif

#define Logger(loglevel, sformat, ...) \
    if(loglevel >= LOG_LEVEL) { \
        cout << __FILE__<<"("<< __func__ <<":"<<__LINE__<<"): "; \
        printf(sformat,##__VA_ARGS__); \
        printf("\n"); \
    }

#endif    // #ifndef __LOGGER_HPP__

