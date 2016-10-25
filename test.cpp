#define TEST69

#ifdef TEST69
#define TEST69_TAG ""
const char* testDescription = "TEST69 2016-10-25-Tue 13:00:29 " TEST69_TAG;
#include <iostream>
#include <regex.h>
#include <sys/types.h>

//using namespace std;

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;

//   const char* regex = R"((^\s*\$\(\s*([^\s]+)\s*\)\s*([+-])\s*P(\d+)([DMYHm])\s*$)|(^\s*(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z)\s*$))";
  char ebuf[128];

  // compile
  regex_t reg;
//   const char* regex = R"(\s*([0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}Z)\s*)";
  const char* regex = R"(\s*\$\(([a-zA-Z_]+)\)\s*([+-])\s*P\s*([0-9]+)\s*([DMYHm])\s*)";
  int cflags = REG_EXTENDED;
  int ret = regcomp(&reg, regex, cflags);
  regerror(ret, &reg, ebuf, sizeof(ebuf));
  std::cout << "compile reg: " << ret << ", " <<  ebuf << std::endl;

  // match
//   std::string str = "2016-10-25T13:00:29Z";
//   std::string str(" $( lastModified)+ P7D 2016-09-22T12:58:41Z");
  std::string str = " $(lastModified)+P7D \t";
  regmatch_t pmatch[10];
  size_t nmatch = sizeof(pmatch) / sizeof(regmatch_t);
  std::cout << nmatch << std::endl;
  int eflags = 0;
  ret = regexec(&reg, str.c_str(), nmatch, pmatch, eflags);
  regerror(ret, &reg, ebuf, sizeof(ebuf));
  std::cout << "exec reg: " << ret << ", " <<  ebuf << std::endl;

  if (ret) {
    return -1;
  }
  // iterate matches
  for (decltype(nmatch) x = 0; x < nmatch && pmatch[x].rm_so != -1; ++x) {
    if (x == 0) {
      std::cout << str << std::endl;
    }
    std::cout << x << ": "
      << str.substr(pmatch[x].rm_so, pmatch[x].rm_eo - pmatch[x].rm_so)
      << std::endl;
  }

  // free
  regfree(&reg);

  return 0;
}
#endif // TEST69
#ifdef TEST68
#define TEST68_TAG "group strings by prefixes"
const char* testDescription = "TEST68 2016-10-24-Mon 19:42:56 " TEST68_TAG;
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <deque>

//using namespace std;

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;
  std::vector<std::string> strs = {
    "a/中文/b/c/d/e/f",
    "a/中文/b/c/d",
    "a/c/c/d/e/f",
    "a/c/c/d/e",
    "a/c/c/d",
    "a/c/ c",
    "a/d/c/d/e/f",
    "a/d/c/d/e",
    "a/d/c/d",
    "a/d/c",
    "a/b/c/d/e/f",
    "a/b/c/d/e",
    "a/b/c/d",
    "a/b/c",
  };
  std::random_shuffle(strs.begin(), strs.end());
  for (const auto& i : strs) {
//     std::cout << i << std::endl;
  }
  std::sort(strs.begin(), strs.end()); // sort in lexicographic order
  for (const auto& i : strs) {
//     std::cout << i << std::endl;
  }

  auto starts_with = [](const std::string& str, const std::string& pre) -> bool {
    if (str.length() < pre.length()) {
      return false;
    } else if (str.compare(0, pre.length(), pre) == 0) {
      return true;
    } else {
      return false;
    }
  };

  std::vector<std::deque<std::string>> prefix_groups;
  if (strs.size() > 0) {
    std::string pivot;
    auto i = strs.rbegin();
    pivot = *i;
    ++i;
    prefix_groups.emplace_back(1, pivot);
    std::cout << "new group: " << pivot << std::endl;
    auto cur_group_it = prefix_groups.begin();
    for (; i!= strs.rend(); ++i) {
      std::cout << "current group size: " << cur_group_it->size() << std::endl;
      if (starts_with(pivot, *i)) {
        cur_group_it->emplace_front(*i);
        std::cout << "update group " << pivot << " to " << *i << std::endl;
      } else {
        std::cout << "new group: " << *i << std::endl;
        prefix_groups.emplace_back(1, *i);
        cur_group_it = std::prev(prefix_groups.end());
      }
      pivot = *i;
    }
  }

  std::cout << "group size: " << prefix_groups.size() << std::endl;
  for (const auto& i : prefix_groups) {
    if (i.cbegin() != i.cend()) {
      std::cout << "============ group: " << *i.cbegin() << std::endl;
    }
    for (const auto& j : i) {
      std::cout << j << std::endl;
    }
  }

  return 0;
}
#endif // TEST68

#ifdef TEST67
#define TEST67_TAG "template's all arguments all with default value"
const char* testDescription = "TEST67 2016-10-20-Thu 13:41:18 " TEST67_TAG;
#include <iostream>

template <typename A, typename B = double>
class C {
public:
  A a;
  B b;
};

// template <>
// class C : public C<int>{
// };

//using namespace std;

int main(int argc, char** argv) {
  std::cout << std::endl << testDescription << std::endl;
  C<int> c;
  return 0;
}
#endif // TEST67

#ifdef TEST66
#define TEST66_TAG "linux signal sigaction test"
const char* testDescription = "TEST66 2016-10-19-Wed 11:34:38 " TEST66_TAG;
#include <iostream>
#include <thread>
#include <chrono>
// #include <signal.h>

//using namespace std;
// bool g_quit = false;
// void signal_handler(int signal) {
//   std::cout  << "signal(" << signal << ") caught";
//   g_quit = true;
//   A::quit = true;
//   A::get_instance()->stop();
// }

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
 
static void hdl (int sig, siginfo_t *siginfo, void *context) {
  printf ("Sending PID: %ld, UID: %ld\n",
      (long)siginfo->si_pid, (long)siginfo->si_uid);
  std::cout << "Signal num: " << siginfo->si_signo << std::endl;
  std::cout << reinterpret_cast<std::string*>(context)->data() << std::endl;
}
 
class A {
public:
  static bool quit;// = false;
  void run() {
    signal(SIGINT, A::signal_handler);
    signal(SIGTERM, A::signal_handler);
    int i = 0;
    while (!quit) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      std::cout << "test " << i++ << std::endl;
    }
  }
  static void signal_handler(int signal) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    std::cout  << "signal(" << signal << ") caught" << std::endl;;
    quit = true;
  }
};

