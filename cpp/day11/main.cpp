#include <charconv>
#include <cmath>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <unordered_map>

std::ostream& operator<<(std::ostream& os, const std::unordered_map<long, std::size_t>& l) {
   for (auto const [key, val] : l) {
      std::cout << key << " => " << val << "\n";
   }
   return os;
}

long get_num_digits(long n) {
   return static_cast<long>(std::log10(n)) + 1;
}

std::pair<long, long> split_number(long n) {
   const auto nNumDigits = get_num_digits(n);
   const auto div = static_cast<long>(std::pow(10, static_cast<long>(std::log10(n)) / 2 + 1));
   return { n / div, n % div };
}

std::unordered_map<long, std::size_t> blink(std::unordered_map<long, std::size_t> const& mInput) {
   // auto iter = lInput.begin();
   // while (iter != lInput.end()) {
   //    if (*iter == 0)
   //       *iter = 1;
   //    else
   //    if (get_num_digits(*iter) % 2 == 0) {
   //       const auto num = *iter;
   //       iter = lInput.erase(iter);
   //       const auto [left, right] = split_number(num);
   //       iter = lInput.insert(iter, right);
   //       iter = lInput.insert(iter, left);
   //       iter = std::next(iter);
   //    } else {
   //       *iter *= 2024;
   //    }
   //
   //    ++iter;
   // }

   auto mRet = mInput;
   for (auto const [nNumber, nCount] : mInput) {
      if (nNumber == 0) {
         mRet[1] += nCount;
      } else 
      if (get_num_digits(nNumber) % 2 == 0) {
         const auto [left, right] = split_number(nNumber);
         mRet[left] += nCount;
         mRet[right] += nCount;
      } else {
         const auto num = nNumber * 2024;
         mRet[num] += nCount;
      }
      mRet[nNumber] -= nCount;
   }

   return mRet;
}

std::size_t solve(std::unordered_map<long, std::size_t> mInput, int nBlinkTimes) {

   for (auto i = 0z; i < nBlinkTimes; ++i) {
      mInput = blink(mInput);
   }

   return std::accumulate(
      mInput.begin(),
      mInput.end(),
      0z,
      [](std::size_t acc, const auto& kvp) {
         return acc + kvp.second;
      }
   );
}

int main() {
   const std::string strData = "28 4 3179 96938 0 6617406 490 816207";

   auto tmp = strData
      | std::views::split(' ')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::transform([](std::string_view strNum) {
         long l{};
         std::from_chars(strNum.begin(), strNum.end(), l);
         return l;
      });

   std::unordered_map<long, std::size_t> mNumbers;
   for (auto n : tmp) {
      ++mNumbers[n]; 
   }

   const auto nPart1 = solve(mNumbers, 25);
   std::cout << "Result Part 1 = " << nPart1 << "\n";
   const auto nPart2 = solve(mNumbers, 75);
   std::cout << "Result Part 2 = " << nPart2 << "\n";
}
