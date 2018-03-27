#include "util.h"

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}


bool checkrecord()
{
std::cout << "[FDS::Util] Check record" << std::endl;
std::string testcmd = exec("ps -C gst-launch-1.0");
if(key_search(testcmd, "gst-launch-1.0") >= 1) return true;
else return false;
}

int key_search(const std::string& s, const std::string& key)
{
    int count = 0;
    size_t pos=0;
    while ((pos = s.find(key, pos)) != std::string::npos) {
        ++count;
        ++pos;
    }
    return count;
}
