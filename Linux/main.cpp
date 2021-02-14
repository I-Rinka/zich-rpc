#include <boost/regex.hpp>
#include <iostream>
#include <string>

int main()
{
  std::string line;
  boost::regex pat("^Subject: (Re: |Aw: )*(.*)");
  printf("hello\n");

  while (std::cin)
  {
    printf("hello\n");
    std::getline(std::cin, line);
    boost::smatch matches;
    if (boost::regex_match(line, matches, pat))
      std::cout << matches[2] << std::endl;
    printf("hello\n");
    printf("hello\n");
    printf("hello\n");
  }
}