#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(12345);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        
        return -1;
    }

    if (listen(server_fd, 3) < 0)
    {
        
        return -1;
    }

    std::cout << "Сервер ожидает подключений..." << std::endl;

    while (true)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            std::cerr << "Ошибка при принятии подключения.";
            close(server_fd);
            return -1;
        }

        char buffer[1024] = {0};
        int valread = read(new_socket, buffer, 1024);
        buffer[valread] = '\0';
        int sum = std::atoi(buffer);

        if (valread > 2 && sum % 32 == 0)
        {
            std::cout << "Полученные данные: " << sum << std::endl;
        }
        else
        {
            std::cout << "Ошибка: Длина данных не соответствует требованиям." << std::endl;
        }

        close(new_socket);
    }

    close(server_fd);
    return 0;
}