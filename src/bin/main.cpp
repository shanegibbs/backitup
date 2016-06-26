/*
 * main.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <sys/stat.h>
#include <vector>

#include <Backitup.h>
#include <LocalStorage.h>
#include <TextNodeRepo.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace std;
using namespace backitup;

namespace po = boost::program_options;

int main(int argc, char** argv) {
  string path;
  string storage_path;
  string index_path;

  // clang-format off
  po::options_description desc("Usage: backitup [OPTIONS] PATH");
  desc.add_options()
  ("help", "Produce this help message")
  ("index", po::value<string>(&index_path)
    ->default_value(string("backitup.db"))
    ->value_name(string("FILE")),
    "File index database path.")
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

  LocalStorage store(storage_path);
  TextNodeRepo index;

  vector<string> excludes;
  excludes.push_back(storage_path);
  excludes.push_back(index_path);
  BackupPath fs(path, excludes);

  Backitup backitup(index, store);
  backitup.init(fs);
  backitup.run(fs);

  sleep(10000);

  return 0;
}
