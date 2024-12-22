#include <cassert>
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
   Keypad* pPrev_{};

public:
   Keypad(std::function<sCoord(std::map<sCoord, std::function<void()>>&)> initFunc)
      : armPos_{ initFunc(mButtons_) }, origArmPos_{ armPos_ }
   {}

   void move_arm_by(int x, int y) {
      armPos_ = armPos_ + sCoord{ x, y };
   }

   void move_arm_to(sCoord coord) {
      armPos_ = coord;
   }

   void press() {
      auto iter = mButtons_.find(armPos_);
      if (iter != mButtons_.end())
         iter->second();
   }

   sCoord get_offset_to(sCoord coord) const {
      return coord - armPos_;
   }

   std::string get_movement_string(sCoord coord, bool bGapIsAtBottom) const {
      std::stringstream strRet;
      const auto offset = get_offset_to(coord);

      if ((offset.y >= 0 and bGapIsAtBottom) or (offset.y < 0 and not bGapIsAtBottom)) {
         char ch = offset.x < 0 ? '<' : '>';
         for (int x = 0; x < abs(offset.x); ++x)
            strRet << ch;
         ch = offset.y > 0 ? 'v' : '^';
         for (int y = 0; y < abs(offset.y); ++y)
            strRet << ch;
      } else {
         char ch = offset.y > 0 ? 'v' : '^';
         for (int y = 0; y < abs(offset.y); ++y)
            strRet << ch;
         ch = offset.x < 0 ? '<' : '>';
         for (int x = 0; x < abs(offset.x); ++x)
            strRet << ch;
      }
      strRet << 'A';

      return strRet.str();
   }

   Keypad* get_prev_keypad() const {
      return pPrev_;
   }

   void set_prev_keypad(Keypad* pPrev) {
      pPrev_ = pPrev;
   }

   void reset() {
      armPos_ = origArmPos_;
   }

   virtual sCoord get_coord_for_output(char ch) const = 0;
   virtual std::string get_movement_string(sCoord coord) const = 0;
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

   sCoord get_coord_for_output(char ch) const override {
      switch (ch) {
         case '1': return sCoord{ 0, 2 };
         case '2': return sCoord{ 1, 2 };
         case '3': return sCoord{ 2, 2 };
         case '4': return sCoord{ 0, 1 };
         case '5': return sCoord{ 1, 1 };
         case '6': return sCoord{ 2, 1 };
         case '7': return sCoord{ 0, 0 };
         case '8': return sCoord{ 1, 0 };
         case '9': return sCoord{ 2, 0 };
         case '0': return sCoord{ 1, 3 };
         case 'A': return sCoord{ 2, 3 };
      }
      throw std::logic_error{ "Invalid output" };
   }

   std::string get_output() const {
      return out_.str();
   }

   std::string get_movement_string(sCoord coord) const override {
      return Keypad::get_movement_string(coord, true);
   }
};

class DirectionalKeypad : public Keypad {
   Keypad& next_;

public:
   DirectionalKeypad(Keypad& next) : next_{ next }, Keypad{[this, &next](std::map<sCoord, std::function<void()>>& mButtons) {
      mButtons_.emplace(sCoord{ 1, 0 }, [this]() { next_.move_arm_by(0, -1); });
      mButtons_.emplace(sCoord{ 2, 0 }, [this]() { next_.press(); });

      mButtons_.emplace(sCoord{ 0, 1 }, [this]() { next_.move_arm_by(-1, 0); });
      mButtons_.emplace(sCoord{ 1, 1 }, [this]() { next_.move_arm_by(0, 1); });
      mButtons_.emplace(sCoord{ 2, 1 }, [this]() { next_.move_arm_by(1, 0); });

      next.set_prev_keypad(this);
      assert(next.get_prev_keypad() == this);

      return sCoord{ 2, 0 };
   } }
   {}

   sCoord get_coord_for_output(char ch) const override {
      switch (ch) {
         case '^': return sCoord{ 1, 0 };
         case 'v': return sCoord{ 1, 1 };
         case '<': return sCoord{ 0, 1 };
         case '>': return sCoord{ 2, 1 };
         case 'A': return sCoord{ 2, 0 };
      }
      throw std::logic_error{ "Invalid output" };
   }

   std::string get_movement_string(sCoord coord) const override {
      return Keypad::get_movement_string(coord, false);
   }
};

std::string control_robots(Keypad& kp, std::string_view strCode) {
   std::stringstream ss;
   std::cout << strCode << "\n";
   kp.reset();
   for (auto ch : strCode) {
      auto coord = kp.get_coord_for_output(ch);
      auto strMove = kp.get_movement_string(coord);
      kp.move_arm_to(coord);
      ss << strMove;
   } 
   if (kp.get_prev_keypad()) {
      return control_robots(*kp.get_prev_keypad(), ss.str());
   }
   return ss.str();
}

using namespace std::literals;

long part1(std::span<std::string_view> vCodes) {
   NumericKeypad numpad;
   DirectionalKeypad robot1{ numpad };
   DirectionalKeypad robot2{ robot1 };
   numpad.set_prev_keypad(&robot1);
   robot1.set_prev_keypad(&robot2);

   for (const auto strCode : vCodes) {
      const auto strSeq = control_robots(numpad, strCode);
      std::cout << strSeq.length() << "\n";
   }
   // const auto strSeq = control_robots(numpad, "379A"sv);
   // std::cout << strSeq << "\n";
   // std::cout << strSeq.length() << "\n";

   return 42;
}
// ^   A         <<      ^^   A     >>   A        vvv      A
// ^   A         ^^      <<   A     >>   A        vvv      A
//
// <       A >   A <    AAv<AA>>^AvAA^Av<AAA>^A
// v<<A >>^A vA ^A v<<A >>^A A v<A <A >>^AAvAA<^A>Av<A>^AA<A>Av<A<A>>^AAAvA<^A>A
// <v<A >>^A vA ^A <vA <A A >>^A A vA <^A>AAvA^A<vA>^AA<A>A<v<A>A>^AAAvA<^A>A

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
