
//
// Created by XingfengYang on 2020/1/1.
//

#include "../include/RaftMessage.h"
#include <memory>
#include <iostream>

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
    char *RaftMessage::EncodeMessage() {
        char *requestBuffer = (char *) malloc(64 + this->conntentLength * sizeof(char));

        // raft request header
        WriteMem(requestBuffer, 0, MAGIC_NUMBER); // write magic number to first byte
        switch (this->type) {
            case Type::RequestVote: {
                WriteMem(requestBuffer, 1, 1); // write request type to second type

                WriteMem(requestBuffer, 2, (char) (this->requestVoteDetails.candidateId >> 8));
                WriteMem(requestBuffer, 3, (char) (this->requestVoteDetails.candidateId)); // write candidateId to bytes 2-3

                WriteMem(requestBuffer, 4, (char) (this->requestVoteDetails.term >> 24));
                WriteMem(requestBuffer, 5, (char) (this->requestVoteDetails.term >> 16));
                WriteMem(requestBuffer, 6, (char) (this->requestVoteDetails.term >> 8));
                WriteMem(requestBuffer, 7, (char) (this->requestVoteDetails.term)); // write term to bytes 4-7
            }
                break;
            case Type::RequestVoteResults: {
                WriteMem(requestBuffer, 1, 1); // write request type to second type

                WriteMem(requestBuffer, 2, 0);
                WriteMem(requestBuffer, 3, (char) this->requestVoteResultsDetails.voteGranted); // write voteGranted to bytes 2-3

                WriteMem(requestBuffer, 4, (char) (this->requestVoteDetails.term >> 24));
                WriteMem(requestBuffer, 5, (char) (this->requestVoteDetails.term >> 16));
                WriteMem(requestBuffer, 6, (char) (this->requestVoteDetails.term >> 8));
                WriteMem(requestBuffer, 7, (char) (this->requestVoteDetails.term)); // write term to bytes 4-7
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

        WriteMem(requestBuffer, 8, (char) (this->conntentLength >> 24));
        WriteMem(requestBuffer, 9, (char) (this->conntentLength >> 16));
        WriteMem(requestBuffer, 10, (char) (this->conntentLength >> 8));
        WriteMem(requestBuffer, 11, (char) (this->conntentLength)); // write content to bytes 8-11

        // crc
        // X32+X26+X23+X22+X16+X12+X11+X10+X8+X7+X5+X4+X2+X1+1

        // content
        WriteMem(requestBuffer, 64 + conntentLength, '\0'); // write EOF  to last type
        return requestBuffer;
    }

    RaftMessage RaftMessage::DecodeMessage(char *buf) {
        if (this->ReadMem(buf, 0) != MAGIC_NUMBER) {
            throw "Decode: Magic Number check failed.";
        }
        RaftMessage raftMessage;
        switch (this->ReadMem(buf, 1)) {
            case 1: {//Type::RequestVote
                raftMessage.type = Type::RequestVote;
                raftMessage.requestVoteDetails.candidateId = (((uint32_t) this->ReadMem(buf, 2)) << 8) | this->ReadMem(buf, 3);
                raftMessage.requestVoteDetails.term = this->ReadMemU32(buf, 4);
            }
                break;
            case 2: {//Type::RequestVoteResults
                raftMessage.type = Type::RequestVoteResults;
                raftMessage.requestVoteResultsDetails.voteGranted = (((uint32_t) this->ReadMem(buf, 2)) << 8) | this->ReadMem(buf, 3);
                raftMessage.requestVoteResultsDetails.term = this->ReadMemU32(buf, 4);
            }
                break;
            case 3: {//Type::HeartBeat
                raftMessage.type = Type::HeartBeat;
            }
                break;
            case 4: {//Type::LogEntry
                raftMessage.type = Type::LogEntry;
            }
                break;
            default:
                break;
        }
        raftMessage.conntentLength = this->ReadMemU32(buf, 8);


        return raftMessage;
    }


    void RaftMessage::WriteMem(char *mem, uint32_t offset, char value) {
        *(mem + offset) = value;
    }

    char RaftMessage::ReadMem(char *mem, uint32_t offset) {
        return *(mem + offset);
    }

    uint32_t RaftMessage::ReadMemU32(char *mem, uint32_t offset) {
        return (
                ((uint32_t) this->ReadMem(mem, offset)) << 24 |
                ((uint32_t) this->ReadMem(mem, offset + 1)) << 16 |
                ((uint32_t) this->ReadMem(mem, offset + 2)) << 8 |
                ((uint32_t) this->ReadMem(mem, offset + 3))
        );
    }
}
