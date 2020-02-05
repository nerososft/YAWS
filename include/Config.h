#ifndef __RAFT_CONFIG_H__
#define __RAFT_CONFIG_H__

#include <map>
#include <ostream>

namespace Raft {
    struct EndPoint {
        std::string host;
        int port;

        friend std::ostream &operator<<(std::ostream &os, const EndPoint &point) {
            os << "{host: " << point.host << ", port: " << point.port << "}";
            return os;
        }
    };

    struct Config {
        std::string id;
        int raftPort;
        int httpPort;
        std::map<unsigned int, EndPoint> endpoints;

        friend std::ostream &operator<<(std::ostream &os, const Config &config) {
            os << "{id: " << config.id << ", raftPort: " << config.raftPort << ", httpPort: " << config.httpPort << ", endpoints: [";
            for (auto it = config.endpoints.begin(); it != config.endpoints.end(); ++it) {
                os << "{nodeId: " << it->first << ", endpoint: " << it->second << "}";
            }
            os << "]}";
            return os;
        }
    };
}
#endif // !__RAFT_CONFIG_H__
