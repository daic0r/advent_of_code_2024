#include <functional>
#include <iostream>
#include <vector>
#include <span>
#include <fstream>
#include <ranges>
#include <string>
#include <map>
#include <sstream>

struct sCoord {
   int x{}, y{};

   constexpr bool operator<(const sCoord& rhs) const {
      if (y < rhs.y)
         return true;
      if (y == rhs.y)
         return x < rhs.x;

      return false;
   } 

   constexpr bool operator==(const sCoord& rhs) const {
      return x == rhs.x and y == rhs.y;
   }

   constexpr sCoord operator+(const sCoord& rhs) const {
      return sCoord{ x + rhs.x, y + rhs.y };
   } 

   constexpr sCoord operator-(const sCoord& rhs) const {
      return sCoord{ x - rhs.x, y - rhs.y };
   } 

   constexpr sCoord operator*(int n) const {
      return sCoord{ x * n, y * n };
   }
};

std::ostream& operator<<(std::ostream& os, const sCoord& c) {
   os << "(" << c.x << ", " << c.y << ")";
   return os;
}

class Keypad {
protected:
   std::map<sCoord, std::function<void()>> mButtons_;
   sCoord armPos_, origArmPos_;

public:
   Keypad(std::function<sCoord(std::map<sCoord, std::function<void()>>&)> initFunc)
      : armPos_{ initFunc(mButtons_) }, origArmPos_{ armPos_ }
   {}

   void move_arm(int x, int y) {
      armPos_ = armPos_ + sCoord{ x, y };
   }

   void press() {
      auto iter = mButtons_.find(armPos_);
      if (iter != mButtons_.end())
         iter->second();
   }
};

class NumericKeypad : public Keypad {
   std::stringstream out_;

public:
   NumericKeypad() : Keypad{ [this](std::map<sCoord, std::function<void()>>& mButtons) {
      mButtons_.emplace(sCoord{ 0, 0 }, [this]() { out_ << "7"; });
      mButtons_.emplace(sCoord{ 1, 0 }, [this]() { out_ << "8"; });
      mButtons_.emplace(sCoord{ 2, 0 }, [this]() { out_ << "9"; });

      mButtons_.emplace(sCoord{ 0, 1 }, [this]() { out_ << "4"; });
      mButtons_.emplace(sCoord{ 1, 1 }, [this]() { out_ << "5"; });
      mButtons_.emplace(sCoord{ 2, 1 }, [this]() { out_ << "6"; });

      mButtons_.emplace(sCoord{ 0, 2 }, [this]() { out_ << "1"; });
      mButtons_.emplace(sCoord{ 1, 2 }, [this]() { out_ << "2"; });
      mButtons_.emplace(sCoord{ 2, 2 }, [this]() { out_ << "3"; });

      mButtons_.emplace(sCoord{ 1, 3 }, [this]() { out_ << "0"; });
      mButtons_.emplace(sCoord{ 2, 3 }, [this]() { out_ << "A"; });

      return sCoord{ 2, 3 };
   } }
   {}

   std::string get_output() const {
      return out_.str();
   }
};

class DirectionalKeypad : public Keypad {
   Keypad& next_;

public:
   DirectionalKeypad(Keypad& next) : next_{ next }, Keypad{[this](std::map<sCoord, std::function<void()>>& mButtons) {
      mButtons_.emplace(sCoord{ 1, 0 }, [this]() { next_.move_arm(0, -1); });
      mButtons_.emplace(sCoord{ 2, 0 }, [this]() { next_.press(); });

      mButtons_.emplace(sCoord{ 0, 1 }, [this]() { next_.move_arm(-1, 0); });
      mButtons_.emplace(sCoord{ 1, 1 }, [this]() { next_.move_arm(0, 1); });
      mButtons_.emplace(sCoord{ 2, 1 }, [this]() { next_.move_arm(1, 0); });

      return sCoord{ 2, 0 };
   } }
   {}
};

long part1(std::span<std::string_view> vCodes) {
   NumericKeypad numpad;
   DirectionalKeypad robot1{ numpad };
   DirectionalKeypad robot2{ robot1 };
   DirectionalKeypad robot3{ robot2 };

   numpad.move_arm(-1, 0);
   numpad.press();
   numpad.move_arm(0, -1);
   numpad.press();
   numpad.move_arm(1, -2);
   numpad.press();
   numpad.move_arm(0, 3);
   numpad.press();

   std::cout << numpad.get_output() << "\n";


   return 42;
}


int main() {
   const std::string strData = "029A\n\
980A\n\
179A\n\
456A\n\
379A";

   auto tmp = strData
      | std::views::split('\n')
      | std::views::transform([](auto rng) { return std::string_view{ rng.begin(), rng.end() }; })
      | std::views::filter([](std::string_view str) { return not str.empty(); });

   std::vector<std::string_view> vCodes{ tmp.begin(), tmp.end() };

   const auto nPart1 = part1(vCodes);
   std::cout << "Result Part 1 = " << nPart1 << "\n";

}