bool A::quit = false;

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;
//   A a;
//   a.run();

  struct sigaction act;
  memset (&act, '\0', sizeof(act));
  /* Use the sa_sigaction field because the handles has two additional parameters */
  act.sa_sigaction = &hdl;
  /* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
  act.sa_flags = SA_SIGINFO;
  std::string ctx = "signal context";
  if (sigaction(SIGTERM, &act, reinterpret_cast<void*>(&ctx)) < 0) {
    perror ("sigaction");
    return 1;
  }

  while (1) {
    sleep (10);
  }

  return 0;
}
#endif // TEST66

#ifdef TEST65
#define TEST65_TAG "smart pointer, shared/unique/weak pointer"
const char* testDescription = "TEST65 2016-10-17-Mon 11:28:46 " TEST65_TAG;
#include <iostream>
#include <memory>

#include <string>
#include <ctime>
#include <sstream>

//using namespace std;

class A {
public:
  int a;
  int b;
  A() {
  }
  A(int a_, int b_): a(a_), b(b_) {
  }
};

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;
  int a = 100;
  std::shared_ptr<int> pa;// = std::make_shared<int>(new int());
  pa.reset(new int());
  *pa = 1000;
  std::cout << *pa << std::endl;
  A* tmp;
  {
    std::unique_ptr<A> p2(new A());
    tmp = p2.get();
    tmp->a = 1211;
    // delete tmp; // deletion will succeed
    std::cout << "=============" << std::endl;
//     int* aaa = new int[100 * 10240];
//     p2 = std::unique_ptr<A>(new A());
  }
  // delete tmp; // deletion will fail
  std::cout << tmp->a << std::endl; // undefined behavior


  std::shared_ptr<A> p3;
  {
    std::shared_ptr<A> p4(new A());
    p4->a = 10086;
    tmp = p4.get();
    p3 = p4;
//     delete tmp; // deletion will succeed, and will cause share_prt deletion falure
    std::cout << "++++++++++++++" << std::endl;
  }
  //  delete tmp; // deletion will succeed, and will cause share_prt deletion falure
  std::cout << p3->a << std::endl;

  std::weak_ptr<A> p5;

  // define a shared_ptr with make_shared
  auto p6 = std::make_shared<A>();
  auto p7 = std::make_shared<A>(1, 2);
  std::cout << p6->a << p6->b << std::endl;
  std::cout << p7->a << p7->b << std::endl;

  return 0;
}
#endif // TEST64

#ifdef TEST64
#define TEST64_TAG "string resize test --- resize will not trigger reallocation"
const char* testDescription = "TEST64 2016-10-17-Mon 11:28:46 " TEST64_TAG;
#include <iostream>
#include <string>
#include <typeinfo>
#include <iomanip>

//using namespace std;

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;
  std::string str1 = "abc";
  std::cout << typeid(std::string::pointer).name() << std::endl;

  std::ios::fmtflags fmt_falgs(std::cout.flags());

  std::cout << std::hex << reinterpret_cast<int64_t>(&str1[0]) << std::endl;
  std::cout << std::hex << reinterpret_cast<int64_t>(str1.c_str()) << std::endl;
  auto pstr1 = &str1;
  std::cout << std::hex << reinterpret_cast<int64_t>(&(*pstr1)[0]) << std::endl;
  str1.resize(1024 * 1024 * 10, 0);
  str1.reserve(1024 * 1024 * 10 + 1);

  std::cout << std::hex << reinterpret_cast<int64_t>(&(*pstr1)[0]) << std::endl;
  str1.clear();
  std::cout.flags(fmt_falgs);
  std::cout << "cap: " << str1.capacity() << std::endl;
  str1.resize(1, 0);
  std::cout << "cap: " << str1.capacity() << std::endl;


  return 0;
}
#endif // TEST64

#ifdef TEST63
#define TEST63_TAG "pointer for each test"
const char* testDescription = "TEST63 2016-10-17-Mon 11:24:38 " TEST63_TAG;
#include <iostream>
#include <vector>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << std::endl << testDescription << std::endl;
  std::vector<int> v = {1, 2, 4, 5};
  auto pv = &v;
  for (const auto& it : *pv) {
    std::cout << it << std::endl;
  }

  return 0;
}
#endif // TEST63

#ifdef TEST62
#define TEST62_TAG "key word const test"
const char* testDescription = "TEST62 2016-10-17-Mon 10:56:51 " TEST62_TAG;
#include <iostream>
#include <string>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << std::endl << testDescription << std::endl;
  int a = 10;
  int b= 100;

  const int* p1 = &a; // pointer points to read-only variable
  int const * p2 = &a; // pointer points to read-only variable

  int* const p3 = &a; // pointer is read-only, cannot be changed
  const int* const p4 = &a; // both pointer itself and the value it points to is read-only

  std::cout << "p1: " << *p1 << std::endl;
//   ++(*p1); // read-only variable is not assignable
//   p1 = &b; // will compile
  std::cout << "p2: " << *p2 << std::endl;
//   ++(*p2); // read-only variable is not assignable
//   p2 = &b; // will compile

  std::cout << "p3: " << *p3 << std::endl;
//   ++(*p3); // will compile
//   p3 = &b; // cannot assign to variable 'p3' with const-qualified type 'int *const'
  std::cout << "p4: " << *p4 << std::endl;
//   ++(*p4); // read-only variable is not assignable

  return 0;
}
#endif // TEST62

#ifdef TEST61
#define TEST61_TAG "stream string hex, std::hex test"
const char* testDescription = "TEST61 2016-10-13-Thu 21:32:12 " TEST61_TAG;
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

//using namespace std;

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;
  std::string str = "\r\n";
  str.push_back(-1);
  std::stringstream ss;
  for (const char& i : str) {
    std::cout << static_cast<int>(i) << std::endl;
    ss << std::setw(2) << std::setfill('0') << std::hex << static_cast<short>(i & 0xff);
  }
  std::cout << ss.str() << std::endl;
  std::cout << "test" << 1 << std::endl;

//   std::cout << std::hex << str.data() << "test" << std::endl;
//   int i = 0;
//   std::cout << i++ << i++ << "test" << i++ << std::endl;

  return 0;
}
#endif // TEST61

#ifdef TEST60
#define TEST60_TAG "timestamp test"
const char* testDescription = "TEST60 2016-10-10-Mon 20:52:57 " TEST60_TAG;
#include <iostream>
#include <chrono>

//using namespace std;

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;
  using namespace std::chrono;
  milliseconds ms = duration_cast<milliseconds>(
      system_clock::now().time_since_epoch()
  );
  std::cout << ms.count() / 1000 << std::endl;
  return 0;
}
#endif // TEST60

#ifdef TEST59
#define TEST59_TAG "fstream test"
const char* testDescription = "TEST59 2016-10-10-Mon 19:44:02 " TEST59_TAG;
#include <iostream>
#include <fstream>
#include <string>

//using namespace std;

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;
  std::ifstream ifs("/Users/gavin/tmp/tmp1.md", std::fstream::binary);
  std::string str;
  std::string buf_str;
  char buf[1024];
  int total_count = 0;
  int count = 0;
  while (ifs) {
    ifs.read(buf, sizeof(buf));
    int read_count = ifs.gcount();
    if (read_count > 0) {
      total_count += read_count;
      str.append({buf, static_cast<size_t>(read_count)});
    }
    ++count;
  }
  std::cout << str << std::endl;
  std::cout << total_count << std::endl;
  std::cout << count << std::endl;
  return 0;
}
#endif // TEST59

#ifdef TEST58
#define TEST58_TAG "hex str to bytes/string"
const char* testDescription = "TEST58 2016-09-27-Tue 18:49:11 " TEST58_TAG;
#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <vector>

//using namespace std;

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;
  std::string str = "E4B8ADe59bbde4baba";
  std::unordered_map<char, char> table = {
      {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4},
      {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9},
      {'a', 10}, {'b', 11}, {'c', 12}, {'d', 13}, {'e', 14}, {'f', 15},
      {'A', 10}, {'B', 11}, {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15}
  };

  char* buf = new char[str.length() / 2 + 1];
  buf[str.length() / 2] = 0;
  for (int i = 0; i < str.length(); ++i) {
    std::cout << str[i];
    if (i & 0x01) {
      // higher bits
      buf[i >> 1] |= (table[str[i]] & 0x0f);
      std::cout << " " << buf[i >> 1] << std::endl;
    } else {
      // lower bits
      buf[i >> 1] = (table[str[i]] & 0x0f) << 4;
    }
  }

//   std::string str1 = buf;
  std::cout << buf << std::endl;

  delete buf;
  return 0;
}
#endif // TEST58

#ifdef TEST57
#define TEST57_TAG "str lowbound compare"
const char* testDescription = "TEST57 2016-09-23-Fri 16:59:47 " TEST57_TAG;
#include <iostream>
#include <algorithm>
#include <vector>

//using namespace std;

void lambda1() {
//   std::vector<std::string>::iterator last_it;
//   bool matched = false;
//   for (int i = 0; i < object.length(); ++i) {
//     last_it = std::lower_bound(prefixes.begin(), prefixes.end(), object.substr(0, i + 1),
//         [i, matched](const std::string& a, const std::string& b) mutable -> int {
//           std::cout << a << " " << b << std::endl;
//           if (a.length() < (i + 1) || a[i] < b[i]) {
//             return -1;
//           } else if (a[i] == b[i]) {
//             if (a.length() == (i + 1)) {
//               matched = true;
//             }
//             return 0;
//           } else {
//             return 1;
//           }
//         }
//     );
//     if (last_it == prefixes.end() || matched) {
//       // no more to be found or matched
//       break;
//     }
//   }

//   if (matched || last_it != prefixes.end()) {
//     std::cout << "match: " << *last_it << std::endl;
//   } else {
//     std::cout << "no match" << std::endl;
//   }
}

// void distribute(std::vector<std::string> prefixes, std::string object) {
//   int i = 0;
//   int j = 0;
//   for (; i < object.length(); ++i) {
//     for (; i < prefixes.size();) {
//     }
//   }
// }

int gaojia(const std::vector<std::string>& prefixes, const std::string& object) {

//   auto it = std::lower_bound(prefixes.begin(), prefixes.end(), object);
  auto it = std::lower_bound(prefixes.rbegin(), prefixes.rend(), object);

//   if (it == prefixes.end()) {
  if (it == prefixes.rend()) {
    // no lower bound found
    std::cout << "no lower bound found" << std::endl;
    return 0;
  }

  auto starts_with = [](const std::string& str, const std::string& pre) -> bool {
    if (str.length() < pre.length()) {
      return false;
    } else if (str.compare(0, pre.length(), pre) == 0) {
      return true;
    } else {
      return false;
    }
  };

  bool matched = false;

  // if string starts with the found prefix
  if (starts_with(object, *it)) {
    std::cout << "first match" << std::endl;
    matched = true;
  }

//   if (!matched && it != prefixes.begin()) {
  if (!matched && it != prefixes.rbegin()) {
    --it; // try to match the previous prefix
    if (starts_with(object, *it)) {
      std::cout << "second match" << std::endl;
      matched = true;
    }
  }
  if (matched) {
    std::cout << "matched prefix: " << *it << std::endl;
  } else {
    std::cout << "no prefix matched" << std::endl;
  }
  return 0;
}

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;
  std::vector<std::string> prefixes = {"str1", "stra", "strb", "strc", "strx", "strz"};
  std::sort(prefixes.begin(), prefixes.end());
  std::string object = "strb/acd";
  gaojia(prefixes, object);
  return 0;
}
#endif // TEST57


#ifdef TEST56
#define TEST56_TAG "regex test"
const char* testDescription = "TEST56 2016-09-22-Thu 12:41:10 " TEST56_TAG;
#include <iostream>
#include <string>
#include <regex>

//using namespace std;

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;

  // expression
//   std::string s(" $( lastModified)+ P7D");

  // absolute time
  std::string s(" 2016-09-22T12:58:41Z ");

  // mixe 2 kinds of time format
//   std::string s(" $( lastModified)+ P7D 2016-09-22T12:58:41Z");
//   std::string s(" 2016-09-22T12:58:41Z  $( lastModified)+ P7D");

  // multiple lines
//   std::string s(" 2016-09-22T12:58:41Z  \n$( lastModified)+ P7D");

  // invalid time format
//   std::string s("fds 2016-09-22T12:58:41Z  $( lastModified)+ P7D akjl");

  // empty string
//   std::string s("");

  // blank string
//   std::string s("  \t");

  std::smatch m;
  std::string regex_str = R"((^\s*\$\(\s*([^\s]+)\s*\)\s*([+-])\s*P(\d+)([DMYHm])\s*$)|(^\s*(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z)\s*$))";
  std::regex e(regex_str);

  std::cout << "Target sequence: " << s << std::endl;
  std::cout << "Regex: " << regex_str << std::endl;

  if (std::regex_search(s, m, e)) {
    // the number of match group is the same as number of pairs of parenthesis
    // match group sequence is the same as the apparence of (
    // m[0] is the whole string which matches the pattern
    std::cout << "The following matches and submatches were found:" << std::endl;
    for (int i = 0; i < m.size(); ++i) {
      std::cout << "m[" << i << "]: " << m[i] << std::endl;
    }
    std::cout << std::endl;
    if (m[1] != "" && m[6] == "") {
      std::cout << "time expression: " << m[1] << std::endl;
      std::cout << "expresstion variable: " << m[2] << std::endl;
      std::cout << "expresstion oprator: " << m[3] << std::endl;
      std::cout << "expresstion right oprand: " << m[4] << std::endl;
      std::cout << "expresstion time gap unit: " << m[5] << std::endl;
    } else if (m[6] != "" && m[1] == "") {
      std::cout << "absolute time: " << m[6] << std::endl;
    } else {
      std::cout << "some format matched but it is invalid expresstion" << std::endl;
    }
  } else {
      std::cout << "no time format matched, invalid expresstion" << std::endl;
  }

  return 0;
}
#endif // TEST56

#ifdef TEST55
#define TEST55_TAG "test sprintf"
const char* testDescription = "TEST55 2016-09-19-Mon 17:49:01 " TEST55_TAG;
#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <map>

void print(int code, ...) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  va_list ap;
  va_start(ap, code);
  for (auto& i : ap) {
//     std::cout << typeid(i).name() << std::endl;
//     std::cout << i;
  }
  va_end(ap);
}

#define set_err_msg(buf, buf_size, fmt, ...) snprintf(buf, buf_size, fmt, ##__VA_ARGS__)

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;
  std::map<int, const char*> m;
  m[404] = "bucket %s, object %s, not found";
  m[409] = "bucket %s, object %s, count %d, not found";
  char buf[10240];
  memset(buf, 0, 10240);
  set_err_msg(buf, sizeof(buf), m[404], "bucket===", "object===");
  std::cout << buf << std::endl;
  memset(buf, 0, 10240);
  set_err_msg(buf, sizeof(buf), m[409], "bucket===", "object===", 12345);
  std::cout << buf << std::endl;

  return 0;
}
#endif // TEST55

#ifdef TEST54
#define TEST54_TAG "tuple iteration"
const char* testDescription = "TEST54 2016-09-18-Sun 21:45:21 " TEST54_TAG;
#include <iostream>
#include <tuple>
#include <utility>

// template<std::size_t I = 0, typename FuncT, typename... Tp>
// inline typename std::enable_if<I == sizeof...(Tp), void>::type for_each(
//   std::tuple<Tp...> &, FuncT) { // Unused arguments are given no names.
// }
// 
// template<std::size_t I = 0, typename FuncT, typename... Tp>
// inline typename std::enable_if<I < sizeof...(Tp), void>::type for_each(
//   std::tuple<Tp...>& t, FuncT f) {
//   f(std::get<I>(t));
//   for_each<I + 1, FuncT, Tp...>(t, f);
// }
// 
// template<typename T>
// void f(T t) {
//   std::cout << t << " ";
// }
// 
// int main(int argc, char** argv) {
//   std::cout << "argc: " << argc << std::endl;
//   for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
//   std::cout << std::endl << testDescription << std::endl;
// 
//   auto args = std::make_tuple(1, "bbb", 1.0);
//   for_each(args, f);
//   return 0;
// }

template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type print(
    int, std::tuple<Tp...>&) { // template convergence
}

template<std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), void>::type print(
    int i, std::tuple<Tp...>& t) {
  std::cout << std::get<I>(t) << std::endl;
  print<I + 1, Tp...>(i, t);
}

int main() {
  typedef std::tuple<int, float, double> T;
  T t = std::make_tuple(2, 3.14159F, 2345.678);
  auto b = std::make_tuple(2, 3.14159F, 2345.678);
  print(1, b);
}

#endif // TEST54

#ifdef TEST53
#define TEST53_TAG "tuple as input parameter test"
const char* testDescription = "TEST53 2016-09-18-Sun 20:24:10 " TEST53_TAG;
#include <iostream>
#include <tuple>
#include <sstream>
#include <string>
#include <type_traits>
#include <iomanip>

template<std::size_t I = 0, typename... Tp>
typename std::enable_if<I == sizeof...(Tp), std::string>::type set_err_msg(
    int, const std::tuple<Tp...>&) { // for template's convergence
//   std::stringstream ss;
//   ss << std::endl;
//   return ss.str();
  return "";
}

/**
 * remove all types' const volatile modifiers, return the most basic type name
 */
template<typename T>
using decay_t = typename std::conditional<std::is_pointer<T>::value,
    std::decay<
      typename std::add_pointer<
        typename std::remove_cv<
          typename std::remove_pointer<T>::type
        >::type
      >::type>,
    std::decay<typename std::remove_cv<T>::type>
  >::type;

/**
 * compare 2 types ignoring modifiers: const, reference
 */
template<typename A, typename B>
struct is_same_type {
  static constexpr bool value = std::is_same<typename decay_t<A>::type, B>::value;
};

// vararg template types must be placed at the end
template<std::size_t I = 0, typename... Tp> 
typename std::enable_if<I < sizeof...(Tp), std::string>::type set_err_msg(
    int code, const std::tuple<Tp...>&& tp) {

  std::stringstream ss;
  if (I == 0) {
    ss << std::to_string(code) << " ";
    std::cout << code << " " << std::endl;;
  }

  auto e = std::get<I>(tp);
  // can be implemented with tuple and function template
  if (is_same_type<decltype(e), int>::value
      || is_same_type<decltype(e), unsigned int>::value
      || is_same_type<decltype(e), float>::value
      || is_same_type<decltype(e), double>::value
      || is_same_type<decltype(e), long>::value
      || is_same_type<decltype(e), unsigned long>::value
      || is_same_type<decltype(e), long long>::value
      || is_same_type<decltype(e), unsigned long long>::value
      || is_same_type<decltype(e), long double>::value
      ) {
    // numbers
    ss << std::setprecision(5) << e << " ";
    std::cout << "===================" << e << " typeid: " << typeid(e).name() <<  std::endl;
  } else if (
      is_same_type<decltype(e), std::string>::value
      || is_same_type<decltype(e), char>::value
      || is_same_type<decltype(e), char*>::value
      ) {
    // strings
    ss << e << " ";
  } else {
    // other types not supported
    throw std::invalid_argument("invalid_argument");
  }
  std::cout << e << std::endl;
  ss << set_err_msg<I + 1, Tp...>(code, std::forward<decltype(tp)>(tp));
  return ss.str();
}

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;

  auto args = std::make_tuple(100.01, 100.100f, "abb", 1123, "bbb", std::string("std string"));
  auto res = set_err_msg(1000, std::forward<decltype(args)>(args));
  std::cout << res << std::endl;
  // wrong invokation
  set_err_msg(10086, std::make_tuple(100.100f, "abb", 1123, "bbb"));
  set_err_msg(10086, std::forward_as_tuple(100.100f, "abb", 1123, "bbb"));

//   std::cout << std::boolalpha
//     << std::is_same<typename std::decay<decltype("")>::type, char[]>::value
//     << std::endl
//     << std::is_same<typename std::decay<decltype("")>::type, char*>::value
//     << std::endl
//     << std::is_same<typename std::decay<decltype("")>::type, const char*>::value
//     << std::endl;

//   auto e = std::get<1>(args);
//   std::cout << std::boolalpha
//       << is_same_type<decltype(e), int>::value << std::endl
//       << is_same_type<decltype(e), unsigned int>::value << std::endl
//       << is_same_type<decltype(e), float>::value << std::endl
//       << is_same_type<decltype(e), double>::value << std::endl
//       << is_same_type<decltype(e), long>::value << std::endl
//       << is_same_type<decltype(e), unsigned long>::value << std::endl
//       << is_same_type<decltype(e), long long>::value << std::endl
//       << is_same_type<decltype(e), unsigned long long>::value << std::endl
//       << is_same_type<decltype(e), long double>::value << std::endl
//       << is_same_type<decltype(e), char*>::value << std::endl
//       << is_same_type<decltype(e), const char*>::value << std::endl
//       << is_same_type<decltype(e), const char>::value << std::endl
//       << is_same_type<decltype(e), char>::value << std::endl
//       << std::is_same<typename std::remove_const<std::remove_pointer<const char*>::type>::type, char>::value << std::endl
//       << std::is_same<typename std::remove_all_extents<const char*>::type, char*>::value << std::endl
//       << std::is_same<typename std::decay<const int>::type, int>::value << std::endl
//       << std::is_same<typename std::decay<const char>::type, char>::value << std::endl
//       << typeid(std::remove_pointer<std::add_pointer<char>::type>::type).name() << std::endl
//       << std::endl;
  return 0;
}
#endif // TEST53

#ifdef TEST52
#define TEST52_TAG "map trie"
const char* testDescription = "TEST52 2016-09-18-Sun 11:06:08 " TEST52_TAG;
#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include <set>

std::string hex_str(const std::string& str) {
  std::string res = "";
  res.reserve(str.length() * 2);
  const char* table = "0123456789ABCDEF";
  for (const char& i : str) {
    res.push_back(table[(i & 0xf0) >> 4]);
    res.push_back(table[i & 0x0f]);
  }
  return res;
}
std::string hex_str(const char& c) {
  return hex_str(std::string() + c);
//     return hex_str(std::string(*const_cast<char*>(&c)));
}

// map trie can save a lot of space

/**
 * node of trie, the given data type
 */
template<typename T>
struct Node {
  std::map<char, Node*> e; // edge
  bool is_end;
  T data;
  int color;
};

/**
 * 
 */
template<typename T>
class MapTrie {
public:
  typedef Node<T> NodeType;
  NodeType root;
  size_t size;
  MapTrie() {
    // empty string is always contained by a trie
    root.is_end = true;
    size = 1;
  }

  void insert(const std::string& str) {
    auto cursor = &root;
    for (const char& i : str) {
      if (cursor->e.find(i) == cursor->e.end()) { // save time
//       if (cursor->e[i] == nullptr) { // save space
        cursor->e[i] = new NodeType();
        cursor->e[i]->is_end = false;
        ++size;
        std::cout << "new edge/node: " << hex_str(i) << std::endl;
      } else {
        std::cout << "existed edge/node: " << hex_str(i) << std::endl;
      }
      cursor = cursor->e[i];
    }
    cursor->is_end = true;
  }

  /**
   * check if the given string matched in the trie
   * default match prefix only
   */
  bool check(const std::string& str, bool prefix_only = true) {
    auto prefix_node = get_prefix_node(str);
    if (prefix_node == nullptr) {
      return false;
    } else {
      return prefix_only ? true : prefix_node->is_end;
    }
  }

  const NodeType* get_prefix_node(const std::string& prefix) {
    auto cursor = &root;
    for (const char& i : prefix) {
      // no such edge
      if (cursor->e.find(i) == cursor->e.end()) { // save space
//       if (cursor->e[i] == nullptr) { // save time
        std::cout << "no edge: " << hex_str(i) << std::endl;
        cursor = nullptr;
        break;
      } else {
        std::cout << "edge: " << hex_str(i) << std::endl;
        cursor = cursor->e[i];
      }
    }
    return cursor;
  }

  void remove(const std::string& str) {
  }

  void walk(std::function<void(NodeType)> f, int walk_type = 0) {
    auto cursor = &root;
    if (walk_type == 0) {
      std::stack<decltype(cursor)> stk;
      std::set<decltype(cursor)> dfs_set;
      stk.push(cursor);
      dfs_set.insert(cursor);
      while (stk.size() > 0) {
        std::cout << "stk.size " << stk.size() << std::endl;
        cursor = stk.top();
        auto it = cursor->e.begin();
        while (true) { // for each adj(cursor)
          if (it != cursor->e.end()) {
            auto tmp = cursor->e.begin()->second;
            if (tmp != nullptr) {
              auto insert_res = dfs_set.insert(tmp);
              if (insert_res.second) {
                stk.push(tmp);
                cursor = tmp;
                it = cursor->e.begin();
              } else {
                ++it;
              }
            } else {
              break;
            }
          } else {
            break;
          }
        }
        auto tmp = stk.top();
        f(*tmp);
        stk.pop();
        dfs_set.insert(tmp);
      }
    }
  }


  ~MapTrie() {
  }

};

void walk(MapTrie<std::string>::NodeType* root) {
  root->color = 1;
  for (auto& i : root->e) {
    if (i.second != nullptr && i.second->color == 0) {
      walk(i.second);
    }
  }
  root->color = 2;
  std::cout << root << std::endl;
}

int main(int argc, char* argv[]) {
  std::cout << "argc: " << argc << std::endl;
  for (int i = 0; i < argc; ++i) {
    std::cout << argv[i] << " ";
  }
  std::cout << std::endl << testDescription << std::endl;
  std::vector<std::string> v= {"", "\0", "abc", "abcd", "中国", "中国人", "example string"};
  MapTrie<std::string> trie;
  for (const auto& i : v) {
    std::cout << "insert: " << i << std::endl;
    trie.insert(i);
  }
  v.push_back("a b c");
  v.push_back("ab");
  for (const auto& i : v) {
    std::cout << i << std::endl << trie.check(i) << std::endl;
  }

//   trie.walk([](MapTrie<std::string>::NodeType node) {
//       if (node.e.begin() != node.e.end()) {
//         std::cout << hex_str(node.e.begin()->first) << std::endl;
//       }
//       std::cout << "xxxxxxxxxxxxxxxxx" << std::endl;
//   });
  walk(&(trie.root));
  std::cout << trie.size << std::endl;
  return 0;
}
#endif // TEST52

#ifdef TEST51
#define TEST51_TAG "struct default move constructor"
const char* testDescription = "TEST51 2016-09-13-Tue 19:05:47 " TEST51_TAG;
#include <iostream>

//using namespace std;
struct Inner {
    Inner(){
        printf("Default constructor called\n");
    }
    Inner(const Inner &i) {
        printf("Copy constructor called\n");
    }
    Inner(Inner &&i) {
        printf("Move constructor called\n");
    }
    ~Inner() {
        printf("Destructor called\n");
    }
    Inner& operator=(const Inner &i) {
        printf("Assign operator called\n");
        return *this;
    }
    Inner& operator=(Inner &&i) {
        printf("Move assign operator called\n");
        return *this;
    }
};

struct Defaulter {
    // Defaulter() = default;
    ~Defaulter() = default;
//     Defaulter(Defaulter &&other) {
//       this->abc = other.abc;
//       this->def = other.def;
//     }
    std::string abc = "aaa";
    int def = 1;
    Inner ii;
};

int main(int argc, char *argv[]) {
  std::cout << testDescription << std::endl;
  Defaulter d;
  d.abc = "bbb";
  d.def = 2;
  Defaulter d2(std::move(d));
  // Defaulter d2(d);
  return 0;
}
#endif // TEST51

#ifdef TEST50
#define TEST50_TAG "init list test"
const char* testDescription = "TEST50 2016-09-12-Mon 21:07:15 " TEST50_TAG;
#include <iostream>
#include <initializer_list>
#include <vector>

//using namespace std;

class C {
  std::vector<std::string> str;
public:
  C(std::initializer_list<std::string> il) {
    for (const auto& i : il) {
      str.push_back(i);
    }
  }
  void print() {
    for (const auto& i : str) {
      std::cout << i << std::endl;
    }
  }
};

