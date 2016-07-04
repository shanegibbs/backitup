#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <set>
#include <thread>

namespace backitup {

/*
 Standard buffered channel, except it only queues unique items.
 put() returns true if the item was not already in the queue.
*/

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

  bool put(const T &i) {
    std::unique_lock<std::mutex> lock(m);
    if (_closed) throw std::logic_error("Channel already closed");
    auto result = queue.insert(i);
    cv.notify_one();
    return result.second;
  }

  bool get(T &out, bool wait = true) {
    std::unique_lock<std::mutex> lock(m);
    if (wait) cv.wait(lock, [&]() { return _closed || !queue.empty(); });
    if (_closed) return false;
    if (queue.empty()) return false;
    out = *queue.begin();
    queue.erase(out);
    return true;
  }

  bool empty() {
    std::unique_lock<std::mutex> lock(m);
    return queue.empty();
  }

  void reset() {
    std::unique_lock<std::mutex> lock(m);
    queue.clear();
    _closed = false;
  }

 private:
  std::set<T> queue;
  std::mutex m;
  std::condition_variable cv;
  bool _closed;
};
}

#endif /* CHANNEL_H_ */
