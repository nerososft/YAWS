//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_TIMEKEEPER_H
#define RAFT_TIMEKEEPER_H

namespace Raft {
    class TimeKeeper {
    public:
        virtual double GetCurrentTime() = 0;
    };
}
#endif //RAFT_TIMEKEEPER_H
