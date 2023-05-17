#include <fastrtps/log/Log.h>
#include "echo_tool.h"
#include "list_tool.h"


using namespace eprosima::fastrtps;

void print_help(int argc, char** argv)
{
    std::cerr << "Usage1: " << argv[0] << " list [-t] " << std::endl;
    std::cerr << "Usage2: " << argv[0] << " echo topic_name" << std::endl;
}


int main(int argc, char** argv)
{

    if(argc < 2)
    {
        print_help(argc, argv);
        return 1;
    }

    std::string command(argv[1]);
    if (command == "list") 
    {
         
        ListTool mysub;
        if (mysub.init())
        {
            if (argc > 2 && std::string(argv[2]) == "-t")
                mysub.run(1);
            else
                mysub.run(0);
        }
    
    } 
    else if (command == "echo" && argc>2 ) 
    {
        EchoTool tool;
        tool.init();
        for(int i = 2; i < argc; i++)
        {
            std::string topic_name = argv[i];
            tool.subscribe_to_topic(topic_name);
        }
        tool.run();
    }
    else
    {
        print_help(argc, argv);
    }

    

    return 0;
}