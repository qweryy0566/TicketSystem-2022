#include <iostream>
#include <string>

#include "../lib/exceptions.hpp"
#include "../lib/utils.hpp"
#include "../src/commands.hpp"

TicketSystem ticket_system;

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

const int MAXL = 1 << 20;
char real_command[MAXL], rev_data[MAXL];
int timestamp = 0;

int main() {
  srand(time(0));
  int slisten = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(7777);               // 7777 端口
  sin.sin_addr.s_addr = htonl(INADDR_ANY);  // 任意链接
  if (bind(slisten, (struct sockaddr *)&sin, sizeof(sin)) == -1)
    printf("bind error!");
  if (listen(slisten, 5) == -1) {
    printf("listen error!");
    return 0;
  }
  int sClient;
  sockaddr_in remoteAddr;
  socklen_t nAddrlen = sizeof(remoteAddr);
  std::string input, output;
  while (1) {
    memset(real_command, 0, sizeof(real_command));
    memset(rev_data, 0, sizeof(rev_data));
    sClient = accept(slisten, (struct sockaddr *)&remoteAddr, &nAddrlen);
    if (!~sClient) {
      printf("Accept Error!");
      continue;
    }
    recv(sClient, rev_data, MAXL, 0);
    input = "[" + std::to_string(timestamp += 2) + "] " + std::string(rev_data);
    printf("> %s\n", input.c_str());
    output = ticket_system.Interprete(input);
    send(sClient, output.c_str(), output.length(), 0);
  }
  return 0;
}