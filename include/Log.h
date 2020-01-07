//
// Created by XingfengYang on 2020/1/7.
//

#ifndef RAFT_LOG_H
#define RAFT_LOG_H

#include <stdio.h>

#define DEBUG 1

#ifdef DEBUG
#define Log(format, ...) do{      \
    printf(format, __VA_ARGS__);    \
                                            \
    } while(0)
#else
#define Log(...) ;
#endif // ! DEBUG

#endif //RAFT_LOG_H
