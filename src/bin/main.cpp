/*
 * main.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <fstream>
#include <regex>
#include <sys/stat.h>
#include <vector>

#include <Backitup.h>
#include <LocalStorage.h>
#include <TextNodeRepo.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <Log.h>

using namespace std;
// using namespace backitup;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

static backitup::Log LOG = backitup::Log("main");

unsigned long static parse_max_file_size_bytes(string max_file_size_str) {
  set<string> filesize_suffixes;
  filesize_suffixes.insert("KB");
  filesize_suffixes.insert("MB");
  filesize_suffixes.insert("GB");
  filesize_suffixes.insert("B");

  regex e("(\\d+)([KMG]?B)");

  if (!regex_match(max_file_size_str, e)) {
    throw string("Does not match regex: \\d+[KMG]?B");
  }

  smatch sm;
  regex_match(max_file_size_str, sm, e);

  string val = sm[1];
  string suffix = sm[2];

  if (suffix == "B") {
    return stoi(val);
  } else if (suffix == "KB") {
    return stoi(val) * 1024;
  } else if (suffix == "MB") {
    return stoi(val) * 1024 * 1024;
  } else if (suffix == "GB") {
    return stoi(val) * 1024 * 1024 * 1024;
  }

  throw string("Unable to determin suffix");
}

static string find_config_file() {
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

int main(int argc, char** argv) {
  backitup::loglevel = backitup::INFO;

  string op;
  string arg0;

  string path;
  string working_path;
  string backup_interval_str;
  string max_file_size_str;

  // clang-format off
  po::options_description desc("Usage: backitup [OPTIONS] PATH");
  desc.add_options()
  ("help", "Produce this help message")
  ;

  po::options_description config("Configuration");
  config.add_options()

  ("path", po::value<string>(&path)
   ->value_name(string("PATH"))
   ->required(),
   "Path to backup.")

  ("working", po::value<string>(&working_path)
   ->value_name(string("PATH"))
   ->required(),
   "Working directory for backitup.")

  ("interval", po::value<string>(&backup_interval_str)
   ->default_value(string("15m"))
   ->value_name(string("TIME")),
   "Time between backups.")

  ("max-file-size", po::value<string>(&max_file_size_str)
   ->default_value(string("10MB"))
   ->value_name(string("BYTES")),
   "Maximum file size to backup. 0 for no limit.")

  ;
  // clang-format on

  po::positional_options_description pd;
  pd.add("op", 1);
  pd.add("arg0", 1);

  po::options_description hidden("Hidden options");
  hidden.add_options()("op", po::value<string>(&op)->required(), "op");
  hidden.add_options()("arg0", po::value<string>(&op), "arg0");

  po::options_description cmdline_options;
  cmdline_options.add(desc).add(config).add(hidden);

  po::variables_map vm;

  string config_file_name = find_config_file();

  try {
    ifstream config_file(config_file_name, ifstream::in);
    po::store(po::parse_config_file(config_file, config), vm);
  } catch (exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    std::cerr << cmdline_options << std::endl;
    return 1;
  }

  try {
    po::store(po::command_line_parser(argc, argv)
                  .options(cmdline_options)
                  .positional(pd)
                  .run(),
              vm);
  } catch (exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    std::cerr << cmdline_options << std::endl;
    return 1;
  }

  if (vm.count("help")) {
    cout << cmdline_options << "\n";
    return 0;
  }

  try {
    po::notify(vm);
  } catch (exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    std::cerr << cmdline_options << std::endl;
    return 1;
  }

  if (working_path.front() != '/') {
    working_path =
        fs::path(config_file_name).parent_path().string() + "/" + working_path;
  }

  if (path.front() != '/') {
    path = fs::path(config_file_name).parent_path().string() + "/" + path;
  }

  try {
    path = boost::filesystem::canonical(path).native();
  } catch (exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    return 1;
  }

  info << "Working path " << working_path;

  string storage_path = working_path + "/blob-storage";
  string index_path = working_path + "/index.db";

  storage_path = boost::filesystem::canonical(storage_path).native();
  fs::create_directories(storage_path);

  if (vm.count("path")) {
    info << "Backup Path: " << path;
  } else {
    cout << "ERROR: Path not set.\n";
    cout << cmdline_options << "\n";
    return 1;
  }

  unsigned long max_file_size_bytes = 0;
  try {
    max_file_size_bytes = parse_max_file_size_bytes(max_file_size_str);
    info << "Max size file for backup is " << max_file_size_bytes << " bytes";
  } catch (string& e) {
    fatal << "Failed to parse max-file-size: " << e;
    return 1;
  }

  set<char> interval_types;
  interval_types.insert('s');
  interval_types.insert('m');
  interval_types.insert('h');

  char interval_kind = backup_interval_str.back();
  if (interval_types.find(interval_kind) == interval_types.end()) {
    fatal << "Period suffix for --interval: " << interval_kind;
    return 1;
  }

  string interval_count_str =
      backup_interval_str.substr(0, backup_interval_str.size() - 1);
  int interval_secs = stoi(interval_count_str);

  switch (interval_kind) {
    case 'm':
      interval_secs = interval_secs * 60;
      break;
    case 'h':
      interval_secs *= 60 * 60;
      break;
  }

  pair<string, int> interval =
      pair<string, int>(backup_interval_str, interval_secs);

  backitup::LocalStorage store(storage_path);
  backitup::TextNodeRepo index(index_path);

  vector<string> excludes;
  excludes.push_back(storage_path);
  excludes.push_back(index_path);
  backitup::BackupPath fs(path, excludes);

  backitup::Backitup bu(index, store);

  if (op == "service") {
    bu.interval(interval);
    bu.max_file_size_bytes(max_file_size_bytes);
    bu.init(fs);
    bu.run(fs).join();
    warn << "ending";

  } else if (op == "ls") {
    string current_dir = fs::current_path().string();
    if (arg0.empty() && current_dir.size() > path.size()) {
      arg0 = current_dir.substr(path.size() + 1);
    }

    auto l = bu.list_path(arg0);
    for (string s : l) {
      cout << s << endl;
    }

  } else {
    error << "Unknown opperation: " << op;
  }

  return 0;
}
