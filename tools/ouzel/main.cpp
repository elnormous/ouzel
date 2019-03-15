#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main(int argc, const char* argv[])
{
    enum class Action
    {
        NONE,
        NEW_PROJECT
    };

    Action action = Action::NONE;
    std::string name;
    std::string location;

    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--help")
        {
            std::cout << "Usage:" << std::endl;
            std::cout << argv[0] << " [--help] [--new-project <name>] [--location <location>]" << std::endl;
            return EXIT_SUCCESS;
        }
        else if (std::string(argv[i]) == "--new-project")
        {
            if (++i >= argc)
                throw std::runtime_error("Invalid command");

            action = Action::NEW_PROJECT;
            name = std::string(argv[i]);
        }
        else if (std::string(argv[i]) == "--new-project")
        {
            if (++i >= argc)
                throw std::runtime_error("Invalid command");

            location = std::string(argv[i]);
        }
    }

    try
    {
        if (action == Action::NONE)
            throw std::runtime_error("No action selected");
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
