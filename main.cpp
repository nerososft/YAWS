#include <sstream>
#include "include/Bootstrap.h"
#include "test.cpp"

int main(int argc, char *argv[]) {
    test();
    auto *bootstrap = new Bootstrap::RaftBootstrap();
    bootstrap->Run(argc, argv);
}
