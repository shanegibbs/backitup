#include <sstream>
#include <stack>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <openssl/sha.h>

#include "HashFuncs.h"

using namespace std;

namespace backitup {

Hash::Hash() {
  if (!SHA256_Init(&_context)) cout << "ERROR" << endl;
}

void Hash::update(char* buf, int count) {
  if (!SHA256_Update(&_context, buf, count)) cout << "ERROR" << endl;
}

string Hash::get() {
  unsigned char md[SHA256_DIGEST_LENGTH];
  if (!SHA256_Final(md, &_context)) cout << "ERROR" << endl;

  stringstream ss;
  ss << hex << setfill('0');

  int i;
  for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    ss << std::setw(2) << static_cast<unsigned>(md[i]);
  }

  return ss.str();
}

const string sha256(const string& filename) {
  int bufLen = 512;
  char buf[bufLen];

  unsigned char md[SHA256_DIGEST_LENGTH];

  SHA256_CTX context;
  if (!SHA256_Init(&context)) cout << "ERROR" << endl;

  cout << "Hashing " << filename << endl;
  ifstream file(filename, ios::in | ios::binary);
  if (!file.is_open()) {
    cout << "ERROR failed to open " << filename << endl;
  }

  while (file) {
    file.read(buf, bufLen);

    if (!SHA256_Update(&context, buf, file.gcount())) cout << "ERROR" << endl;

    if (file.eof()) break;
  }

  if (!SHA256_Final(md, &context)) cout << "ERROR" << endl;

  stringstream ss;
  ss << hex << setfill('0');

  int i;
  for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    ss << std::setw(2) << static_cast<unsigned>(md[i]);
  }

  return ss.str();
}
}
