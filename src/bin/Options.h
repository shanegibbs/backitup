#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <string>

namespace backitup {

class OptionsException : public std::runtime_error {
 public:
  OptionsException() : runtime_error("Options error") {}
  OptionsException(std::string msg)
      : runtime_error(
            std::string(std::string("Options error: ").append(msg)).c_str()) {}
};

class Options {
 public:
  Options() {}

  std::string find_config_file();
  unsigned long parse_max_file_size_bytes(std::string max_file_size_str);
  std::pair<std::string, time_t> parse_path_time_spec(std::string current_dir,
                                                      std::string backup_path,
                                                      std::string arg0);

 private:
};
}

#endif /* OPTIONS_H_ */
