#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <thread>
#include <limits>
#include <array>

#include <cassert> // for test
#include <cmath> // for test
#include <sstream> // for test

#define DEBUG 0

std::ostream& operator<<(std::ostream& os, const tm& tm_t) {
  std::cout << "second: " << tm_t.tm_sec << std::endl;
  std::cout << "min: " << tm_t.tm_min << std::endl;
  std::cout << "hour: " << tm_t.tm_hour << std::endl;
  std::cout << "month: " << tm_t.tm_mon << std::endl;
  std::cout << "year: " << tm_t.tm_year << std::endl;
  std::cout << "day of week: " << tm_t.tm_wday << std::endl;
  return os;
}

class CronTab {

private:
  int64_t _start_time;

public:

  CronTab() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    _start_time = 
      std::chrono::duration_cast<
        std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "start time: " << _start_time << std::endl;
    std::cout << std::asctime(std::localtime(reinterpret_cast<time_t*>(&_start_time))) << std::endl;
  }

  CronTab(int64_t start_time) : _start_time(start_time) {
    std::cout << "start time: " << _start_time << std::endl;
    std::cout << std::asctime(std::localtime(reinterpret_cast<time_t*>(&_start_time))) << std::endl;
  }


  struct CronField {
    std::string str;
    int value;
    bool is_cyclic;

    friend std::ostream& operator<<(std::ostream& os, const CronField& cron) {
      os << "str: " << cron.str
        << " value: " << cron.value
        << " is_cyclic: " << cron.is_cyclic;
      return os;
    }
  };

  constexpr static int CRON_FIELD_NUM = 7;
  //                                           s   M   H   d   m   w     y
  constexpr static int CRON_FIELD_MAX_VAL[] = {60, 60, 24, 31, 12, 7, std::numeric_limits<int>::max()};

  int parse_cron(std::string cron_str, std::vector<CronField>* cron) {
    cron->clear();

    // split str
    while (true) {
      auto pos = cron_str.find(" ");
      if (pos == std::string::npos) {
        if (!cron_str.empty()) {
          cron->push_back({cron_str, 0, 0});
        }
        break;
      }
      cron->push_back({cron_str.substr(0, pos), 0, 0});
      cron_str = cron_str.substr(pos + 1);
    }
    if (cron->size() != CRON_FIELD_NUM) {
      return -1;
    }
    if (CRON_FIELD_NUM != sizeof(CRON_FIELD_MAX_VAL) / sizeof(int)) {
      return -1;
    }

    // extract value
    auto& cron_ref = *cron;
    for (size_t i = 0; i < cron_ref.size(); ++i) {
      std::string val_str = cron_ref[i].str;
      auto pos = val_str.find("/");
      if (pos != std::string::npos) {
        val_str = val_str.substr(pos + 1);
        cron_ref[i].is_cyclic = true;
      }
      if (val_str == "*") {
        cron_ref[i].value = -1;
        continue;
      }
      int base = 1;
      int value = 0;
      for (auto it = val_str.rbegin(); it != val_str.rend(); ++it) {
        if (std::isalnum(*it) && !std::isalpha(*it)) {
          value += (*it - '0') * base;
        } else {
          return -2;
        }
        base *= 10;
      }
      if (cron_ref[i].is_cyclic) {
        if (value < 1) {
          value = 1;
        }
      } else {
        if (value >= CRON_FIELD_MAX_VAL[i]) {
          return -1; // out of bound
        }
      }
      cron_ref[i].value = value;
    }
    return 0;
  }

  enum MatchDirection {
    LAST_MATCH = -1,
    NEXT_MATCH = 1,
  };

  int64_t match_time(int64_t ts, const std::string& cron_str,
      const MatchDirection& md) {

#if DEBUG
    auto __start_time__ = std::chrono::system_clock::now().time_since_epoch();
#endif // debug

    std::vector<CronField> cron;
    if (parse_cron(cron_str, &cron)) {
      return -1;
    }
    if (ts < _start_time) {
      return -1;
    }
    time_t t = static_cast<time_t>(ts);
    std::tm local_tm;
    // compensation for time zone
    if (::localtime_r(&t, &local_tm) == nullptr) {
      return -1;
    }
    std::tm start_tm;
    time_t start_ts = _start_time;
    if (::localtime_r(&start_ts, &start_tm) == nullptr) {
      return -1;
    }
    local_tm.tm_year += 1900; // compensation
    start_tm.tm_year += 1900; // compensation

  //   std::cout << "ts: " << t << std::endl
  //     << "tm: " << local_tm << std::endl;
    // match each field
    std::vector<bool> match(cron.size(), false);
    int index = 0;
    time_t tmp = 0;
    bool cyclic_taken = false;

    // second
    while (true) {
  //     std::cout << "match second: " << std::endl;
      if (cron[index].is_cyclic) {
        cyclic_taken = true;
        tmp = t - start_ts;
        while (tmp % cron[index].value) {
          tmp += md * 1;
        }
        match[index] = true;
        t = start_ts + tmp;
      } else if (cron[index].value == -1
          || cron[index].value == local_tm.tm_sec) {
        match[index] = true;
      }
      if (match[index]) {
        break;
      }
      t += md * 1; // seek
      if (::localtime_r(&t, &local_tm) == nullptr) {
        return -1;
      }
    }
    ++index;

    // minute
    if (::localtime_r(&t, &local_tm) == nullptr) {
      return -1;
    }
    while (true) {
  //     std::cout << "match minute: " << std::endl;
      if (cron[index].is_cyclic) {
        if (cyclic_taken) {
          match[index] = true;
          break;
        }
        cyclic_taken = true;
        tmp = (t - start_ts) / 60 * 60;
        while ((tmp / 60) % cron[index].value) {
          tmp += md * 60;
        }
        match[index] = true;
        auto compensation = (t - start_ts + 60) % 60;
        t = start_ts + tmp + compensation;
      } else if (cron[index].value == -1
          || cron[index].value == local_tm.tm_min) {
        match[index] = true;
      }
      if (match[index]) {
        break;
      }
      t += md * 60; // seek
      if (::localtime_r(&t, &local_tm) == nullptr) {
        return -1;
      }
    }
    ++index;

    // hour
    if (::localtime_r(&t, &local_tm) == nullptr) {
      return -1;
    }
    while (true) {
  //     std::cout << "match hour: " << std::endl;
      if (cron[index].is_cyclic) {
        if (cyclic_taken) {
          match[index] = true;
          break;
        }
        cyclic_taken = true;
        tmp = (t - start_ts) / 3600 * 3600;
        while ((tmp / 3600) % cron[index].value) {
          tmp += md * 3600;
        }
        match[index] = true;
        auto compensation = (t - start_ts + 3600) % 3600;
        t = start_ts + tmp + compensation;
      } else if (cron[index].value == -1
          ||cron[index].value == local_tm.tm_hour) {
        match[index] = true;
      }
      if (match[index]) {
        break;
      }
      t += md * 3600; // seek
      if (::localtime_r(&t, &local_tm) == nullptr) {
        return -1;
      }
    }
    ++index;

    // day of month
    while (true) {
  //     std::cout << "match day of month: " << std::endl;
      if (cron[index].is_cyclic) {
        if (cyclic_taken) {
          match[index] = true;
          break;
        }
        cyclic_taken = true;
        tmp = (t - start_ts) / 86400 * 86400;
        while ((tmp / 86400) % cron[index].value) {
          tmp += md * 86400;
        }
        match[index] = true;
        auto compensation = (t - start_ts + 86400) % 86400;
        t = start_ts + tmp + compensation;
      } else if (cron[index].value == local_tm.tm_mday
          || cron[index].value == -1) {
        match[index] = true;
      }
      if (match[index]) {
        break;
      }
      t += md * 86400; // seek
      if (::localtime_r(&t, &local_tm) == nullptr) {
        return -1;
      }
    }
    ++index;

    // month, cyclic match not supported
    while (true) {
  //     std::cout << " match month: " << local_tm.tm_mon << std::endl;
      if (cron[index].value == local_tm.tm_mon
          || cron[index].value == -1) {
        match[index] = true;
      } 
      if (match[index]) {
        break;
      }
      t += md * 86400; // seek
      if (::localtime_r(&t, &local_tm) == nullptr) {
        return -1;
      }
    }
    ++index;

    // day of week,cyclic match not supported
    while (true) {
  //     std::cout << "match day of week: " << std::endl;
      if (cron[index].value == local_tm.tm_wday
          || cron[index].value == -1) {
        match[index] = true;
      }
      if (match[index]) {
        break;
      }
      t += md * 86400; // seek
      if (::localtime_r(&t, &local_tm) == nullptr) {
        return -1;
      }
    }
    ++index;

    // year, cyclic match may not be supported, seems meaningless
    if (::localtime_r(&t, &local_tm) == nullptr) {
      return -1;
    }
    local_tm.tm_year += 1900;
  //   std::cout << "match year: " << local_tm.tm_year << std::endl;
    while (true) {
      if (cron[index].is_cyclic) {
    //       std::cout << "cyclic match year: " << local_tm.tm_year << std::endl;
        while ((local_tm.tm_year + start_tm.tm_year) % cron[index].value) {
          local_tm.tm_year += md * 1;
        }
        match[index] = true;
      } else if (cron[index].value == local_tm.tm_year
          || cron[index].value == -1) {
        match[index] = true;
      } else {
        break;
      }
      if (match[index]) {
        break;
      }
    }
    if (!match[index]) {
      return -3; // not found
    }
    local_tm.tm_year -= 1900;
    t = ::timelocal(&local_tm);

#if DEBUG
    auto __time_end__ = std::chrono::system_clock().now().time_since_epoch();
    std::chrono::microseconds __elpased__ = __time_end__ - __start_time__;
    std::cout << "elapsed: " << __elpased__.count() << " us" << std::endl;
#endif // debug

    return t;
  }

  int64_t match_time(const std::string& cron_str, const MatchDirection& md) {
    return match_time(std::time(nullptr), cron_str, md);
  }

};

