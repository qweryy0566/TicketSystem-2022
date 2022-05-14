#include <iostream>
#include <string>

#include "commands.hpp"

TicketSystem ticket_system;

int main() {
  std::ios::sync_with_stdio(0);
  std::cin.tie(0), std::cout.tie(0);
  std::string input, output;
  while (std::getline(cin, input))
    try {
      output = ticket_system.Interprete(input);
      std::cout << output << '\n';
      if (output == "bye") break;
    } catch (Exception *exception) {
#ifdef DEBUG
      cout << exception->what() << '\n';
#endif
    }
  return 0;
}