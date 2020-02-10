//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_MESSAGEIMPL_H
#define RAFT_MESSAGEIMPL_H

#include <memory>
#include <map>

namespace Raft {

    static const char MAGIC_NUMBER = 0b01111110;

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
        uint32_t commissionId = 0;
    };

    class RaftMessageImpl {
    public:
        ~RaftMessageImpl() noexcept;

        RaftMessageImpl(const RaftMessageImpl &) = delete;

        RaftMessageImpl(RaftMessageImpl &&) noexcept;

        RaftMessageImpl &operator=(const RaftMessageImpl &) = delete;

        RaftMessageImpl &operator=(RaftMessageImpl &&) noexcept;

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

    private:
        std::map<int, char*> raftMessageType;

    public:
        char *EncodeMessage();

        std::shared_ptr<RaftMessageImpl> DecodeMessage(char *buf);

       char* getMessageType() const;


    public:
        RaftMessageImpl();
    };
}

#endif //RAFT_MESSAGEIMPL_H
