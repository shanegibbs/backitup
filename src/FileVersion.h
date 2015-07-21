/*
 * FileVersion.h
 *
 *  Created on: Jul 20, 2015
 *      Author: sgibbs
 */

#ifndef FILEVERSION_H_
#define FILEVERSION_H_

#include <string>

using namespace std;

class FileVersion {

private:
  unsigned int num;
  size_t size;
  unsigned long timestamp;
  string hashMd5;
  string hashSha1;

};

#endif /* FILEVERSION_H_ */
