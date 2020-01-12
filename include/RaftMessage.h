//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_MESSAGEIMPL_H
#define RAFT_MESSAGEIMPL_H

#include <memory>

namespace Raft {

    static const char MAGIC_NUMBER = 0b1001101;

    enum class Type {
        Unknown = 0,
        RequestVote = 1,
        RequestVoteResults = 2,
        HeartBeat = 3,
        LogEntry = 4,
    };

    struct RequestVoteHeader {
        uint32_t term = 0;
        uint32_t candidateId = 0;
    };

    struct RequestVoteResultsHeader {
        uint32_t term = 0;
        bool voteGranted = false;
    };

    struct HeartBeatHeader {
        uint32_t term = 0;
    };

    struct LogEntryHeader {
        uint32_t term = 0;
    };

    class RaftMessage {
    public:
        ~RaftMessage() noexcept;

        RaftMessage(const RaftMessage &) = delete;

        RaftMessage(RaftMessage &&) noexcept;

        RaftMessage &operator=(const RaftMessage &) = delete;

        RaftMessage &operator=(RaftMessage &&) noexcept;

    public:
        union {
            RequestVoteHeader requestVoteDetails;
            RequestVoteResultsHeader requestVoteResultsDetails;
            HeartBeatHeader heartBeatDetails;
            LogEntryHeader logEntryDetails;
        };
        Type type = Type::Unknown;
        size_t conntentLength = 0;
        char *content;

    public:
        bool isElectionMessage = false;

    public:
        const char *EncodeMessage();

        RaftMessage DncodeMessage(char *const buf);


    public:
        RaftMessage();
    };
}

#endif //RAFT_MESSAGEIMPL_H
