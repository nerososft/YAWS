
//
// Created by XingfengYang on 2020/1/1.
//

#include "../../include/raft/RaftMessageImpl.h"
#include "../../include/common/Common.h"
#include <memory>
#include <iostream>
#include <map>
#include <string>


namespace Raft {
    RaftMessageImpl::~RaftMessageImpl() noexcept = default;

    RaftMessageImpl::RaftMessageImpl(Raft::RaftMessageImpl &&) noexcept = default;

    RaftMessageImpl &RaftMessageImpl::operator=(RaftMessageImpl &&) noexcept = default;

    RaftMessageImpl::RaftMessageImpl() {
        raftMessageType.insert(std::pair<int, char *>(0, "Unknown"));
        raftMessageType.insert(std::pair<int, char *>(1, "RequestVote"));
        raftMessageType.insert(std::pair<int, char *>(2, "RequestVoteResults"));
        raftMessageType.insert(std::pair<int, char *>(3, "HeartBeat"));
        raftMessageType.insert(std::pair<int, char *>(4, "LogEntry"));
    }


//    char requestType = 0;
//    union {
//        char candidateId[2];
//        char voteGranted[2];
//    };
//    unsigned int term = 0;
//    unsigned int contentSize = 0;
//    char crc[4];
//    char *buf;
    char *RaftMessageImpl::EncodeMessage() {
        char *requestBuffer = (char *) malloc(64 + this->conntentLength * sizeof(char));

        // raft request header
        Common::WriteMem(requestBuffer, 0, MAGIC_NUMBER); // write magic number to first byte
        switch (this->type) {
            case Type::RequestVote: {
                Common::WriteMem(requestBuffer, 1, 1); // write request type to second type

                Common::WriteMem(requestBuffer, 2, (char) (this->requestVoteDetails.candidateId >> 8));
                Common::WriteMem(requestBuffer, 3, (char) (this->requestVoteDetails.candidateId)); // write candidateId to bytes 2-3

                Common::WriteMem(requestBuffer, 4, (char) (this->requestVoteDetails.term >> 24));
                Common::WriteMem(requestBuffer, 5, (char) (this->requestVoteDetails.term >> 16));
                Common::WriteMem(requestBuffer, 6, (char) (this->requestVoteDetails.term >> 8));
                Common::WriteMem(requestBuffer, 7, (char) (this->requestVoteDetails.term)); // write term to bytes 4-7
            }
                break;
            case Type::RequestVoteResults: {
                Common::WriteMem(requestBuffer, 1, 1); // write request type to second type

                Common::WriteMem(requestBuffer, 2, 0);
                Common::WriteMem(requestBuffer, 3, (char) this->requestVoteResultsDetails.voteGranted); // write voteGranted to bytes 2-3

                Common::WriteMem(requestBuffer, 4, (char) (this->requestVoteDetails.term >> 24));
                Common::WriteMem(requestBuffer, 5, (char) (this->requestVoteDetails.term >> 16));
                Common::WriteMem(requestBuffer, 6, (char) (this->requestVoteDetails.term >> 8));
                Common::WriteMem(requestBuffer, 7, (char) (this->requestVoteDetails.term)); // write term to bytes 4-7
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

        Common::WriteMem(requestBuffer, 8, (char) (this->conntentLength >> 24));
        Common::WriteMem(requestBuffer, 9, (char) (this->conntentLength >> 16));
        Common::WriteMem(requestBuffer, 10, (char) (this->conntentLength >> 8));
        Common::WriteMem(requestBuffer, 11, (char) (this->conntentLength)); // write content to bytes 8-11

        // crc
        // X32+X26+X23+X22+X16+X12+X11+X10+X8+X7+X5+X4+X2+X1+1

        // content
        Common::WriteMem(requestBuffer, 64 + conntentLength, '\0'); // write EOF  to last type
        return requestBuffer;
    }

    std::shared_ptr<RaftMessageImpl> RaftMessageImpl::DecodeMessage(char *buf) {
        if (Common::ReadMem(buf, 0) != MAGIC_NUMBER) {
            throw std::logic_error("Decode: Magic Number check failed.");
        }
        std::shared_ptr<RaftMessageImpl> raftMessage = std::make_shared<RaftMessageImpl>();
        switch (Common::ReadMem(buf, 1)) {
            case 1: {//Type::RequestVote
                raftMessage->type = Type::RequestVote;
                raftMessage->requestVoteDetails.candidateId = (((uint32_t) Common::ReadMem(buf, 2)) << 8) | Common::ReadMem(buf, 3);
                raftMessage->requestVoteDetails.term = Common::ReadMemU32(buf, 4);
            }
                break;
            case 2: {//Type::RequestVoteResults
                raftMessage->type = Type::RequestVoteResults;
                raftMessage->requestVoteResultsDetails.voteGranted = (((uint32_t) Common::ReadMem(buf, 2)) << 8) | Common::ReadMem(buf, 3);
                raftMessage->requestVoteResultsDetails.term = Common::ReadMemU32(buf, 4);
            }
                break;
            case 3: {//Type::HeartBeat
                raftMessage->type = Type::HeartBeat;
            }
                break;
            case 4: {//Type::LogEntry
                raftMessage->type = Type::LogEntry;
            }
                break;
            default:
                break;
        }
        raftMessage->conntentLength = Common::ReadMemU32(buf, 8);


        return raftMessage;
    }

    char *RaftMessageImpl::getMessageType() const {
        return raftMessageType.find((int) (type))->second;
    }
}
