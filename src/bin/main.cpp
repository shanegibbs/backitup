/*
 * main.cpp
 *
 *  Created on: Jul 19, 2015
 *      Author: sgibbs
 */

#include <Backitup.h>
#include <LocalStorage.h>
#include <TextNodeRepo.h>

using namespace std;
using namespace backitup;

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << "No path" << endl;
    return 1;
  }

  string path = argv[1];
  cout << "Backup Path: " << path << endl;

  auto store = LocalStorage::create("storage");
  TextNodeRepo repo;
  Backitup backitup(repo, *store);

  auto b = BackupPath::create(path);
  backitup.run(*b);

  return 0;
}
