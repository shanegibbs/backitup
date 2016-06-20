/*
 * File.h
 *
 *  Created on: Jul 20, 2015
 *      Author: sgibbs
 */

#ifndef FILE_H_
#define FILE_H_

#include <vector>

using namespace std;

namespace backitup {

class FileTreeNode;
class FileVersion;

class File {

private:
  const FileTreeNode filename;
  const FileVersion *currentVersion;
  vector<const FileVersion*> versions;

};

}

#endif /* FILE_H_ */
