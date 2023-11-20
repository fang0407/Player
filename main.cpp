#include "player.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        DEBUG("not support arg");
        return -1;
    }

    std::string file_name = argv[1];

    bool ret = false;

    Player player;
    ret = player.Init(file_name);
    if (!ret) {
        DEBUG("player.Init() failed");
        return -1;
    }

    ret = player.Run();
    if (!ret) {
        DEBUG("player.Run() failed");
        return -1;
    }

    return 0;
}
