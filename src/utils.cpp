#include "utils.hpp"

TApplication *create_root_app(const std::string &name) {
    int nargs = 2;
    const char *argv[2];
    argv[0] = name.c_str();
    argv[1] = "-b";
    return new TApplication ("ROOTWrapperGenerator", &nargs, const_cast<char **>(argv));
}
