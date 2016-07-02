#ifndef BACKITUPX_H_
#define BACKITUPX_H_

#include <string>

#include "BackupPath.h"
#include "Channel.h"
#include "Index.h"
#include "Storage.h"

namespace backitup {

class Backitup {
 public:
  Backitup(Index& i, Storage& s);

  void init(BackupPath& b);

  void run(BackupPath& b, function<void(const string& path)> fn = nullptr);

  void stop();

  void sleep_on_empty(bool b) { _sleep_on_empty = b; }

 private:
  bool process_nl(const string& path, const NodeList& nl);

  std::string _base_path;
  Index& _index;
  Storage& _store;
  Channel<string> _chan;

  bool _sleep_on_empty = true;

  bool _running = true;

  bool _stopped;
  condition_variable stopped_cv;
  std::mutex _stopped_m;
};
}

#endif /* BACKITUP_H_ */
