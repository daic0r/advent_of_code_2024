#include <charconv>
#include <cmath>
#include <iostream>
#include <numeric>
#include <queue>
#include <vector>
#include <ranges>
#include <map>
#include <fstream>
#include <string>
#include <regex>
#include <span>
#include <tuple>

struct sAction {
   char cButton;
   int nMoveX{}, nMoveY{};
   int nTokens{};
   int nActions{};
};

std::ostream& operator<<(std::ostream& os, const sAction& ac) {
   std::cout << "Button: " << ac.cButton << ": X=" << ac.nMoveX << ", Y=" << ac.nMoveY << "\n";
   std::cout << "Tokens: " << ac.nTokens;
   std::cout << "Actions involved: " << ac.nActions;
   return os;
}

struct sMachine {
   std::vector<sAction> vActions{};
   int nTargetX{}, nTargetY{};
};

std::ostream& operator<<(std::ostream& os, const sMachine& mc) {
   for (const auto& ac : mc.vActions) {
      std::cout << ac << "\n";
   }
   std::cout << "Target: X=" << mc.nTargetX << ", Y=" << mc.nTargetY << "\n";
   return os;
}

int length(std::pair<int, int> vec) {
   return vec.first + vec.second;
}


long find_tokens(const sMachine& mach) {
   struct sSearchInfo {
      long nTokens{};
      long nActions{};
   };
   std::map<std::pair<int, int>, sSearchInfo> mCostMap;
   mCostMap.emplace(std::make_pair(0, 0), sSearchInfo{ 0, 0 });

   std::queue<std::pair<int, int>> q;

   q.push(std::make_pair(0, 0));
   
   int i{};

   while (not q.empty()) {
      const auto cur = q.front();
      q.pop();

      if (cur.first > mach.nTargetX or cur.second > mach.nTargetY)
         continue;

      const auto info = mCostMap.at(cur); 
      // if (info.nActions > 100)
      //    continue;
      //std::cout << "At (" << cur.first << ", " << cur.second << "), cost = " << info.nTokens << "\n";

      for (const auto& ac : mach.vActions) {
         const auto nextPos = std::make_pair(cur.first + ac.nMoveX, cur.second + ac.nMoveY);

         //std::cout << "Next -> (" << nextPos.first << ", " << nextPos.second << "), cost = " << nCurCost + ac.nTokens << "\n";

         auto iter = mCostMap.find(nextPos);
         if (iter == mCostMap.end() or iter->second.nActions > ac.nTokens + info.nActions)  {
            q.push(nextPos);
            mCostMap.insert_or_assign(nextPos, sSearchInfo{ ac.nTokens + info.nTokens, ac.nActions + info.nActions });
         }
      }
   }

   auto iter = mCostMap.find(std::make_pair(mach.nTargetX, mach.nTargetY));
   if (iter == mCostMap.end())
      return 0;

   return iter->second.nTokens;
}

long part1(std::span<sMachine> vMachines) {
   return std::accumulate(vMachines.begin(), vMachines.end(), 0L,
         [](long acc, const sMachine& mc) { return acc + find_tokens(mc); });
}


int main() {

   using namespace std::literals;

//    const std::string strData = "Button A: X+94, Y+34\n\
// Button B: X+22, Y+67\n\
// Prize: X=8400, Y=5400\n\
// \n\
// Button A: X+26, Y+66\n\
// Button B: X+67, Y+21\n\
// Prize: X=12748, Y=12176\n\
// \n\
// Button A: X+17, Y+86\n\
// Button B: X+84, Y+37\n\
// Prize: X=7870, Y=6450\n\
// \n\
// Button A: X+69, Y+23\n\
// Button B: X+27, Y+71\n\
// Prize: X=18641, Y=10279";

   std::ifstream f{ "input.txt" };
   const std::string strData{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
   f.close();

   auto tmp = strData
      | std::views::split("\n\n"sv)
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::filter([](std::string_view strMachine) { return not strMachine.empty(); })
      | std::views::transform([](std::string_view strMachine) {
         auto lines = strMachine
            | std::views::split('\n')
            | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
            | std::views::filter([](std::string_view strMachine) { return not strMachine.empty(); });

         const std::regex rex_button{ R"(Button (.): X\+(\d+), Y\+(\d+))" };
         std::smatch match;

         sMachine mach{};

         auto iter = lines.begin();
         for (auto i = 0z; i < 2; ++i) {
            const std::string line1{ *iter++ };
            if (std::regex_match(line1, match, rex_button)) {
               char cButton = match[1].str()[0];
               int nMoveX, nMoveY;
               const std::string strMoveX = match[2].str(), strMoveY = match[3].str();
               std::from_chars(strMoveX.data(), strMoveX.data() + strMoveX.length(), nMoveX);
               std::from_chars(strMoveY.data(), strMoveY.data() + strMoveY.length(), nMoveY);

               sAction ac{ cButton, nMoveX, nMoveY, 1, 1 };
               if (cButton == 'A') {
                  ac.nTokens = 3;
                  mach.vActions.push_back(ac);
               } else {
                  mach.vActions.push_back(ac);
                  auto ac2 = ac;
                  auto ac3 = ac;
                  ac2.nMoveX = ac.nMoveX * 2;
                  ac2.nMoveY = ac.nMoveY * 2;
                  ac2.nTokens = 2;
                  ac2.nActions = 2;
                  ac3.nMoveX = ac.nMoveX * 3;
                  ac3.nMoveY = ac.nMoveY * 3;
                  ac3.nTokens = 3;
                  ac3.nActions = 3;
                  mach.vActions.push_back(ac2);
                  mach.vActions.push_back(ac3);
               }
            }
         }

         const std::regex rex_target{ R"(Prize: X=(\d+), Y=(\d+))" };
         const std::string line_target{ *iter };
         if (std::regex_match(line_target, match, rex_target)) {
            const std::string strTargetX{ match[1].str() };   
            const std::string strTargetY{ match[2].str() };   
            std::from_chars(strTargetX.data(), strTargetX.data() + strTargetX.length(), mach.nTargetX);
            std::from_chars(strTargetY.data(), strTargetY.data() + strTargetY.length(), mach.nTargetY);
         }
         return mach;
      });
   
   std::vector<sMachine> vMachines{ tmp.begin(), tmp.end() };

   const auto nPart1 = part1(vMachines);
   std::cout << "Result Part 1 = " << nPart1 << "\n";

}
