#include "Visual/Visual.h"
#include <iostream>

int main()
{
    VisualInfo info;
    info.debug  = true;
    info.name   = "SkyLands";
    info.width  = 800;
    info.height = 600;

    try
    {
        Visual vis(info);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}