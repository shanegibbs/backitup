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

int main(int argc, char** argv) {
  backitup::loglevel = backitup::WARN;

  string op;
  string path;
  string storage_path;
  string index_path;

  // clang-format off
  po::options_description desc("Usage: backitup [OPTIONS] OP");
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
  pd.add("op", 1);
  pd.add("path", 1);

  po::options_description hidden("Hidden options");
  hidden.add_options()("op", po::value<string>(&op), "op");
  hidden.add_options()("path", po::value<string>(&path), "op");

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

  if (!boost::filesystem::exists(storage_path)) {
    if (mkdir(storage_path.c_str(), 0755) == -1) {
      cerr << "Failed to mkdir for storage path:" << storage_path << endl;
      return 1;
    }
  }

  storage_path = boost::filesystem::canonical(storage_path).native();

  backitup::LocalStorage store(storage_path);
  backitup::TextNodeRepo index;

  vector<string> excludes;
  backitup::BackupPath fs("/", excludes);  // TODO we don't need BackupPath here

  backitup::Backitup bu(index, store);

  if (op == "ls") {
    auto l = bu.list_path(path);
    for (string s : l) {
      cout << s << endl;
    }
  }

  return 0;
}
