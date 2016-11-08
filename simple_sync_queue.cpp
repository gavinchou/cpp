#include <iostream>
#include "simple_sync_queue.hpp"

int main(void) {
    SimpleSyncQueue<int> q;
    q.put(100);
    std::cout << q.empty() << std::endl;
    std::cout << q.size() << std::endl;
    q.clear();
    int a;
    q.get_with_timeout(&a, 10);
    std::cout << a << std::endl;
    return 0;
}
// vim: et tw=100 ts=4 sw=4 cc=100:
