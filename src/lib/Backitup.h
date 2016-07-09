#ifndef BACKITUPX_H_
#define BACKITUPX_H_

#include <string>
#include <thread>

#include "BackupPath.h"
#include "Channel.h"
#include "Index.h"
#include "Storage.h"

namespace backitup {

class BackitupException : public std::runtime_error {
 public:
  BackitupException() : runtime_error("Backitup error") {}
  BackitupException(std::string msg)
      : runtime_error(string(string("Backitup error: ").append(msg)).c_str()) {}
};

class Backitup {
 public:
  Backitup(Index& i, Storage& s);

  void init(BackupPath& b);

  std::thread run(BackupPath& b,
                  function<void(const string& path)> fn = nullptr);

  void stop();

  void sleep_on_empty(bool b) { _sleep_on_empty = b; }

  void interval(pair<string, int> i) { _interval = i; }

  void max_file_size_bytes(unsigned long l) { _max_file_size_bytes = l; }

  vector<string> list_path(string path);

  long restore(string path, string dest);

 private:
  bool process_nl(const string& path, const NodeList& nl);

  std::string _base_path;
  Index& _index;
  Storage& _store;
  Channel<string> _chan;

  bool _sleep_on_empty = true;

  pair<string, int> _interval;

  unsigned long _max_file_size_bytes;

  bool _running = true;

  bool _stopped;
  condition_variable stopped_cv;
  std::mutex _stopped_m;
};
}

#endif /* BACKITUP_H_ */
