#ifndef __RAFT_CONFIG_H__
#define __RAFT_CONFIG_H__

#include <vector>

namespace Raft {
    struct EndPoint {
        char *host;
        int port;
    };

    struct Config {
        char *id;
        std::vector<EndPoint> endpoints;
    };
}
#endif // !__RAFT_CONFIG_H__
