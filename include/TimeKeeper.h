//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_TIMEKEEPER_H
#define RAFT_TIMEKEEPER_H

namespace Raft {
    class TimeKeeper {
        ~TimeKeeper() noexcept;

        TimeKeeper(const TimeKeeper &) = delete;

        TimeKeeper(TimeKeeper &&) noexcept;

        TimeKeeper &operator=(const TimeKeeper &) = delete;

        TimeKeeper &operator=(TimeKeeper &&) noexcept;

    private:

    public:
        TimeKeeper();

        double GetCurrentTime();
    };
}
#endif //RAFT_TIMEKEEPER_H
