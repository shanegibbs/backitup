#include "Options.h"

#include <iomanip>
#include <regex>
#include <set>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <Log.h>

using namespace std;

namespace fs = boost::filesystem;

namespace backitup {

static backitup::Log LOG = backitup::Log("Options");

string Options::find_config_file() {
  fs::path current = fs::current_path();

  while (!current.string().empty()) {
    string config_file = current.string() + "/.backitup";
    if (fs::exists(config_file)) {
      info << "Using config " << config_file;
      return config_file;
    }
    current = current.parent_path();
  }

  string system_config = "/etc/backitup/config";
  if (fs::exists(system_config)) {
    info << "Using config " << system_config;
    return system_config;
  }

  return ".backitup";
}

unsigned long Options::parse_max_file_size_bytes(string max_file_size_str) {
  set<string> filesize_suffixes;
  filesize_suffixes.insert("KB");
  filesize_suffixes.insert("MB");
  filesize_suffixes.insert("GB");
  filesize_suffixes.insert("B");

  regex e("(\\d+)([KMG]?B)");

  if (!regex_match(max_file_size_str, e)) {
    throw OptionsException("Max file size does not match regex: \\d+[KMG]?B");
  }

  smatch sm;
  regex_match(max_file_size_str, sm, e);

  string val = sm[1];
  string suffix = sm[2];

  if (suffix == "B") {
    return stoi(val);
  } else if (suffix == "KB") {
    return stoi(val) * 1024UL;
  } else if (suffix == "MB") {
    return stoi(val) * 1024UL * 1024UL;
  } else if (suffix == "GB") {
    return stoi(val) * 1024UL * 1024UL * 1024UL;
  }

  // should not be able happen
  throw OptionsException("Unable to determin max file size suffix");
}

/* Parse PATH@TIME, PATH, @TIME
 *
 * PATH - If not specified, takes current directory if inside backup path.
 *
 * TIME - Two formats:
 *           1year2months3weeks4days5hours6mins7secs
 *           2016-07-10-18:52:49
 */
pair<string, time_t> Options::parse_path_time_spec(string current_dir,
                                                   string backup_path,
                                                   string arg0) {
  string path_arg = arg0;
  time_t timestamp_arg = time(nullptr);

  if (arg0.find('@') != string::npos) {
    regex e("^([^@]*)@([^@]+)$");

    if (!regex_match(arg0, e)) {
      throw backitup::OptionsException(
          "Path spec does not match regex: ^([^@]*)@([^@]+)$");
    }

    smatch sm;
    regex_match(arg0, sm, e);
    path_arg = sm[1];
    string time_arg = sm[2];
    info << "time_arg=" << time_arg;

    regex timespec_regex("^(\\d+)([a-z]+)s?");
    smatch m;

    string time_arg_tmp = time_arg;

    while (regex_search(time_arg_tmp, m, timespec_regex)) {
      int count = stoi(m[1]);
      string period = m[2];
      if (period.back() == 's') {
        period = period.substr(0, period.size() - 1);
      }

      int magnitude;

      if (period == "sec") {
        magnitude = 1;
      } else if (period == "min") {
        magnitude = 60;
      } else if (period == "hour") {
        magnitude = 60 * 60;
      } else if (period == "day") {
        magnitude = 60 * 60 * 24;
      } else if (period == "week") {
        magnitude = 60 * 60 * 24 * 7;
      } else if (period == "year") {
        magnitude = 60 * 60 * 24 * 7 * 52;
      } else {
        throw backitup::OptionsException(string("Unknown time period: ") +
                                         m[2].str());
      }

      timestamp_arg -= (count * magnitude);

      time_arg_tmp = m.suffix().str();
    }

    if (!time_arg_tmp.empty()) {
      throw backitup::OptionsException(string("Failed to parse timespec: ") +
                                       time_arg);
    }
  }

  tm tm = *std::localtime(&timestamp_arg);
  info << "Timestamp set to " << put_time(&tm, "%c %Z");

  // string current_dir = fs::current_path().string();
  if (path_arg.empty() && current_dir.size() > backup_path.size()) {
    path_arg = current_dir.substr(backup_path.size() + 1);
  }

  info << "path_arg=" << path_arg;
  return pair<string, time_t>(path_arg, timestamp_arg);
}
}