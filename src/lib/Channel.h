#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <list>
#include <thread>

namespace backitup {

template <class T>
class Channel {
 public:
  Channel() : _closed(false) {}

  void close() {
    std::unique_lock<std::mutex> lock(m);
    _closed = true;
    cv.notify_all();
  }

  bool closed() {
    std::unique_lock<std::mutex> lock(m);
    return _closed;
  }

  void put(const T &i) {
    std::unique_lock<std::mutex> lock(m);
    if (_closed) throw std::logic_error("put to closed channel");
    queue.push_back(i);
    cv.notify_one();
  }

  bool get(T &out, bool wait = true) {
    std::unique_lock<std::mutex> lock(m);
    if (wait) cv.wait(lock, [&]() { return _closed || !queue.empty(); });
    if (_closed) return false;
    if (queue.empty()) return false;
    out = queue.front();
    queue.pop_front();
    return true;
  }

 private:
  std::list<T> queue;
  std::mutex m;
  std::condition_variable cv;
  bool _closed;
};
}

#endif /* CHANNEL_H_ */