template<typename T1, typename T2>
class MyPair {
public:
  T1 a;
  T2 b;
  MyPair(T1 a, T2 b): a(a), b(b) {
    std::cout << "construct MyPair" << std::endl;
  }
  MyPair(std::initializer_list<T1> il) {
    for (const auto& i : il) {
      std::cout << i << " ";
    }
    std::cout << std::endl;
  }
};

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  C c{"a", "b", "c"};
  c.print();

  // construct 
  MyPair<int, std::string> my_pair = {10, "bb"};
  std::cout << my_pair.a << " " << my_pair.b << std::endl;

  // container
  std::vector<MyPair<int, std::string>> v = {
    {1, "1"},
    {2, "2"},
    {3, "3"},
    {4, "4"},
  };
  for (const auto& i : v) {
    std::cout << i.a << "->" << i.b << std::endl;
  }

  MyPair<int, int> mp = {1, 2}; // initializer_list ctor
  std::vector<MyPair<int, int>> v1 = {
    {1, 1},
    {2, 2},
    {3, 3},
    {4, 4},
  };
  return 0;
}
#endif // TEST50

#ifdef TEST49
#define TEST49_TAG "map init list, enum as map's input class"
const char* testDescription = "TEST49 2016-09-12-Mon 20:16:43 " TEST49_TAG;
#include <iostream>
#include <map>

//using namespace std;

enum E {
  A = 100,
  B,
  C,
};

std::map<E, std::string> get_err_msg_map() {
//   std::map<int, std::string> m = {
//     {E::A, "yangzhijia"},
//     {E::B, "zhoufei"},
//   };
//   return m;
  return {
    {E::A, "yangzhijia"},
    {E::B, "zhoufei"},
    {E::C, "test"},
  };
}

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  auto m = get_err_msg_map();
  for (auto& i : m) {
    std::cout << i.first << " " << i.second << std::endl;
  }
  return 0;
}
#endif // TEST49

#ifdef TEST48
#define TEST48_TAG "lambda test"
const char* testDescription = "TEST48 2016-09-12-Mon 11:48:13 " TEST48_TAG;
#include <iostream>
#include <vector>
#include <algorithm>

//using namespace std;

struct Rule {
  std::string action;
  int last_modified;
  Rule(const std::string& action, int last_modified):
    action(action), last_modified(last_modified) {
      std::cout << "构造" << std::endl;
  }
};
int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  auto f = [](Rule r1, Rule r2) -> int {
      if (r1.action == "DeleteObject" && r2.action != "DeleteObject") {
        return 1;
      } else if (r2.action == "DeleteObject" && r1.action != "DeleteObject") {
        return -1;
      } else {
        return 0;
      }
  };
//   std::cout << f() << std::endl;
  std::vector<Rule> rules;
  rules.emplace_back(Rule("Transition", 1));
  rules.emplace_back(Rule("Other", 1));
  rules.emplace_back(Rule("DeleteObject", 1));
  for (auto& i : rules) {
    std::cout << i.action << std::endl;
  }
  std::sort(rules.begin(), rules.end(), f);
  for (auto& i : rules) {
    std::cout << i.action << std::endl;
  }
  std::string str1 = "中国";
  std::string str2 = "中人";
  std::cout << (str1 < str2) << std::endl;
  std::cout << str1.size() << std::endl;
  auto bin = [] (char c) -> void {
    for (size_t i = 0; i < 8; ++i) {
      std::cout << (c & (0x80 >> i) ? '1' : '0');
    }
    std::cout << std::endl;
  };
  for (auto& i : str2) {
    std::cout << int(i & 0xff) << ' ';
    bin(i);
  }
  return 0;
}
#endif // TEST48

#ifdef TEST47
#define TEST47_TAG "verion test"
const char* testDescription = "TEST47 2016-08-18-Thu 14:55:08 " TEST47_TAG;
#include <iostream>

// #define VERSION "2016-08-18-Thu 14:55:42"

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::cout << "version: " << VERSION << std::endl;
  return 0;
}
#endif // TEST47

#ifdef TEST46
#define TEST46_TAG "test yang"
const char* testDescription = "TEST46 2016-08-17-Wed 20:30:21 " TEST46_TAG;
#include <vector>
#include <iostream>

using namespace std;
struct Stat {
    Stat() {
        std::cout << "contruct\n";
    }
    Stat& operator=(const Stat& oth) {
        id = oth.id;
        used = oth.used;
        count = oth.count;
        std::cout << id << " assign\n";
        return *this;
    }
    Stat(const int& i, const int& u, const int& cnt) {
        id = i;
        used = u;
        count = cnt;
        std::cout << id<< "construct2\n";
    }

    /*
    Stat(Stat&& oth) {
        id = oth.id;
        used = oth.used;
        count = oth.count;
        std::cout << id<< "move construct\n";
    }
*/
    Stat(const Stat& oth) {
        id = oth.id;
        used = oth.used;
        count = oth.count;
        std::cout << id<< "copied\n";
    }
/*
    Stat& operator=(Stat&& oth) {
        id = oth.id;
        used = oth.used;
        count = oth.count;
        std::cout << id<< "move assign\n";
        return *this;
    }
    */
    int id = 0;
    int used = 0;
    int count = 0;
};

int main() {
    vector<Stat> vec;

    Stat st;
    Stat st2;
    st2.id = 2;
    Stat st3;
    st3.id = 3;
    Stat st4;

    vec.reserve(16);
    cout << "========vector push back===========\n";
    vec.push_back(st2);

    cout << "========vector insert===========\n";
    vec.insert(vec.begin(),  st3);
}
#endif // TEST46

#ifdef TEST45
#define TEST45_TAG "test vector insert copy"
const char* testDescription = "TEST45 2016-08-17-Wed 20:10:15 " TEST45_TAG;
#include <iostream>
#include <vector>

class A {
  public:
  int a;
  A(int a): a(a) {
    std::cout << "construct A" << std::endl;
  }

  A(const A& other) {
    std::cout << other.a << "copied" << std::endl;
    this->a = other.a;
  }

  A& operator =(const A& other) {
    std::cout << other.a << "assign" << std::endl;
    this->a = other.a;
    return *this;
  }
};

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::vector<A> v;
  v.reserve(100);
  A a1(1);
  A a2(2);
  A a3(3);
  A x = a1;
  a2 = a1;
  A y(a1);
//   v.push_back(a1);
//   v.push_back(a2);
//   auto it = v.begin();
//   ++it;
//   v.insert(it, a3);
//   for (auto i : v) {
//     std::cout << i.a << std::endl;
//   }
  return 0;
}
#endif // TEST45

#ifdef TEST44
#define TEST44_TAG "test terminal name"
const char* testDescription = "TEST44 2016-08-17-Wed 15:26:54 " TEST44_TAG;
#include <iostream>
#include <termios.h>
#include <unistd.h>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::cout << ttyname(1) << "aaaaaaa" << std::endl;
  return 0;
}
#endif // TEST44

#ifdef TEST43
#define TEST43_TAG "test stderr"
const char* testDescription = "TEST43 2016-08-12-Fri 00:16:10 " TEST43_TAG;
#include <iostream>
#include <unistd.h>
//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::cerr << "test" << std::endl;

  if (isatty(1)) {
    std::cout << "not directed" << std::endl;
  }

  return 0;
}
#endif // TEST43

#ifdef TEST42
#define TEST42_TAG "constexpr"
const char* testDescription = "TEST42 2016-08-09-Tue 19:26:10 " TEST42_TAG;
#include <iostream>

//using namespace std;
class CTidu {
//   constexpr static int tidu;
  static int tidu1;
};

int CTidu::tidu1 = 10;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;

  return 0;
}
#endif // TEST42

#ifdef TEST41
#define TEST41_TAG "immutable cast"
const char* testDescription = "TEST41 2016-08-03-Wed 20:07:13 " TEST41_TAG;
#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <map>

template <typename T>
struct immutable_cast {
  template <typename R>
  typename std::add_const<T>::type operator()(R arg) {
    typedef typename std::add_const<T>::type add_const_t;
    add_const_t arg1 = arg;
    return arg1;
  }
};

template <typename To, typename From>
const To& immutable_cast_func(From& arg) {
  const To& arg1 = arg;
//   arg1 = new int[1];
  return arg1;
}

void test_immuatable_func() {
  int a = 10;
  int* pa = new int[1];
//   auto b = immutable_cast_func<int>(a);
  auto b1 = immutable_cast_func<int*>(pa);
//   b = 11;
  *b1 = 12;
//   b1 = new int[1];
//   b1 = &a;
  std::cout << typeid(b1).name() << std::endl;
  std::cout << *b1 << std::endl;
}

class Class {
};

const char* const_char(char* in) {
  const char*& a = in;
  return a;
}

void test_const_return() {
  char* p = new char[10];
  p[0] = 'a';
//   auto a = const_char(p);
  auto a = immutable_cast_func<char*>(p);
  a[0] = 98;
}

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;


//   int a = 10;
//   typedef std::add_const<int>::type A;
//   A a1 = 10;
//   const int* ptr = new int[1];
//   auto ptr1 = const_cast<int*>(ptr);
//   std::cout << *ptr1 << " " << typeid(ptr1).name() << " " << typeid(Class).name();

//   std::map<int, int> m;
//   auto m1 = immutable_cast<std::map<int, int>>(m);
  return 0;
}
#endif // TEST41

#ifdef TEST40
#define TEST40_TAG "linked list quick sort"
const char* testDescription = "TEST40 2016-07-31-Sun 18:04:24 " TEST40_TAG;
#include <iostream>
#include <functional>
#include <random>

//using namespace std;

template <typename T>
struct Node {
  Node* next;
  T val;
};

template <typename T>
Node<T>* make_single_linked_list(int n, std::function<T()> random_val_generator) {
  typedef Node<T>* NodePtr;
  if (n <= 0) {
    return nullptr;
  }
  NodePtr head = new Node<T>();
  NodePtr node = head;
  int cnt = 0;
  while (true) {
    ++cnt;
//     std::cout << "making: " << cnt;
    node->val = random_val_generator();
    if (cnt >= n) {
      node->next = nullptr;
      break;
    } else {
      node->next = new Node<T>();
      node = node->next;
    }
  }
  return head;
}

template <typename T>
void print_linked_list(const Node<T>* head) {
  const Node<T>* node = head;
  while (node != nullptr) {
    std::cout << node->val << " ";
    node = node->next;
  }
  std::cout << std::endl;
}

int random_int_generator(int lowerbound = 0, int upperbound = 99) {
//   static int a[] = {1, 2, 3, 4, 5, 6};
//   static int idx = 0;
//   return a[idx++ % (sizeof(a) / sizeof(int))];
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(lowerbound, upperbound);
  return dist(mt);
}

template <typename T>
Node<T>* linked_list_partition(Node<T>* head, Node<T>** ltail, Node<T>** rtail, Node<T>* pre_head) {
  if (head == nullptr) {
    return nullptr;
  }
  typedef Node<T>* NodePtr;
  NodePtr p = head;
  NodePtr new_head = head;
  NodePtr last = head;
  *ltail = head;
  *rtail = head;
  NodePtr cur = head->next;
  NodePtr node_infront_pivot = nullptr;
  while (cur != nullptr) {
    if (cur->val >= p->val) {
      // move cursor
      last = cur;
      cur = cur->next;
    } else if (cur->val <= p->val) {
      // move node under cursor to the head and move cursor forward
      NodePtr cur_next = cur->next;
      cur->next = new_head;
      last->next = cur_next;
      new_head = cur;
      if (node_infront_pivot == nullptr) {
        node_infront_pivot = new_head;
        *ltail = new_head;
      }
      cur = cur_next;
    }
    *rtail = cur;
  }
  // split the list
  if (node_infront_pivot != nullptr) {
//     node_infront_pivot->next = nullptr;
  }
  if (pre_head != nullptr) {
    std::cout << "============" << std::endl;
    pre_head->next = new_head;
  }
  return new_head;
}

