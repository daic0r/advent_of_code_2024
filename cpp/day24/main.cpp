#include <iostream>
#include <ranges>
#include <vector>
#include <string>
#include <fstream>
#include <functional>
#include <unordered_map>
#include <map>
#include <regex>

using namespace std::literals;

using map_t = std::unordered_map<std::string, std::function<long()>>;

long part1(const map_t& mGates) {
   long nOut{};

   auto bits = mGates
      | std::views::filter([](const auto& kvp) { return kvp.first[0] == 'z'; })
      | std::views::transform([](const auto& kvp) { 
         const auto nBit = std::stoi(kvp.first.data() + 1);
         return kvp.second() << nBit;
      });

   for (const auto bit : bits)
      nOut |= bit;

   return nOut;
}

int main() {
   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   std::string_view strInitialValues, strConn;

   {
      auto tmp = strData
         | std::views::split("\n\n"sv)
         | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
         | std::views::filter([](std::string_view str) { return not str.empty(); });

      auto iter = tmp.begin();
      strInitialValues = *iter++;
      strConn = *iter;
   }

   map_t mGates;

   {
      auto tmp = strInitialValues
         | std::views::split('\n')
         | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
         | std::views::filter([](std::string_view str) { return not str.empty(); })
         | std::views::transform([](auto str) {
            auto tmp = str 
               | std::views::split(": "sv)
               | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; });

            auto iter = tmp.begin();
            std::string_view strWire = *iter++;
            std::string_view strValue = *iter;

            const auto nValue = std::stol(strValue.data());
            
            return std::make_pair(strWire, [nValue]() { return nValue; });
         });

      for (auto&& kvp : tmp)
         mGates.insert(std::move(kvp));
   }

   {
      auto tmp = strConn
         | std::views::split('\n')
         | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
         | std::views::filter([](std::string_view str) { return not str.empty(); })
         | std::views::transform([&mGates](auto str) {
            std::cmatch match;
            const std::regex rex{ R"((.+) (.+) (.+) -> (.+))" };
            std::regex_match(str.begin(), str.end(), match, rex);

            return std::make_pair(match[4], [a=match[1], b=match[3], gate=match[2], &mGates]() {
               const auto nVal1 = mGates[a]();
               const auto nVal2 = mGates[b]();

               if (gate == "AND") {
                  return nVal1 & nVal2; 
               } else 
               if (gate == "OR") {
                  return nVal1 | nVal2;
               } else
               if (gate == "XOR") {
                  return nVal1 ^ nVal2;
               }
               throw std::logic_error{ "Unreachable" };
            });
         });

      for (auto&& kvp : tmp)
         mGates.insert(std::move(kvp));
   }

   const auto nPart1 = part1(mGates);
   std::cout << "Result Part 1 = " << nPart1 << "\n";

}
