#include <iostream>
#include <string>

#include "commands.hpp"

TicketSystem ticket_system;

int main() {
  std::ios::sync_with_stdio(0);
  // std::cin.tie(0), std::cout.tie(0);
  std::string input, output;
  while (std::getline(cin, input)) try {
      output = ticket_system.Interprete(input);
      if (output.length()) {
#ifdef NO_STAMP_
        std::cout << output.substr(output.find(' ') + 1) << '\n';
#else
        std::cout << output << '\n';
#endif
        if (output.substr(output.find(' ')) == " bye") break;
      }
    } catch (const Exception &exception) {
#ifdef DEBUG
      cout << exception.what() << '\n';
#endif
    }
  return 0;
}