template <typename T>
Node<T>* linked_list_qsort(Node<T>* head, Node<T>* tail, Node<T>* pre_head) {
  if (head == nullptr || head->next == nullptr || head == tail) {
    return head;
  }
//   std::cout << "qsort: " << head->val << std::endl;
  Node<T>* ltail;
  Node<T>* rtail;
  auto left_head = linked_list_partition(head, &ltail, &rtail, pre_head);
  auto right_head = head->next == nullptr ? nullptr : head->next;
  left_head = linked_list_qsort(left_head, ltail, static_cast<Node<T>*>(nullptr));
  linked_list_qsort(right_head, rtail, head);
  return left_head;
}

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;

//   for (auto i : a) {
//     std::cout << i << std::endl;
//     std::cout << random_int_generator() << std::endl;
//   }
//   std::function<int()> random_val_generator = random_int_generator;
//   auto head = make_single_linked_list(10, random_val_generator);

  auto head = make_single_linked_list(10, std::function<int()>(std::bind(random_int_generator, -1000, 1000)));
  auto tail = head;
  std::vector<int> v;
  while (true) {
    v.push_back(tail->val);
    if (tail->next == nullptr) {
      break;
    } else {
      tail = tail->next;
    }
  }
  std::sort(v.begin(), v.end());
  print_linked_list(head);
  for (auto i : v) {
    std::cout << i << " ";
  }
  std::cout << std::endl;

  head = linked_list_qsort(head, tail, static_cast<Node<int>*>(nullptr));
  print_linked_list(head);
  return 0;
}
#endif // TEST40

#ifdef TEST39
#define TEST39_TAG "test decltype rvalue"
const char* testDescription = "TEST39 2016-07-19-Tue 19:41:21 " TEST39_TAG;
#include <iostream>

//using namespace std;
//

int f1() {
  return 1;
}

int f2() {
  return 1L;
}

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  decltype(f1() + f2()) i;
  std::cout << sizeof(i) << std::endl;
  return 0;
}
#endif // TEST39

#ifdef TEST38
#define TEST38_TAG "quick sort"
const char* testDescription = "TEST38 2016-07-13-Wed 18:33:24 " TEST38_TAG;
#include <iostream>
#include <algorithm>
#include <vector>

//using namespace std;
template <typename Array>
int partition(Array a, int l, int r) {
  int pivot = a[l];
  int i = l;
  int j = r;
  while (i < j) {
    // >= pivot, from right first
    while (a[j] >= pivot && i < j) {
      --j;
    }
    // <= pivot, or this loop will be dead loop
    while (a[i] <= pivot && i < j) {
      ++i;
    }
    std::swap(a[i], a[j]);
  }
  std::swap(a[l], a[j]);
  return j;
}

template <typename Array>
void qsort(Array a, int l, int r) {
  if (l >= r) {
    return;
  }
  int pidx = partition(a, l, r);
  qsort(a, l, pidx - 1);
  qsort(a, pidx + 1, r);
}

template <typename Array>
void qselect(Array a, int l, int r, int kth) {
  int pidx = partition(a, l, r);
  if (pidx == kth) {
    return;
  } else if (pidx < kth) {
    qselect(a, pidx + 1, r, kth);
  } else if (pidx > kth) {
    qselect(a, l, pidx - 1, kth);
  }
}

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
//   int a[] = {5, 100000, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
//   int max_idx = sizeof(a) / sizeof(int) - 1;
  std::vector<int> a = {5, 100000, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  int max_idx = a.size() - 1;
//   std::cout << partition(a, 0, max_idx) << std::endl;
  int kth = 10;
  qselect(a, 0, max_idx, kth);
  std::cout << kth << "th: " << a[kth] << std::endl;
  qsort(a, 0, max_idx);
  for (auto i : a) {
    std::cout << i << " ";
  }
  std::cout << std::endl;
  return 0;
}
#endif // TEST38

#ifdef TEST37
#define TEST37_TAG "test function template"
const char* testDescription = "TEST37 2016-07-04-Mon 18:49:19 " TEST37_TAG;
#include <iostream>

template <typename Function>
struct function_traits
: public function_traits<decltype(&Function::operator())> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const> {
// struct function_traits<ReturnType(ClassType::*)(Args...) const> {
//   typedef ReturnType (*pointer)(Args...);
  typedef std::function<ReturnType(Args...)> function;
};

template <typename Function>
typename function_traits<Function>::function to_function (Function& lambda) {
  return typename function_traits<Function>::function(lambda);
}

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  auto f = []() -> int {
    std::cout << "test" << std::endl;
    return 1;
  };
  std::cout << to_function(f)() << std::endl;;
  return 0;
}
#endif // TEST37

#ifdef TEST36
#define TEST36_TAG "multi core test"
const char* testDescription = "TEST36 2016-06-24-Fri 21:18:51 " TEST36_TAG;
#include <iostream>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  double d = 1000.0;
  for (;;) {
    d *= 1.1f;
    d /= 1.1f;
  }
  return 0;
}
#endif // TEST36

#ifdef TEST35
#define TEST35_TAG "atoi"
const char* testDescription = "TEST35 2016-06-23-Thu 14:43:35 " TEST35_TAG;
#include <iostream>
#include <cstdlib>
#include <string>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::string str = "1234";
  int tmp = std::atoi(str.c_str());
  std::cout << tmp << std::endl;
  return 0;
}
#endif // TEST35

#ifdef TEST34
#define TEST34_TAG "binary to hex string"
const char* testDescription = "TEST34 2016-06-16-Thu 15:51:37 " TEST34_TAG;
#include <iostream>

//using namespace std;

std::string binary_to_hex_string(const char* binary, size_t length,
      bool is_upper = true) {
  const char* const lut = "0123456789abcdef";
  std::string ret = "";
  for (size_t i = 0; i < length; ++i) {
    ret.push_back(lut[binary[i] >> 4] | (is_upper ? 32 : 0));
    ret.push_back(lut[binary[i] & 0xf] | (is_upper ? 32 : 0));
  }
  ret += "";
  return ret;
}

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  char b[100];
  b[0] = 1;
  b[1] = 127;
  std::cout << binary_to_hex_string(b, 2, false) << std::endl;
  return 0;
}
#endif // TEST34

#ifdef TEST33
#define TEST33_TAG "union memory alignment"
const char* testDescription = "TEST33 2016-05-14-Sat 23:37:59 " TEST33_TAG;
#include <iostream>

//using namespace std;
struct T {
    uint32_t id:24;
    uint32_t ts:16;
    uint32_t off:32;
};
struct T1 {
    uint32_t id:24;
    uint32_t ts:16;
    uint64_t off:32;
};

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::cout << sizeof(T) << '\n';
  std::cout << sizeof(T1) << '\n';
  return 0;
}
#endif // TEST33

#ifdef TEST32
#define TEST32_TAG "test memory address"
const char* testDescription = "TEST32 2016-04-08-Fri 11:03:15 " TEST32_TAG;
#include <iostream>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  char a;
  char b;
  int c;
  
  // stack address decrease by sequence of declaration
  // &a > &b > &c
  printf("&a %ld\n", &a);
  printf("&b %ld\n", &b);
  printf("&c %ld\n", &c);
  printf("&a %p\n", &a);
  printf("&b %p\n", &b);
  printf("&c %p\n", &c);
//   std::cout << (&a) << std::endl;
//   std::cout << reinterpret_cast<long>(&a) << std::endl;
//   std::cout << reinterpret_cast<long>(&b) << std::endl;

  return 0;
}
#endif // TEST32

#ifdef TEST31
#define TEST31_TAG "test negative index of array"
const char* testDescription = "TEST31 2016-04-06-Wed 14:43:37 " TEST31_TAG;
#include <iostream>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;

  return 0;
}
#endif // TEST31

#ifdef TEST31
#define TEST31_TAG "test negative index of array"
const char* testDescription = "TEST31 2016-04-06-Wed 14:43:37 " TEST31_TAG;
#include <iostream>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;

  return 0;
}
#endif // TEST31

#ifdef TEST30
#define TEST30_TAG "test wjy"
const char* testDescription = "TEST30 2016-02-19-Fri 17:48:32 " TEST30_TAG;
#include <iostream>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;

  return 0;
}
#endif // TEST30

#ifdef TEST29
#define TEST29_TAG "n sum k"
const char* testDescription = "TEST29 2016-02-18-Thu 11:27:15 " TEST29_TAG;
#include <iostream>

//using namespace std;

int a[10];
int m[10];
int K = 10;
int dp[11][11];

// dp[i + 1][j] := if sum of prev a[i] == j
// if dp[i + 1][j] == true then dp[i][j - a[i] * k] == true, where 0 <= k <= m[i]
// on the other hand, if dp[i][j - a[i] * k] == true, then dp[i + 1][j] === true
void solve() {
  dp[0][0] = 0;
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j <= K; ++j) {
      for (int k = 0; k <= m[i] && k * a[i] <= j; ++k) {
        dp[i + 1][j] |= dp[i][j - k * a[i]];
      }
    }
  }
  if (dp[10][K]) {
    std::cout << "Yes" << std::endl;
  } else {
    std::cout << "No" << std::endl;
  }
}

// dp[i + 1][j] := num of a[i] remain when sum of prev a[i] == j,
// dp[i + 1][j] = m[i], when dp[i][j] >= 0
// dp[i + 1][j] = -1, sum of prev a[i] cannot be j, j < a[i] or dp[i + 1][j - a[i]] < 0
// dp[i + 1][j] = dp[i + 1][j - a[i]] - 1, other situations
void solve1() {
  dp[0][0] = 0;
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j <= K; ++j) {
      if (dp[i][j] >= 0) {
        dp[i + 1][j] = m[i];
      } else if (j < a[i] || dp[i + 1][j - a[i]] < 0) {
        dp[i + 1][j] = -1;
      } else {
        dp[i + 1][j] = dp[i + 1][j - a[i]] - 1;
      }
    }
  }
  if (dp[10][K] >= 0) {
    std::cout << "Yes" << std::endl;
  } else {
    std::cout << "No" << std::endl;
  }
}

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;

  return 0;
}
#endif // TEST29

#ifdef TEST28
#define TEST28_TAG "test random number"
const char* testDescription = "TEST28 2016-02-05-Fri 13:32:51 " TEST28_TAG;
#include <iostream>
#include <cstdlib>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  for (int i = 0; i < 100; ++i) {
    std::cout << rand()%100 << std::endl;
  }

  std::cout << std::endl;

  for (int i = 0; i < 100; ++i) {
    std::cout << rand()%100 << std::endl;
  }
  return 0;
}
#endif // TEST28

#ifdef TEST27
#define TEST27_TAG "test shard key"
const char* testDescription = "TEST27 2016-01-28-Thu 18:12:47 " TEST27_TAG;
#include <iostream>
#include <openssl/md5.h>

//using namespace std;


