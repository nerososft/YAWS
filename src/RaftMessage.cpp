
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
    const char *RaftMessage::EncodeMessage() {
        char *requestBuffer = (char *) malloc(64 + this->conntentLength * sizeof(char));

        // header
        *requestBuffer = MAGIC_NUMBER;
        requestBuffer++;
        switch (this->type) {
            case Type::RequestVote: {
                *requestBuffer = 1;
                requestBuffer++;
                uint32_t candidateId = this->requestVoteDetails.candidateId;
                uint32_t twoByteCandidateId = ((candidateId << 16) >> 16) | 0xFFFF0000;
                requestBuffer -= 2 * sizeof(char);
                *(uint32_t *) requestBuffer &= twoByteCandidateId;
                requestBuffer += sizeof(uint32_t);
                *(uint32_t *) requestBuffer = this->requestVoteDetails.term;
                requestBuffer += sizeof(uint32_t);
                *(uint32_t *) requestBuffer = this->conntentLength;
            }
                break;
            case Type::RequestVoteResults: {
                *requestBuffer = 2 * sizeof(char);
                requestBuffer++;
                uint32_t voteGranted = this->requestVoteResultsDetails.voteGranted;
                requestBuffer -= 2 * sizeof(char);
                *(uint32_t *) requestBuffer &= voteGranted;
                requestBuffer += sizeof(uint32_t);
                *(uint32_t *) requestBuffer = this->requestVoteResultsDetails.term;
                requestBuffer += sizeof(uint32_t);
                *(uint32_t *) requestBuffer = this->conntentLength;
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
        // X32+X26+X23+X22+X16+X12+X11+X10+X8+X7+X5+X4+X2+X1+1


        // content

        *requestBuffer = '\0';
        return requestBuffer;
    }

    RaftMessage RaftMessage::DncodeMessage(char *const buf) {
        return RaftMessage();
    }
}
