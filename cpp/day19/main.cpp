#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <vector>
#include <ranges>
#include <fstream>
#include <string>
#include <stack>

using namespace std::literals;

bool is_pattern_possible(std::string_view strPattern, std::span<const std::string_view> vAvail, int nDepth = 0) {
   std::map<std::string, bool> memo;
   std::stack<std::string> s;

   s.push("");
   memo[""] = true;

   bool bdebug = strPattern ==  "bbrgwb";
   
   while (not s.empty()) {
      auto cur = s.top();
      s.pop();

      if (cur == strPattern)
         break;

      for (const auto strAvail : vAvail) {
         if (strAvail.length() + cur.length() > strPattern.length())
            continue;

         auto tmp = strPattern;
         tmp.remove_prefix(strPattern.length() - cur.length());

         std::string strNew = std::string{ strAvail } + std::string{ tmp };
         assert(strNew.length() > tmp.length());
         //std::cout << "Trying " << strNew << "\n";
         
         if (strPattern.ends_with(strNew)) {
            memo[strNew] = true;
            s.push(strNew);
         } else {
            memo[strNew] = false;
         }
      }
   }
   const auto ret = memo[std::string{ strPattern }];
   if (ret) {
      std::cout << strPattern << " be possible\n";
   }
   return ret;
   



   // if (strPattern.empty()) {
   //    std::cout << "FOUND\n";
   //    return true;
   // }
   // 
   // std::cout << "Checking " << strPattern << ", depth = " << nDepth << "\n";
   //
   // for (auto [idx, strAvail] : vAvail 
   //       | std::views::filter([strPattern](std::string_view strAvail) { return strAvail.length() <= strPattern.length(); })
   //       | std::views::enumerate) 
   // {
   //    //std::cout << "Checking " << strPattern << " against " << strAvail << "[" << idx << "]\n";
   //    if (strPattern.starts_with(strAvail)) {
   //       auto strNext = strPattern;
   //       strNext.remove_prefix(strAvail.length());
   //       if (is_pattern_possible(strNext, vAvail, nDepth + 1)) {
   //          return true;
   //       }
   //    }
   // }
   //
   // return false;
}

long part1(std::span<const std::string_view> vAvail, std::span<const std::string_view> vDesired) {
   return std::accumulate(vDesired.begin(), vDesired.end(), 0L,
         [vAvail](long acc, std::string_view strPattern) {
            std::cout << strPattern << "\n";
            return acc + static_cast<long>(is_pattern_possible(strPattern, vAvail));
         });
}

int main() {
//    const std::string strData = "r, wr, b, g, bwu, rb, gb, br\n\
// \n\
// brwrr\n\
// bggr\n\
// gbbr\n\
// rrbgbr\n\
// ubwu\n\
// bwurrg\n\
// brgr\n\
// bbrgwb";

   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   using namespace std::literals;

   std::string_view strAvailPatterns;
   std::string_view strDesiredPatterns;
   {
      auto tmp = strData
         | std::views::split("\n\n"sv)
         | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; });

      auto iter = tmp.begin();
      strAvailPatterns = *iter++;
      strDesiredPatterns = *iter;
   }

   const std::vector<std::string_view> vAvail = std::invoke([strAvailPatterns]() {
      auto tmp = strAvailPatterns
         | std::views::split(", "sv)
         | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
         | std::views::filter([](std::string_view str) { return not str.empty(); });

      return std::vector<std::string_view>{ tmp.begin(), tmp.end() };
   });

   const std::vector<std::string_view> vDesired = std::invoke([strDesiredPatterns]() {
      auto tmp = strDesiredPatterns
         | std::views::split("\n"sv)
         | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
         | std::views::filter([](std::string_view strLine) { return not strLine.empty(); });

      return std::vector<std::string_view>{ tmp.begin(), tmp.end() };
   });

   const auto nPart1 = part1(vAvail, vDesired);
   std::cout << "Result Part 1 = " << nPart1 << "\n";
}
