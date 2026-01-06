#include <iostream>
#include <stdexcept>
#include "Application.h"

int main()
{
    try
    {
        sky::Application App{{"Sky", 800, 600, true}};
        App.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}