void test_parse() {
  //                         s M H D m w Y
  // std::string cron_str = "* * * * * * *";
  // std::string cron_str = "* */20 10/2 */3 */4 2016 *";
  std::string cron_str = "* * 10/2 */3 */4 * 2016";
  CronTab ct;
  std::vector<CronTab::CronField> cron;
  int ret = ct.parse_cron(cron_str, &cron);
  if (ret != 0) {
    std::cerr << "parse error" << std::endl;
  }
  for (auto i : cron) {
    std::cout << i << std::endl;
  }
}

void test_match_time() {
  // std::string cron_str = "*/3 * * * * * *";
  CronTab ct(1478240146);
  // auto t = std::time(nullptr);
  decltype(std::time(nullptr)) t = 1478240146; // 2016-11-04-Friday 14:15:46 CST
  decltype(t) last;
  tm last_tm;
  tm cur_tm;
  tm start_tm;
  ::localtime_r(&t, &start_tm);
  std::stringstream cron_str("*/3 * * * * * *");
  std::vector<CronTab::MatchDirection> match_directions {
      CronTab::MatchDirection::NEXT_MATCH,
      CronTab::MatchDirection::LAST_MATCH,
    };

  // second
  for (auto it = match_directions.begin();
        false && it != match_directions.end();
        ++it) {
    auto& match_direction = *it;
    std::cout << "================ direction: " << match_direction << std::endl;
    for (int test_val = 1; test_val < 100; ++test_val) {
      // cron_str = "*/100 * * * * * *";
      cron_str.str("");
      cron_str << "*/" << test_val << " * * * * * *";
      last = std::numeric_limits<decltype(last)>::max();
      std::cout << "test second cyclic: " << cron_str.str() << std::endl;
      for (int i = 0; i < 3000; ++i) {
        time_t cur = ct.match_time(t + i, cron_str.str(), match_direction);
        if (last == cur) {
          continue;
        }
        if (i == 0) {
          assert(cur == t);
        }
        if (i != 0) {
          assert(cur - last == test_val);
        }
        last = cur;
#if DEBUG
        std::string time_str = std::asctime(std::localtime(&cur));
        std::cout << time_str.substr(0, time_str.length() - 1)
          << " " << cur << std::endl;
#endif // debug
      }
    }

    last = std::numeric_limits<decltype(last)>::max();
    for (int test_val = 0; test_val < 60; ++test_val) {
      // cron_str = "0 * * * * * *";
      cron_str.str("");
      cron_str << test_val << " * * * * * *";
      std::cout << "test second: " << cron_str.str() << std::endl;
      for (int i = 0; i < 3000; ++i) {
        time_t cur = ct.match_time(t + i, cron_str.str(), match_direction);
        if (last == cur) {
          continue;
        }
        if (i == 0) {
          ::localtime_r(&cur, &cur_tm);
          if (match_direction == CronTab::MatchDirection::LAST_MATCH) {
            if (start_tm.tm_sec >= test_val) { // minute not changed
              assert(start_tm.tm_min == cur_tm.tm_min);
            } else { // find backwards for one minute
              assert(start_tm.tm_min == cur_tm.tm_min + 1);
            }
          }
          if (match_direction == CronTab::MatchDirection::NEXT_MATCH) {
            if (start_tm.tm_sec <= test_val) { // minute not changed
              assert(start_tm.tm_min == cur_tm.tm_min);
            } else { // find forwards for one minute
              assert(start_tm.tm_min == cur_tm.tm_min - 1);
            }
          }
          assert(cur_tm.tm_sec == test_val);
        }
        if (i != 0) {
          assert(cur - last == 60);
        }
        last = cur;
#if DEBUG
        std::string time_str = std::asctime(std::localtime(&cur));
        std::cout << time_str.substr(0, time_str.length() - 1)
          << " " << cur << std::endl;
#endif // debug
      }
    }
  }

  // minute
  for (auto it = match_directions.begin();
        false && it != match_directions.end();
        ++it) {
    auto& match_direction = *it;
    std::cout << "================ direction: " << match_direction << std::endl;
    for (int test_val = 1; test_val < 110; ++test_val) {
      // cron_str = "*/100 * * * * * *";
      cron_str.str("");
      cron_str << "0 */" << test_val << " * * * * *";
      last = std::numeric_limits<decltype(last)>::max();
      std::cout << "test minute cyclic: " << cron_str.str() << std::endl;
      for (int i = 0; i < 3000; ++i) {
        time_t cur = ct.match_time(t + i, cron_str.str(), match_direction);
        if (last == cur) {
          continue;
        }
        ::localtime_r(&cur, &cur_tm);
        assert(cur_tm.tm_sec == 0);
        if (i == 0) { // first match
          if (match_direction == CronTab::MatchDirection::LAST_MATCH) {
            if (start_tm.tm_sec >= 0) { // minute not changed
              assert(t - cur < 60);
            } else { // find backwards
              assert(t - cur >= test_val * 60 && t - cur < (test_val + 1) * 60);
            }
          }
          // find find forward
          if (match_direction == CronTab::MatchDirection::NEXT_MATCH) {
            assert(cur - t >= 0 && cur - t <= test_val * 60);
          }
        }
        if (i != 0) {
          assert(cur - last == test_val * 60);
        }
        last = cur;
#if DEBUG
        std::string time_str = std::asctime(std::localtime(&cur));
        std::cout << time_str.substr(0, time_str.length() - 1)
          << " " << cur << std::endl;
#endif // debug
      }
    }

    last = std::numeric_limits<decltype(last)>::max();
    for (int test_val = 0; test_val < 60; ++test_val) {
      // cron_str = "0 * * * * * *";
      cron_str.str("");
      cron_str << "59 " << test_val << " * * * * *";
      std::cout << "test minute: " << cron_str.str() << std::endl;
      for (int i = 0; i < 3000; ++i) {
        time_t cur = ct.match_time(t + i, cron_str.str(), match_direction);
        if (last == cur) {
          continue;
        }
        // std::cout << cur << std::endl;
        ::localtime_r(&cur, &cur_tm);
        assert(cur_tm.tm_sec == 59);
        assert(cur_tm.tm_min == test_val);
        if (i == 0) {
          ::localtime_r(&cur, &cur_tm);
          if (match_direction == CronTab::MatchDirection::LAST_MATCH) {
            if (start_tm.tm_min > test_val) { // hour not changed
              assert(start_tm.tm_hour == cur_tm.tm_hour);
            } else { // find backwards for one hour
              assert(start_tm.tm_hour == cur_tm.tm_hour + 1);
            }
          }
          if (match_direction == CronTab::MatchDirection::NEXT_MATCH) {
            if (start_tm.tm_min <= test_val) { // hour not changed
              assert(start_tm.tm_hour == cur_tm.tm_hour);
            } else { // find forwards for one hour
              assert(start_tm.tm_hour == cur_tm.tm_hour - 1);
            }
          }
        }
        if (i != 0) {
          assert(cur - last == 3600);
        }
        last = cur;
#if DEBUG
        std::string time_str = std::asctime(std::localtime(&cur));
        std::cout << time_str.substr(0, time_str.length() - 1)
          << " " << cur << std::endl;
#endif // debug
      }
    }
  }


  // hour
  if (false) {
    last = -1;
    cron_str.str("");
    cron_str << "0 16 */3 * * * *";
    std::cout << "test hour cyclic: " << cron_str.str() << std::endl;
    for (int i = 0; i < 24 * 3600; i += 60 * 10) {
      // auto cur = ct.match_time(t + i, cron_str.str(), CronTab::MatchDirection::NEXT_MATCH);
      auto cur = ct.match_time(t + i, cron_str.str(), CronTab::MatchDirection::LAST_MATCH);
      if (last == cur) {
        continue;
      }
      last = cur;
      std::cout << cur << std::endl;
    }

    last = -1;
    cron_str.str("");
    cron_str << "0 5 3 * * * *";
    std::cout << "test hour: " << cron_str.str() << std::endl;
    for (int i = 0; i < 24 * 3600; i += 60 * 10) {
      auto cur = ct.match_time(t + i, cron_str.str(), CronTab::MatchDirection::NEXT_MATCH);
      if (last == cur) {
        continue;
      }
      last = cur;
      std::cout << cur << std::endl;
    }
  }

  // day
  if (false) {
    last = -1;
    cron_str.str("");
    cron_str << "0 5 1 */30 * * *";
    std::cout << "test day of month cyclic: " << cron_str.str() << std::endl;
    for (int i = 0; i < 365 * 86400; i += 86400) {
      auto cur = ct.match_time(t + i, cron_str.str(), CronTab::MatchDirection::NEXT_MATCH);
      if (last == cur) {
        continue;
      }
      last = cur;
      std::cout << cur << std::endl;
    }

    last = -1;
    cron_str.str("");
    cron_str << "0 5 1 30 * * *";
    std::cout << "test day of month: " << cron_str.str() << std::endl;
    for (int i = 0; i < 365 * 86400; i += 86400) {
      auto cur = ct.match_time(t + i, cron_str.str(), CronTab::MatchDirection::NEXT_MATCH);
      if (last == cur) {
        continue;
      }
      last = cur;
      std::cout << cur << std::endl;
    }
  }

  // month
  if (false) {
    last = -1;
    cron_str.str("");
    cron_str << "0 5 1 15 10 * *";
    std::cout << "test month: " << cron_str.str() << std::endl;
    std::cout << ct.match_time(t, cron_str.str(), CronTab::MatchDirection::NEXT_MATCH) << std::endl;
    for (int i = 0; i < 365 * 86400; i += 86400 * 30) {
      auto cur = ct.match_time(t + i, cron_str.str(), CronTab::MatchDirection::NEXT_MATCH);
      if (last == cur) {
        continue;
      }
      last = cur;
      std::cout << cur << std::endl;
    }
  }

  // year
  if (true) {
    last = -1;
    cron_str.str("");
    cron_str << "0 5 1 15 10 * */2";
    std::cout << "test year: " << cron_str.str() << std::endl;
  //   std::cout << ct.match_time(t, cron_str, CronTab::MatchDirection::NEXT_MATCH) << std::endl;
    for (int i = 0; i < 5 * 365 * 86400; i += 86400 * 30) {
      auto cur = ct.match_time(t + i, cron_str.str(), CronTab::MatchDirection::NEXT_MATCH);
      if (last == cur) {
        continue;
      }
      last = cur;
      std::cout << cur << std::endl;
    }

    last = -1;
    cron_str.str("");
    cron_str << "0 5 1 15 10 * 2016";
    std::cout << "test year: " << cron_str.str() << std::endl;
  //   std::cout << ct.match_time(t, cron_str, CronTab::MatchDirection::NEXT_MATCH) << std::endl;
    for (int i = 0; i < 5 * 365 * 86400; i += 86400 * 30) {
      auto cur = ct.match_time(t + i, cron_str.str(), CronTab::MatchDirection::NEXT_MATCH);
      if (last == cur) {
        continue;
      }
      last = cur;
      std::cout << cur << std::endl;
    }
  }

}

constexpr int CronTab::CRON_FIELD_MAX_VAL[];
// std::vector<int> CronTab::CRON_FIELD_MAX_VAL = {60, 60, 24, 31, 12, 7, std::numeric_limits<int>::max()};

int main(void) {
  test_match_time();
  return 0;
}

// vim: et tw=80 ts=2 sw=2 cc=80:
