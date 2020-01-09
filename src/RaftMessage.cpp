
//
// Created by XingfengYang on 2020/1/1.
//

#include "../include/RaftMessage.h"
#include <memory>

namespace Raft {
    RaftMessage::~RaftMessage() noexcept = default;

    RaftMessage::RaftMessage(Raft::RaftMessage &&) noexcept = default;

    RaftMessage &RaftMessage::operator=(RaftMessage &&) noexcept = default;

    RaftMessage::RaftMessage() {}


//    char requestType = 0;
//    union {
//        char candidateId[2];
//        char voteGranted[2];
//    };
//    unsigned int term = 0;
//    unsigned int contentSize = 0;
//    char crc[4];
//    char *buf;
    const char *RaftMessage::EncodeMessage(const RaftMessage &raftMessage) {
        char *requestBuffer = (char *) malloc(64 + raftMessage.conntentLength * sizeof(char));

        // header
        *requestBuffer = MAGIC_NUMBER;
        requestBuffer++;
        switch (raftMessage.type) {
            case Type::RequestVote: {
                *requestBuffer = 1;
                requestBuffer++;
                unsigned int candidateId = raftMessage.requestVoteDetails.candidateId;
                unsigned int twoByteCandidateId = ((candidateId << 16) >> 16) | 0xFFFFFFFF;
                requestBuffer -= 2;
                *(uint32_t *) requestBuffer &= twoByteCandidateId;
                requestBuffer += 4;
                *(uint32_t *) requestBuffer = raftMessage.requestVoteDetails.term;
                requestBuffer += 4;
                *(uint32_t *) requestBuffer = raftMessage.conntentLength;
            }
                break;
            case Type::RequestVoteResults: {
                *requestBuffer = 2;
                requestBuffer++;
                unsigned int voteGranted = raftMessage.requestVoteResultsDetails.voteGranted;
                unsigned int twoByteCandidateId = ((voteGranted << 16) >> 16) | 0xFFFFFFFF;
                requestBuffer -= 2;
                *(uint32_t *) requestBuffer &= twoByteCandidateId;
                requestBuffer += 4;
                *(uint32_t *) requestBuffer = raftMessage.requestVoteResultsDetails.term;
                requestBuffer += 4;
                *(uint32_t *) requestBuffer = raftMessage.conntentLength;
            }
                break;
            case Type::HeartBeat: {
            }
                break;
            case Type::LogEntry: {
            }
                break;
            case Type::Unknown: {
            }
                break;
            default:
                break;
        }

        // crc

        // content

        *requestBuffer = '\0';
        return requestBuffer;
    }

    RaftMessage RaftMessage::DncodeMessage(char *const buf) {
        return RaftMessage();
    }
}
