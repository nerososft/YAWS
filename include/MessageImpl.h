//
// Created by XingfengYang on 2020/1/1.
//

#ifndef RAFT_MESSAGEIMPL_H
#define RAFT_MESSAGEIMPL_H

namespace Raft {
    struct MessageImpl {

        enum class Type {
            Unknown,
            RequestVote,
            RequestVoteResults,
        };

        struct RequestVoteDetails {
            unsigned int term = 0;
            unsigned int candidateId = 0;
        };

        struct RequestVoteResultsDetails {
            unsigned int term = 0;
            bool voteGranted = false;
        };

        Type type = Type::Unknown;

        union {
            RequestVoteDetails requestVoteDetails;
            RequestVoteResultsDetails requestVoteResultsDetails;
        };

        bool isElectionMessage = false;


        MessageImpl() {}
    };
}

#endif //RAFT_MESSAGEIMPL_H