// void GenUrl(const std::string& bucket, const std::string& object, std::string& url) {
//     url.append("bs://");
//     url.append(bucket);
//     url.append("/");
//     url.append(object);
// }
// const int MD5_LEN = 16;
// int32_t GetShardKey(const std::string &bucket, const std::string &object) {
//   std::string url = "bs://zyb-hiphotos/3801213fb80e7bec5e56593c292eb9389a506b5a.jpg";
// //  GenUrl(bucket, object, url);
// //   std::string md5sum = MD5(url);
//   std::string md5sum="258f28bab9a74294e21c75d87e341ff6";
//   unsigned char* p = reinterpret_cast<const unsigned char*>(md5sum.data());
//   unsigned char temp[MD5_LEN/4] = {};
//   for (size_t i = 0; i < 4; i++) {
//     temp[i] = p[i] ^ p[i + 4] ^ p[i + 8] ^ p[i + 12];
//   }
//   uint32_t shard_key = 0;
//   for (size_t i = 0; i < 4; i++) {
//     shard_key << 4;
//     shard_key += static_cast<uint32_t>(temp[i]);
//   }
//   return static_cast<int32_t>(shard_key % BUCKET_SHARD_NUM);
// }
// 
int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::string str = "123";
  unsigned char * tmp_hash;
  tmp_hash = MD5(str.data(), str.length(), NULL);
//   std::cout << GetShardKey(str, str, str) << std::endl;
  return 0;
}

#endif // TEST27

#ifdef TEST26
#define TEST26_TAG "test bcs get object name"
const char* testDescription = "TEST26 2016-01-12-Tue 15:07:01 " TEST26_TAG;
#include <iostream>
#include <sstream>
#include <vector>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::string value = "bs://engine/340e2d4a08fc0dda15913cd4/bdb25/d1d";
  std::stringstream ss(value);
  std::vector<std::string> result;
  std::string item;
  while (std::getline(ss, item, '/')) {
    result.push_back(item);
  }
  std::stringstream tmp;
  for (int i = 3; i < result.size() - 1; ++i) {
    tmp << result[i] << "/";
  }
  tmp << result[result.size() - 1];
  if (value.find_last_of("/") == value.size() - 1) {
    tmp << "/";
  }

  std::cout << "bucket: " << result[2] << std::endl;
  std::cout << "object: " << tmp.str() << std::endl;

  return 0;
}
#endif // TEST26

#ifdef TEST25
#define TEST25_TAG "test fstream append create"
const char* testDescription = "TEST25 2016-01-11-Mon 22:35:27 " TEST25_TAG;
#include <iostream>
#include <fstream>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::ofstream fs;
  fs.open("/Users/gavin/tmp/tmp", std::ios::app);
  if (fs.fail()) {
    std::cout << "fail" << std::endl;
    return -1;
  }
  fs << "test" << std::endl;

  return 0;
}
#endif // TEST25

#ifdef TEST24
#define TEST24_TAG "test member function pointer"
const char* testDescription = "TEST24 2015-12-03-Thu 21:46:09 " TEST24_TAG;
#include <iostream>

//using namespace std;

class A;

// general fun pointer
typedef void (*FunPtr)();
// class mem fun pointer
typedef void (A::*CFunPtr)();

class A {
public:
  void a() {
    std::cout << __func__ << std::endl;
  }
  static void b() {
    std::cout << __func__ << std::endl;
  }

  FunPtr pCFunA;
  FunPtr pFunB;

  A() {
//     pCFunA = A::a;
    pFunB = A::b;
  }

  static virtual void c() {
    std::cout << __func__ << std::endl;
  }
};

class B: public A {
  public:
    void c() {
      std::cout << "this B::c" << std::endl;
    }
};


void some_api(FunPtr fp) {
  fp();
}

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  A a;
//   a.pFunA = a.a;
  a.pFunB = A::b;
  some_api(a.pFunB);

  B b;
  b.c();
  return 0;
}
#endif // TEST24

#ifdef TEST23
#define TEST23_TAG "uninit class member test"
const char* testDescription = "TEST23 2015-11-23-Mon 21:23:19 " TEST23_TAG;
#include <iostream>

//using namespace std;
class A {
public:
  int a;
  int get_a() {
    return a;
  }
};

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  for (int i = 0; i < 1000; ++i) {
    A a;
    if (a.get_a() != 0 ) {
      std::cout << a.get_a() << std::endl;
    }
  }

  return 0;
}
#endif // TEST23

#ifdef TEST22
#define TEST22_TAG "utc cst gmt unix time test"
const char* testDescription = "TEST22 2015-11-19-Thu 15:30:34 " TEST22_TAG;
#include <iostream>
#include <ctime>

//using namespace std;

int main(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) { std::cout << argv[i] << std::endl; }
  std::cout << testDescription << std::endl;
  time_t cur_time = time(NULL);
  if(cur_time < 0) {
    perror("time");
    return -1;
  }

  struct tm utc_tm;;
  if(NULL == gmtime_r(&cur_time, &utc_tm)) {
    perror("gmtime");
    return -1;
  }

  struct tm local_tm;
  if(NULL == localtime_r(&cur_time, &local_tm)) {
    perror("localtime");
    return -1;
  }

  printf("UTC = %s", asctime(&utc_tm));
  printf("LOC = %s", asctime(&local_tm));
  printf("LOC = %s", ctime(&cur_time));

// #include <chrono> // system clock
// #include <ctime> // std::gmtime, std::mktime, std::strftime, ::strptime
// 
// using namspace std::chrono
// int64_t ts = duration_cast<seconds>(system_clock()).count();
// 
// std::time_t epoch = std::time(nullptr); // timestamp from time()
// epoch = static_cast<time_t>(ts); // convert from system clock
// 
// std::tm* utc_tm = std::gmtime(epoch); // epoch to tm, greenwich mean time, utc
// std::tm* local_tm = std::localtime(time_t);
// 
// std::time_t epoch_a = std::mktime(utc_tm); // get epoch from tm
// std::time_t epoch_b = std::mktime(local_tm); // get epoch from tm
// 
// float time_zone = (epoch_b - epoch_a) / 3600.0f;
// 
// // conversion
// // format a tm
// size_t sz = std::strftime(buf, max_size, time_fmt, ptm);
// char* p_1st_char_not_processed = ::strptime(time_string, time_fmt, ptm); // not in std lib, a posix interface

  return 0;
}
#endif // TEST22

#ifdef TEST21
#define TEST21_TAG "system() output"
const char* testDescription = "TEST21 2015-11-12-Thu 19:28:47 " TEST21_TAG;
#include <iostream>
#include <cstdlib>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
//   const char* cmd = "/bin/ls -al .";
  const char* cmd = "gettime";
  FILE* fp = popen(cmd, "r");

  if (fp == nullptr) {
    std::cout << "open pipe error" << std::endl;
    return -1;
  }

  char buf[2048];

  while (fgets(buf, sizeof(buf)-1, fp) != NULL) {
//     printf("%s", buf);
    std::cout << buf << std::endl;
  }

  int status = pclose(fp);
  if (status == -1) {
    /* Error reported by pclose() */
    std::cout << "close pipe error" << std::endl;
  } else {
    /* Use macros described under wait() to inspect `status' in order
       to determine success/failure of command executed by popen() */
  }

  return 0;
}
#endif // TEST21

#ifdef TEST20
#define TEST20_TAG "test global time"
const char* testDescription = "TEST20 2015-11-11-Wed 14:09:56 " TEST20_TAG;
#include <iostream>
#include <unistd.h>

//using namespace std;
time_t g_now = time(NULL);

int func() {
  return 10;
}

int a = func();
int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::cout << g_now << std::endl;
  sleep(1);
  std::cout << g_now << std::endl;
  std::cout << time(NULL) << std::endl;
  std::cout << a << std::endl;

  return 0;
}
#endif // TEST20

#ifdef TEST19
#define TEST19_TAG "RFC3986 url encode, normalize string"
const char* testDescription = "TEST19 2015-11-02-Mon 14:51:59 " TEST19_TAG;
#include <iostream>
#include <stdio.h>
#include <ctype.h>

//using namespace std;

// tables for different standards
char rfc3986[256] = {0};
char html5[256] = {0};

/* caller responsible for memory */
void encode(const char *s, char *enc, char *tb)
{
  for (; *s; s++) {
    if (tb[*s]) {
      sprintf(enc, "%c", tb[*s]);
    } else {
      sprintf(enc, "%%%02X", *s);
    }
    while (*++enc);
  }
}
 
int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;

  const char url[] = "http://foo bar/";
  char enc[(strlen(url) * 3) + 1];
 
  int i;
  for (i = 0; i < 256; i++) {
    rfc3986[i] = isalnum(i)||i == '~'||i == '-'||i == '.'||i == '_'
      ? i : 0;
    html5[i] = isalnum(i)||i == '*'||i == '-'||i == '.'||i == '_'
      ? i : (i == ' ') ? '+' : 0;
  }
 
  encode(url, enc, rfc3986);
  puts(enc);
  encode(url, enc, html5);
  puts(enc);
 
  return 0;
}

#endif // TEST19

#ifdef TEST18
#define TEST18_TAG "test splilt string"
const char* testDescription = "TEST18 2015-10-24-Sat 18:19:14 " TEST18_TAG;
#include <iostream>
#include <sstream>
#include <curl/curl.h>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
//   std::string str = "test/fdsaf/afdsaf/dsafdsa/";
//   std::stringstream ss(str);
//   std::string item;
//   while (std::getline(ss, item, '\0')) {
//     std::cout << item << std::endl;
//   }

  std::string url = "/abc/def/gdsajkfdlsaj//// fjdkslafj///";
  url = "abc/def/gdsajkfdlsaj/ fjdkslafj";
  url = "abc/def/gdsajkfdlsaj/ fjdkslafj/";
  url = "";
  url = "fdjskal";
  std::stringstream ss(url);
  std::string item;
  std::string result = "";
  char skip = '/';
//   CURL* _curl_handle = curl_easy_init();
  while (std::getline(ss, item, skip)) {
    char* output = curl_easy_escape(NULL, item.c_str(), item.length());
    std::string str = output;
    std::cout << item << skip << std::endl;
    curl_free(output);
    result += str + skip;
  }
  if (url.length() > 0 && url[url.length() - 1] != skip) {
    result = result.substr(0, result.length() - 1);
  }

  std::cout << result << std::endl;

  return 0;
}

#endif // TEST18

#ifdef TEST17
#define TEST17_TAG "curl escape"
const char* testDescription = "TEST17 2015-10-23-Fri 18:29:32 " TEST17_TAG;
#include <iostream>
#include <curl/curl.h>

//using namespace std;

std::string normalize_string(const std::string& src) {
  std::string rc;

  char b, c;
  for (std::string::const_iterator it = src.begin(); it != src.end(); ++it) {
    c = *it;
    if (isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~') {
      rc += c;
      continue;
    }
    rc += '%';
    b =  c >> 4;
    rc += (char)(b > 9 ? b + 0x37 : b + 0x30);
    b = c & 0xf;
    rc += (char)(b > 9 ? b + 0x37 : b + 0x30);
  }

  return rc;
}

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  CURL *curl = curl_easy_init();
  if(curl) {
    std::string str = "`~!@#$%^&*()-_+ ,./;'[]\\|}{\":?><";
    for (auto each : str) {
      std::string tmp = std::string("") + each;
      char *output = curl_easy_escape(curl, tmp.c_str(), 1);
      if(output) {
        // printf("Encoded: %s\n", output);
        printf("%s %s\n", tmp.c_str(), output);
        curl_free(output);
      }
    }
  }
  std::cout << std::endl;
  std::string str = "`~!@#$%^&*()-_+ ,./;'[]\\|}{\":?><";
  for (auto each : str) {
    std::string tmp = std::string("") + each;
    std::cout << tmp << " " << normalize_string(tmp) << std::endl;
  }
  return 0;
}

#endif // TEST17

#ifdef TEST16
#define TEST16_TAG "test substr"
const char* testDescription = "TEST16 2015-10-21-Wed 14:58:41 " TEST16_TAG;
#include <iostream>
#include <string>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;

  std::string str = "spretr";
  std::string prefix = "pre";
  std::string sub_str = str.substr(0, prefix.length());

  std::cout << sub_str << std::endl;

  return 0;
}

