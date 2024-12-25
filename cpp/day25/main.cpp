#include <algorithm>
#include <iostream>
#include <vector>
#include <ranges>
#include <string>
#include <fstream>

using namespace std::literals;

struct sKey {
   std::size_t nHeight{};
   std::vector<std::size_t> vColumnHeights; 
};

struct sLock {
   std::size_t nHeight{};
   std::vector<std::size_t> vColumnHeights; 
};

template<typename KeyOrLock>
requires std::is_same_v<KeyOrLock, sKey> or std::is_same_v<KeyOrLock, sLock>
std::ostream& operator<<(std::ostream& os, const KeyOrLock& kl) {
   for (auto [nIdx,n] : kl.vColumnHeights | std::views::enumerate) {
      os << n;
      if (nIdx < kl.vColumnHeights.size()-1)
         std::cout << ","sv;
   }
   return os;
}

long part1(std::span<sKey> vKeys, std::span<sLock> vLocks) {
   long nRet{};
   for (const auto& lock : vLocks) {
      nRet += std::ranges::count_if(vKeys, [&lock](const auto& key) {
         return std::ranges::all_of(std::views::zip(lock.vColumnHeights, key.vColumnHeights),
               [&key](const auto& tup) { return (std::get<0>(tup) + std::get<1>(tup)) <= key.nHeight; });
      });
   } 
   return nRet;
}

int main() {
   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto vLocksAndKeys = strData
      | std::views::split("\n\n"sv)
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::filter([](std::string_view str) { return not str.empty(); });

   std::vector<sKey> vKeys;
   std::vector<sLock> vLocks;

   for (auto strKeyOrLock : vLocksAndKeys) {
      auto tmp = strKeyOrLock
         | std::views::split("\n"sv)
         | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
         | std::views::filter([](std::string_view str) { return not str.empty(); });

      std::vector<std::string_view> vLines{ tmp.begin(), tmp.end() };
      const auto bLock = std::ranges::all_of(vLines.front(), [](auto ch) { return ch == '#'; });
      const auto nSearchDir = bLock ? 1 : -1;

      std::vector<std::size_t> vColHeights;

      for (auto nCol = 0z; nCol < vLines.front().size(); ++nCol) {
         std::size_t nColHeight{};

         std::size_t nSearchRow = bLock ? 1 : vLines.size() - 2;

         while (vLines[nSearchRow][nCol] == '#' and ((nSearchDir == 1 and nSearchRow < vLines.size()) or (nSearchDir == -1 and nSearchRow >= 0))) {
            ++nColHeight;
            nSearchRow += nSearchDir;
         }

         vColHeights.push_back(nColHeight);
      }

      if (bLock) {
         vLocks.emplace_back(vLines.size()-2, std::move(vColHeights));
      } else {
         vKeys.emplace_back(vLines.size()-2, std::move(vColHeights));
      }
   }

   const auto nPart1 = part1(vKeys, vLocks);
   std::cout << "Result Part 1 = " << nPart1 << "\n";

}
