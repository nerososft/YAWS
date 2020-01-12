//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_ITIMEKEEPER_H
#define RAFT_ITIMEKEEPER_H

#include <memory>

namespace Raft {
    class ITimeKeeper {
    public:
        virtual double GetCurrentTime() = 0;

    private:
    };
}
#endif //RAFT_ITIMEKEEPER_H