#endif // TEST16

#ifdef TEST15
#define TEST15_TAG "test ++i i++ efficiency, there is no big difference"
const char* testDescription = "TEST15 2015-10-21-Wed 10:32:23 " TEST15_TAG;
#include <iostream>
#include <chrono>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::chrono::time_point<std::chrono::system_clock> _start_time_, _end_time_;
  std::chrono::duration<double> _elapsed_time_;
   _start_time_ = std::chrono::system_clock::now();

  int i, j;
  for (i = 0; i < 1000000000; ++i);
  _end_time_ = std::chrono::system_clock::now();
  _elapsed_time_ = _end_time_ - _start_time_;
  std::cout << _elapsed_time_.count() << " seconds" << std::endl;

   _start_time_ = std::chrono::system_clock::now();
  for (j = 0; j < 1000000000; j++);

  _end_time_ = std::chrono::system_clock::now();
  _elapsed_time_ = _end_time_ - _start_time_;
  std::cout << _elapsed_time_.count() << " seconds" << std::endl;

  return 0;
}

#endif // TEST15

#ifdef TEST14
#define TEST14_TAG "test intersection"
const char* testDescription = "TEST14 2015-10-16-Fri 12:59:25 " TEST14_TAG;
#include <iostream>
#include <vector>

//using namespace std;
int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;

  std::vector<int> node_ids_from_zk{-5, -2, -1, 1, 5, 7, 8, 9};
  std::vector<int> node_ids_from_db{0, 2, 5, 6, 8, 9, 10, 11};
  std::vector<int> ids_2_delete;

  size_t i = 0;
  size_t j = 0;
  size_t last_j = j;
  size_t zk_ids_count = node_ids_from_zk.size();
  size_t db_ids_count = node_ids_from_db.size();
  sort(node_ids_from_zk.begin(), node_ids_from_zk.end());
  sort(node_ids_from_db.begin(), node_ids_from_db.end());
  while (i < zk_ids_count && j < db_ids_count) {
    while ((i < zk_ids_count && j < db_ids_count)) {
      if (node_ids_from_zk[i] < node_ids_from_db[j]) {
        ++i;
      } else if (node_ids_from_zk[i] > node_ids_from_db[j]) {
        ++j;
      } else {
        break;
      }
    }
    if (i < zk_ids_count && j < db_ids_count) {
      for (size_t k = last_j; k < j; ++k) {
        ids_2_delete.push_back(node_ids_from_db[k]);
      }
      // skip intersection element
      ++i;
      ++j;
      last_j = j;
    }
  }
  std::cout << i << " " << j << std::endl;
  if (j < db_ids_count) {
    while (j < db_ids_count) {
      ids_2_delete.push_back(node_ids_from_db[j]);
      ++j;
    }
  } else if (
      node_ids_from_zk[zk_ids_count - 1] > node_ids_from_db[db_ids_count - 1]) {
    for (size_t k = last_j; k < db_ids_count; ++k) {
      ids_2_delete.push_back(node_ids_from_db[k]);
    }
  }

  for (auto each : ids_2_delete) {
    std::cout << each << std::endl;
  }

  return 0;
}

#endif // TEST14

#ifdef TEST13
#define TEST13_TAG "container duplication"
const char* testDescription = "TEST13 2015-10-15-Thu 20:10:36 " TEST13_TAG;
#include <iostream>
#include <set>
#include <map>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::set<std::string> s;
  std::set<std::string> s1;
  std::set<std::string> s2;
  s.insert("first");
  s.insert("second");
  s.insert("third");
  s1 = s;
  s.clear();
  s.insert("test");

  for (auto each : s1) {
    std::cout << each << std::endl;
  }

  std::cout << "construct large set" << std::endl;
  std::string str = "string";
  char buf[100];
  for (int i = 0; i < 1000000; ++i) {
    sprintf(buf, "%d", i);
    s.insert(buf);
  }

  std::cout << "copy the large set" << std::endl;
  // copy is a little faster than insert? copy all elements
  s1 = s;

  return 0;
}

#endif // TEST13

#ifdef TEST12
#define TEST12_TAG "container erase test"
const char* testDescription = "TEST12 2015-10-15-Thu 19:54:18 " TEST12_TAG;
#include <iostream>
#include <set>
#include <map>
#include <vector>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::map<int, std::string> m;
  m[1] = "first";
  m[2] = "second";
  std::set<int> s;
  s.insert(10);
  s.insert(11);
  s.insert(12);

  for (auto each : m) {
    std::cout << each.first << " " << each.second << std::endl;
  }

  for (auto each : s) {
    std::cout << each << std::endl;
  }

  m.erase(1);
  m.erase(3);
  for (auto each : m) {
    std::cout << each.first << " " << each.second << std::endl;
  }
  s.erase(11);
  s.erase(13);
  for (auto each : s) {
    std::cout << each << std::endl;
  }

  std::vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  v.push_back(4);

  return 0;
}

#endif // TEST12

#ifdef TEST11
#define TEST11_TAG "custome std::pair test"
const char* testDescription = "TEST11 2015-10-15-Thu 13:28:40 " TEST11_TAG;
#include <iostream>
#include <map>

// define macro to generate custom pair
#define CUSTOM_PAIR(pair_name, type1, type2, name1, name2) \
  struct pair_name : public std::pair<type1, type2> { \
    pair_name() : name1(first), name2(second) {} \
    pair_name(type1 a, type2 b) : \
        std::pair<type1, type2>(a, b), name1(first), name2(second) {} \
    type1& name1; \
    type2& name2; \
  };

// declare it
CUSTOM_PAIR(Person, std::string, int, name, age);

// concrete example
struct Range : public std::pair<int, int> {
  Range() : lowerbound(first), upperbound(second) {}
  Range(int a, int b) :
      std::pair<int, int>(a, b), lowerbound(first), upperbound(second) {}
  int& lowerbound;
  int& upperbound;
};

// function uses std::pair<>
void f(const std::pair<int, int>& p) {
  std::cout << p.first << std::endl;
  std::cout << p.second << std::endl;
}

// function uses std::pair<>
void f1(const std::pair<std::string, int>& p) {
  std::cout << p.first << std::endl;
  std::cout << p.second << std::endl;
}

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  Range mp;
  mp.lowerbound = 1;
  mp.upperbound = 2;
  f(mp);
  Range mp2(3, 4);
  f(mp2);

  Person sp1;
  sp1.name = "gavin";
  sp1.age = 26;
  Person sp2("aaa", 10);
  f1(sp1);
  f1(sp2);

  return 0;
}

#endif // TEST11

#ifdef TEST10
#define TEST10_TAG "stringstream to string test"
const char* testDescription = "TEST10 2015-10-09-Fri 15:44:48 " TEST10_TAG;
#include <iostream>
#include <sstream>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::stringstream ss;
  ss << "a" << "b";
  std::string str(ss.str());
  std::cout << str << std::endl;

  return 0;
}

#endif // TEST10

#ifdef TEST9
#define TEST9_TAG "inner class"
const char* testDescription = "TEST9 2015-10-08-Thu 20:34:58 " TEST9_TAG;
#include <iostream>
#include <vector>

//using namespace std;

class A {
public:
  struct B {
    int a;
    int b;
  };
  std::vector<B> vb;
  void func() {
    A::B b;
    b.a = 1;
    b.b = 2;
    vb.push_back(b);
    B b2;
    b2.a = 3;
    b2.b = 4;
    vb.push_back(b2);
  }
  void f2(B b) {
    vb.push_back(b);
  }
};


int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  A a;
  a.func();
  A::B b{5, 6};
  a.f2(b);

  for (auto each : a.vb) {
    std::cout << each.a << std::endl;
    std::cout << each.b << std::endl;
  }

  return 0;
}

#endif // TEST9

#ifdef TEST8
#define TEST8_TAG "test return reference life cycle"
const char* testDescription = "TEST8 2015-09-28-Mon 19:31:29 " TEST8_TAG;
#include <iostream>
#include <string>

using namespace std;

class A {
public:
  A(const std::string& f) {
    field = new string(f);
  }
  const std::string& get_field() const {
    return *field;
  }
  const std::string* get() const {
    return field;
  }
  std::string* field;
  ~A() {
    if (field != nullptr) {
      delete field;
      field = nullptr;
    }
  }
};

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  string str;
  {
    A a("test");
    cout << a.get_field() << endl;
    str = a.get_field();
  }
  cout << str << endl;

  {
    A a("test 2");
    string str1 = a.get_field();
    const string* str2 = a.get();
    cout << "before A ~tor str1: " << str1 << endl;
    cout << "before A ~tor str2: " << *str2 << endl;
    a.~A();
    cout << "after A ~tor BANG? str1: " << str1 << endl;
    cout << "after A ~tor BANG!!! str2: " << *str2 << endl;
  }

  return 0;
}

#endif // TEST8

#ifdef TEST7
#define TEST7_TAG "test utf8"
const char* testDescription = "TEST7 2015-09-24-Thu 19:04:30 " TEST7_TAG;
#include <iostream>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::string str = "中文";
  std::cout << str.size()  << std::endl;
  std::cout << str  << std::endl;
  std::cout << str.c_str() << std::endl;
  std::cout << str.data() << std::endl;

  std::string str1 = "";
  std::cout << str1.size()  << std::endl;
  std::cout << str1  << std::endl;
  std::cout << str1.c_str() << std::endl;
  std::cout << str1.data() << std::endl;

  return 0;
}

#endif // TEST7

#ifdef TEST6
#define TEST6_TAG "class declearation"
const char* testDescription = "TEST6 2015-09-22-Tue 16:34:52 " TEST6_TAG;
#include <iostream>

using namespace std;
class B;
class A {
public:
  void print(const string& str) {
    cout << str << endl;
  }
  friend class B;
};

class B {
public:
  void print(std::string& str) {
    A a;
    a.print(str);
    cout << str << endl;
  }
private:
  void print1(int ) {
    cout << endl;
  }
};

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  return 0;
}

#endif // TEST6

#ifdef TEST5
#define TEST5_TAG "charbuf to string test&"
const char* testDescription = "TEST5 2015-09-10-Thu 16:01:38 " TEST5_TAG;
#include <iostream>
#include <string>

//using namespace std;

void f1(const std::string str) {

  std::cout << __func__ << std::endl;
  std::cout << str << std::endl;
}

void f2(const std::string& str) {

  std::cout << __func__ << std::endl;
  std::cout << str << std::endl;
}

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;

  char str[1024];
  sprintf(str, "test %s", "string&");

  f1(str);
  f2(str);

  return 0;
}

#endif // TEST5

#ifdef TEST4
#define TEST4_TAG "last iterator"
const char* testDescription = "TEST4 2015-08-29-Sat 12:45:36 " TEST4_TAG;
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  vector<int> vi;
  for (int i = 0; i < 10; ++i) {
    vi.push_back(i);
  }

  vector<int>::iterator it = vi.end();
  while (true) {
    --it;
    cout << *it << endl;
    if (it == vi.begin()) break;
  }
  return 0;
}

#endif // TEST4

#ifdef TEST3
#define TEST3_TAG "int test "
const char* testDescription = "TEST3 2015-08-28-Fri 20:06:24 " TEST3_TAG;
#include <iostream>
#include <cstdint>
#include <cstdio>

using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
//   int64_t a = ((int64_t)2 << 30);
//   cout << a << endl;
//   cout << (a >> 30) << endl;
  unsigned char buf[100] = {0, 0, 0, 1, 0, 0, 0, 0, (unsigned char)255, 0, 0, (unsigned char)255};
  int c = 128;
  memcpy(&c, buf, 8);
  printf("%d %ld\n", (unsigned char)255, *(int*)buf);
  return 0;
}

