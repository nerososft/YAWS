//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_MESSAGEIMPL_H
#define RAFT_MESSAGEIMPL_H

namespace Raft {
    struct RaftMessageImpl {

        enum class Type {
            Unknown,
            RequestVote,
            RequestVoteResults,
            HeartBeat,
        };

        struct RequestVoteDetails {
            unsigned int term = 0;
            unsigned int candidateId = 0;
        };

        struct RequestVoteResultsDetails {
            unsigned int term = 0;
            bool voteGranted = false;
        };

        struct HeartBeatDetails {
            unsigned int term = 0;
        };

        Type type = Type::Unknown;

        union {
            RequestVoteDetails requestVoteDetails;
            RequestVoteResultsDetails requestVoteResultsDetails;
            HeartBeatDetails heartBeatDetails;
        };

        bool isElectionMessage = false;


        RaftMessageImpl() {}
    };
}

#endif //RAFT_MESSAGEIMPL_H
