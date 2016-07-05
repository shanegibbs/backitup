/*
 * main.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

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

static backitup::Log LOG = backitup::Log("main");

unsigned long parse_max_file_size_bytes(string max_file_size_str) {
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

int main(int argc, char** argv) {
  backitup::loglevel = backitup::INFO;

  string path;
  string storage_path;
  string index_path;
  string backup_interval_str;
  string max_file_size_str;

  // clang-format off
  po::options_description desc("Usage: backitup [OPTIONS] PATH");
  desc.add_options()
  ("help", "Produce this help message")
  ("index", po::value<string>(&index_path)
    ->default_value(string("backitup.db"))
    ->value_name(string("FILE")),
    "File index database path.")
  ("interval", po::value<string>(&backup_interval_str)
   ->default_value(string("15m"))
   ->value_name(string("TIME")),
   "Time between backups.")
  ("max-file-size", po::value<string>(&max_file_size_str)
   ->default_value(string("10KB"))
   ->value_name(string("BYTES")),
   "Maximum file size to backup. 0 for no limit.")
  ("storage", po::value<string>(&storage_path)
    ->default_value(string("storage"))
    ->value_name(string("PATH"))
    ->required(),
    "Path to file storage for backup destination.")
  ;
  // clang-format on

  po::positional_options_description pd;
  pd.add("path", 1);

  po::options_description hidden("Hidden options");
  hidden.add_options()("path", po::value<string>(&path), "path");

  po::options_description cmdline_options;
  cmdline_options.add(desc).add(hidden);

  po::variables_map vm;
  try {
    po::store(po::command_line_parser(argc, argv)
                  .options(cmdline_options)
                  .positional(pd)
                  .run(),
              vm);
  } catch (exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    std::cerr << desc << std::endl;
    return 1;
  }

  if (vm.count("help")) {
    cout << desc << "\n";
    return 0;
  }

  try {
    po::notify(vm);
  } catch (exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    std::cerr << desc << std::endl;
    return 1;
  }

  try {
    path = boost::filesystem::canonical(path).native();
  } catch (exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
    return 1;
  }

  if (!boost::filesystem::exists(storage_path)) {
    if (mkdir(storage_path.c_str(), 0755) == -1) {
      cerr << "Failed to mkdir for storage path:" << storage_path << endl;
      return 1;
    }
  }

  storage_path = boost::filesystem::canonical(storage_path).native();

  if (vm.count("path")) {
    cout << "Backup Path: " << path << endl;
  } else {
    cout << "ERROR: Path not set.\n";
    cout << desc << "\n";
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
  backitup::TextNodeRepo index;

  vector<string> excludes;
  excludes.push_back(storage_path);
  excludes.push_back(index_path);
  backitup::BackupPath fs(path, excludes);

  backitup::Backitup bu(index, store);
  bu.interval(interval);
  bu.max_file_size_bytes(max_file_size_bytes);
  bu.init(fs);
  bu.run(fs);

  sleep(10000);

  return 0;
}