#endif // TEST3


#ifdef TEST2

#define TEST2_TAG "inner struct "
const char* testDescription = "TEST2 2015-08-27-Thu 12:39:35 " TEST2_TAG;
#include <iostream>
#include <vector>

struct Pipelet {
    std::string     pipe;
    int             id;
    struct {
        int64_t     pipelet_msg_id;
        uint32_t    seq_id;
    } pos;
    struct Pos {
        int64_t     pipelet_msg_id;
        uint32_t    seq_id;
    };
    Pos pos1;
    std::vector<int64_t>   unsend_window;
};


int main(int argc, char** argv) {
  int a = (2 << 30);
  unsigned int b = (2 << 30);
  std::cout << a << std::endl << b << std::endl;
  std::cout << testDescription << std::endl;

  Pipelet pplt;
  pplt.pos.pipelet_msg_id = 10;
  pplt.pos1.seq_id = 2;

  std::cout << pplt.pos.pipelet_msg_id << std::endl;
  std::cout << pplt.pos1.seq_id << std::endl;

  return 0;
}

#endif // TEST2



#ifdef TEST1
#define TEST1_TAG "strftime"
const char* testDescription = "TEST1 20:30:40 " TEST1_TAG;
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;

  for (int i = 0; i < 1000; ++i) {
  struct timeval curTime;
  gettimeofday(&curTime, NULL);
  int milli = curTime.tv_usec / 1000;

  time_t now = time(NULL);
  struct tm* tm = localtime(&now);
  char buf[100];
  size_t len = strftime(buf, 100, "%G-%m-%d %H:%M:%S.", tm);
  // std::cout << len << std::endl;
  std::cout << buf << milli << std::endl;
  usleep(1000);
  }
  return 0;
}

#endif // TEST1

#ifdef linux_TEST64
#define linux_TEST64_TAG "std::string resize/capacity test"
const char* testDescription = "linux_TEST64 2016-10-17-Mon 11:28:46 " linux_TEST64_TAG;
#include <iostream>
#include <string>
#include <typeinfo>
#include <iomanip>

//using namespace std;

int main(int argc, char** argv) {
  for (int i = 0; i < argc; ++i) { std::cout << argv[i] << " "; }
  std::cout << std::endl << testDescription << std::endl;
  std::string str1 = "abc";
  std::cout << typeid(std::string::pointer).name() << std::endl;

  std::ios::fmtflags fmt_falgs(std::cout.flags());

  std::cout << std::hex << reinterpret_cast<int64_t>(&str1[0]) << std::endl;
  std::cout << std::hex << reinterpret_cast<int64_t>(str1.c_str()) << std::endl;
  auto pstr1 = &str1;
  std::cout << std::hex << reinterpret_cast<int64_t>(&(*pstr1)[0]) << std::endl;
  str1.resize(1024 * 1024 * 10, 0);
  str1.reserve(1024 * 1024 * 10 + 1);

  std::cout << std::hex << reinterpret_cast<int64_t>(&(*pstr1)[0]) << std::endl;
  str1.clear();
  std::cout.flags(fmt_falgs);
  std::cout << "cap: " << str1.capacity() << std::endl;
  str1.resize(1, 0);
  std::cout << "cap: " << str1.capacity() << std::endl;
  str1.resize(1024 * 1024 * 10 + 1, 0);
  std::cout << "cap: " << str1.capacity() << std::endl;


  return 0;
}
#endif // linux_TEST64


#ifdef linux_TEST29
#define linux_TEST29_TAG "test yang"
const char* testDescription = "linux_TEST46 2016-08-17-Wed 20:30:21 " linux_TEST29_TAG;
#include <vector>
#include <iostream>

using namespace std;
struct Stat {
    Stat() {
        std::cout << "contruct\n";
    }
    Stat& operator=(const Stat& oth) {
        id = oth.id;
        used = oth.used;
        count = oth.count;
        std::cout << id << " assign\n";
        return *this;
    }
    Stat(const int& i, const int& u, const int& cnt) {
        id = i;
        used = u;
        count = cnt;
        std::cout << id<< "construct2\n";
    }

    /*
    Stat(Stat&& oth) {
        id = oth.id;
        used = oth.used;
        count = oth.count;
        std::cout << id<< "move construct\n";
    }
*/
    Stat(const Stat& oth) {
        id = oth.id;
        used = oth.used;
        count = oth.count;
        std::cout << id<< "copied\n";
    }
/*
    Stat& operator=(Stat&& oth) {
        id = oth.id;
        used = oth.used;
        count = oth.count;
        std::cout << id<< "move assign\n";
        return *this;
    }
    */
    int id = 0;
    int used = 0;
    int count = 0;

//     int id;
//     int used;
//     int count;
};

int main() {
    vector<Stat> vec;

    Stat st;
    Stat st2;
    st2.id = 2;
    Stat st3;
    st3.id = 3;
    Stat st4;

    vec.reserve(16);
    cout << "========vector push back===========\n";
    vec.push_back(st2);

    cout << "========vector insert===========\n";
    vec.insert(vec.begin(),  st3);
}
#endif // linux_TEST46

#ifdef linux_TEST28
#define linux_TEST28_TAG ""
const char* testDescription = "linux_TEST28 2016-07-15-Fri 18:03:51 " linux_TEST28_TAG;
#include <iostream>
#include <cstdio>
#include <functional>

//using namespace std;


//template <typename ReturnType>
//ReturnType do_until(ReturnType(*job)(),
//                    bool(*judge)(ReturnType),
//                    int times = 10) {
//    int i = 0;
//    do {
//        ReturnType ret = job();
//
//        if (judge(ret)) {
//            return ret;
//        }
//        printf("failed\n");
//        i++;
//    } while (i < times);
//}

template <typename ReturnType>
ReturnType do_until_base(std::function<ReturnType()> job,
                        std::function<bool(ReturnType)> judge,
                        int times = 10) {
    int i = 0;
    do {
        ReturnType ret = job();
        if (judge(ret)) {
            return ret;
        }
        printf("failed\n");
        i++;
    } while (i < times);
}

template <typename ReturnType>
ReturnType do_until(ReturnType(*job)(),
                    bool(*judge)(ReturnType),
                    int times = 10) {
    return do_until_base<ReturnType>(job, judge, times);
}
template <typename ReturnType, typename ClassType>
ReturnType do_until(ReturnType(ClassType::* job)(),
                    bool(*judge)(ReturnType),
                    int times = 10) {
    return do_until_base<ReturnType>(job, judge, times);
}

int test() {
    return -1;
}

bool judge(int ret) {
    return ret == 0;
}

struct aaa {
  int test() {
    return -1;
  }
};

int main(int argc, char *argv[]) {
  std::cout << testDescription << std::endl;
    auto test1 = [](){return -1;};
    do_until(a::test, judge);
    return 0;
}

#endif // linux_TEST28

std::string binary_to_hex_string(const char* binary, size_t length,
    bool is_upper = true) {
  const char* const lut = "0123456789ABCDEF";
  std::string ret = "";
  for (size_t i = 0; i < length; ++i) {
    ret.push_back(lut[binary[i] >> 4]);
    ret.push_back(lut[binary[i] & 0xf]);
  }
  ret += "";
  return ret;
}

#ifdef linux_TEST25
#define linux_TEST25_TAG "test memory address"
const char* testDescription = "linux_TEST25 2016-04-08-Fri 11:03:15 " linux_TEST25_TAG;
#include <iostream>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  char a;
  char b;
  int c;

  //   printf("%ld\n", &a);
  //   printf("%ld\n", &b);
  //   printf("%ld\n", &c);
  //   std::cout << (&a) << std::endl;
  std::cout << reinterpret_cast<long>(&a) << std::endl;
  std::cout << reinterpret_cast<long>(&b) << std::endl;
  std::cout << reinterpret_cast<long>(&c) << std::endl;

  return 0;
}
#endif // linux_TEST25


#ifdef linux_TEST24
#define linux_TEST24_TAG "md5"
const char* testDescription = "linux_TEST24 2016-02-04-Thu 11:05:44 " linux_TEST24_TAG;
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>

int main()
{
  unsigned char digest[MD5_DIGEST_LENGTH];
  char string[] = "happy";

  MD5((unsigned char*)&string, strlen(string), (unsigned char*)&digest);    

  char mdString[33];

  for(int i = 0; i < 16; i++) {
    sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
  }

  printf("md5 digest: %s\n", mdString);

  return 0;
}

#endif // linux_TEST24

#ifdef linux_TEST23
#define linux_TEST23_TAG "uninit class member test"
const char* testDescription = "linux_TEST23 2015-11-23-Mon 21:23:19 " linux_TEST23_TAG;
#include <iostream>

//using namespace std;
class A {
  public:
    int a;
    int get_a() {
      return a;
    }
};

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  for (int i = 0; i < 1000; ++i) {
    A a;
    if (a.get_a() != 0 ) {
      std::cout << a.get_a() << std::endl;
    }
  }

  return 0;
}
#endif // linux_TEST23

#ifdef linux_TEST7
#define linux_TEST7_TAG "curl escape"
const char* testDescription = "linux_TEST7 2015-10-23-Fri 18:34:23 " linux_TEST7_TAG;
#include <iostream>
#include <curl/curl.h>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  CURL *curl = curl_easy_init();
  if(curl) {
    char *output = curl_easy_escape(curl, "data to convert", 15);
    if(output) {
      printf("Encoded: %s\n", output);
      curl_free(output);
    }
  }
  return 0;
}

#endif // linux_TEST7



#ifdef linux_TEST6
#define linux_TEST6_TAG "stringstream to string test"
const char* testDescription = "linux_TEST6 2015-10-09-Fri 15:48:00 " linux_TEST6_TAG;
#include <iostream>
#include <sstream>

//using namespace std;

int main(int argc, char** argv) {
  std::cout << testDescription << std::endl;
  std::stringstream ss;
  ss << 123 << "test";
  std::cout << ss.str() << std::endl;
  ss.clear();
  std::cout << ss.str() << std::endl;

  return 0;
}

#endif // linux_TEST6

#ifdef linux_TEST5
#define linux_TEST5_TAG "charbuf to string test&"
const char* testDescription = "linux_TEST5 2015-09-10-Thu 16:01:38 " linux_TEST5_TAG;
#include <iostream>
#include <string>
#include <regex>

//using namespace std;

class String {
  public:
    const char* str;
    String(const char* str) {
      this->str = str;
    }
  private:
    String();
};

// segment fault here when stepping intot this function, why
void f1(const std::string& str) {
  std::cout << __func__ << std::endl;
  std::cout << str << std::endl;
}

void f2(const char* str) {
  std::cout << __func__ << std::endl;
  std::cout << str << std::endl;
}

void f3(std::string str) {
  std::cout << __func__ << std::endl;
  std::cout << str << std::endl;
}

void f4(const std::string str) {
  std::cout << __func__ << std::endl;
  std::cout << str << std::endl;
}

void f5(String str) {
  std::cout << __func__ << std::endl;
  std::cout << str.str << std::endl;
}

int main(int argc, char** argv) {
  // system("clear");
  std::cout << testDescription << std::endl;

  char str[1024];
  sprintf(str, "test %s", "string&");

  f1("test function1");
  f2("test function2");
  f3("test function3");
  f4("test function4");
  f5("test function5");

  return 0;
}

#endif // linux_TEST5

// vim: et sw=2 ts=2 tw=80:
// vim: sw=2 ts=2 et ft=cpp:
