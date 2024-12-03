#include <charconv>
#include <iostream>
#include <regex>
#include <fstream>

std::pair<int, int> parse_match(std::smatch const& match) {
   const std::string strMatchFirst = match[1].str();
   const std::string strMatchSecond = match[2].str();
   int a{}, b{};
   std::from_chars(strMatchFirst.data(), strMatchFirst.data() + strMatchFirst.length(), a);
   std::from_chars(strMatchSecond.data(), strMatchSecond.data() + strMatchSecond.length(), b);

   return { a, b };
}

int part1(std::string strData) {
   std::regex rex{ R"(mul\((\d{1,3}),(\d{1,3})\))" };
   int nSum{};
   for (std::smatch match{}; std::regex_search(strData, match, rex);)
   {
      const auto [a, b] = parse_match(match);

      nSum += a * b;

      strData = match.suffix();
   }
   return nSum;
}

int part2(std::string strData) {
   std::regex rex{ R"(mul\((\d{1,3}),(\d{1,3})\)|do\(\)|don't\(\))" };
   int nSum{};
   int nFactor = 1;
   for (std::smatch match{}; std::regex_search(strData, match, rex);)
   {
      if (match[0] == "do()")
         nFactor = 1;
      else if (match[0] == "don't()")
         nFactor = 0;
      else {
         const auto [a, b] = parse_match(match);
         nSum += nFactor * a * b;
      }
      strData = match.suffix();
   }

   return nSum;
}

int main() {
   std::ifstream f{ "input.txt" };
   //const std::string data{ "xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]then(mul(11,8)mul(8,5))" };
   //const std::string data{ "xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))" };
   const std::string data{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   const auto nPart1 = part1(data);
   const auto nPart2 = part2(data);

   std::cout << "Result Part 1 = " << nPart1 << "\n";
   std::cout << "Result Part 2 = " << nPart2 << std::endl;
}
