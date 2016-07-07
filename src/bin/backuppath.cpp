/*
 * main.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <Backitup.h>
#include <LocalStorage.h>
#include <TextNodeRepo.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <Log.h>

using namespace std;

namespace po = boost::program_options;

static backitup::Log LOG = backitup::Log("main");

int main(int argc, char** argv) {
  backitup::loglevel = backitup::INFO;

  string path;

  // clang-format off
  po::options_description desc("Usage: BackupPath PATH");
  desc.add_options()
  ("help", "Produce this help message")
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
    fatal << e.what();
    std::cerr << std::endl << desc << std::endl;
    return 1;
  }

  if (vm.count("help")) {
    cout << desc << "\n";
    return 0;
  }

  try {
    po::notify(vm);
  } catch (exception& e) {
    fatal << e.what();
    std::cerr << std::endl << desc << std::endl;
    return 1;
  }

  try {
    path = boost::filesystem::canonical(path).native();
  } catch (exception& e) {
    fatal << e.what();
    return 1;
  }

  if (!vm.count("path")) {
    fatal << "Path not set";
    cout << std::endl << desc;
    return 1;
  }

  vector<string> excludes;
  backitup::BackupPath backuppath(path, excludes);

  backuppath.watch([](const string& changed) -> void { info << changed; });

  return 0;
}
