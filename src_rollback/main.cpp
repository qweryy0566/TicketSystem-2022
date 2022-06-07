#include <iostream>
#include <string>

#include "../lib/exceptions.hpp"
#include "../lib/utils.hpp"
#include "commands.hpp"

TicketSystem ticket_system;

int main() {
  srand(time(0));
  std::ios::sync_with_stdio(0);
  // std::cin.tie(0), std::cout.tie(0);
  std::string input, output;
  while (std::getline(cin, input)) try {
      output = ticket_system.Interprete(input);
      if (output.length()) {
        std::cout << output << '\n';
        if (output.substr(output.find(' ')) == " bye") break;
      }
    } catch (const Exception &exception) {
      std::cerr << exception.what() << '\n';
    } catch (...) {
      std::cerr << "Invalid format!\n";
    }
  return 0;
}