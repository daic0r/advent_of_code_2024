#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ranges>

struct sNode {
   std::vector<int> vFileIds{};
   int nCapacity{};
};

std::ostream& operator<<(std::ostream& os, const sNode& node) {
   for (auto i = 0z; i < node.nCapacity; ++i) {
      if (i < node.vFileIds.size())
         std::cout << node.vFileIds[i];
      else
         std::cout << ".";
   }
   return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<sNode>& data) {
   for (const auto& node : data) {
      std::cout << node;
   }
   return os;
}

long calc_checksum(std::span<sNode> vData) {
   long nChecksum{};
   for(auto [idx, val] : vData 
      | std::views::transform([](const auto& node) { return node.vFileIds; })
      | std::views::join
      | std::views::enumerate)
   {
      nChecksum += idx * val;
   }
   
   return nChecksum;
}

long part1(std::vector<sNode> vData) {
   auto iter_left = vData.begin();
   auto iter_right = std::prev(vData.end());
   for (; iter_left < iter_right;) {
      while (not(iter_left->vFileIds.empty() and iter_left->nCapacity > 0)) ++iter_left;
      while (iter_right->vFileIds.empty()) --iter_right;
      if (iter_left >= iter_right)
         break;
      while (iter_left->nCapacity > 0) {
         const auto val = iter_right->vFileIds.back();

         iter_right->vFileIds.pop_back();
         iter_left->vFileIds.push_back(val);

         --iter_left->nCapacity;
         ++iter_right->nCapacity;

         if (iter_right->vFileIds.empty()) {
            while (iter_right->vFileIds.empty()) --iter_right;
            if (iter_left >= iter_right)
               break;
         }
      }
   }
   std::cout << vData << std::endl;

   return calc_checksum(vData);
}

long part2(std::vector<sNode> vData) {
   auto iter_right = std::prev(vData.end());
   for (; iter_right != vData.begin();) {
      while (iter_right->vFileIds.empty()) --iter_right;
      auto iter = vData.begin();
      while ((iter->nCapacity-static_cast<int>(iter->vFileIds.size())) < static_cast<int>(iter_right->vFileIds.size()) and iter < iter_right) {
         ++iter;
      }
      if (iter == iter_right) {
         --iter_right;
         continue;
      }
      std::ranges::copy(iter_right->vFileIds, std::back_inserter(iter->vFileIds));
      iter_right->vFileIds.clear();
   }
      
   for (auto& node : vData)
      node.vFileIds.resize(node.nCapacity);

   return calc_checksum(vData);
}

int main() {
   //const std::string strInput = "2333133121414131402";
   std::ifstream f{ "input.txt" };
   std::string strInput{};
   std::getline(f, strInput);
   f.close();
   std::string result{};

   std::vector<sNode> vData;

   int nCurFileId{};
   for (auto [idx, ch] : strInput | std::views::enumerate) {
      if (idx % 2 == 0) {
         vData.emplace_back(sNode{ std::vector<int>(ch - 48, nCurFileId++), static_cast<int>(ch - 48) });

      } else {
         std::vector<int> vFileIds{};
         vFileIds.reserve(ch - 48);
         vData.emplace_back(sNode{ std::move(vFileIds), static_cast<int>(ch - 48) });
      }
   }

   const auto nPart1 = part1(vData);
   std::cout << "Result Part 1 = " << nPart1 << "\n";
   const auto nPart2 = part2(vData);
   std::cout << "Result Part 2 = " << nPart2 << "\n";

}
