// #include "Chat_manager.hpp"
// #include <iostream>
// #include <thread>

// void ReadKey(SharedQueue<std::string>* In)
// {
//     std::string s;
//     while(true)
//     {
//         std::getline(std::cin, s);
//         In->Push(s);
//     }
// }

// int main()
// {
//     int flag;
//     std::cin >> flag;
//     if(flag == 0)
//     {
//         ServerChatManager x("8306");
//         x.Run();
//     }
//     if(flag == 1)
//     {
//         SharedQueue<std::string>* In = new SharedQueue<std::string>();
//         SharedQueue<std::string>* Out = new SharedQueue<std::string>();

//         std::thread T(&ClientChatManager::Run, ClientChatManager("192.168.0.100", "8306", "Kaszka Manna", In, Out));
//         std::thread T2(ReadKey, In);
//         while(true)
//         {
//             std::cout << Out->Pop() << std::endl;
//         }
//     }
// }