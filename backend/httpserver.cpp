#include <fstream>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int AiChatWindow::startserver()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    std::ofstream outfile;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    while (true) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        // 读取 HTTP 请求头
        read(new_socket, buffer, 1024);
        std::string request(buffer);

        // 检查是否是文件上传请求
        if (request.find("POST /upload") != std::string::npos) {
            // 保存文件到指定路径
            outfile.open("/home/lllj/Desktop/ollama_qt_cpp/LlamaBerry/received_file", std::ios::binary);
            if (!outfile.is_open()) {
                std::cerr << "Failed to open file for writing" << std::endl;
                close(new_socket);
                continue;
            }

            // 读取文件内容
            int bytes_read;
            while ((bytes_read = read(new_socket, buffer, 1024)) > 0) {
                outfile.write(buffer, bytes_read);
            }

            outfile.close();
            std::cout << "File received and saved" << std::endl;

            // 返回响应
            const char *response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"status\":\"success\"}";
            send(new_socket, response, strlen(response), 0);
        }

        close(new_socket);
    }

    close(server_fd);
    return 0;
}