#ifndef HASH_FUNCS_H_
#define HASH_FUNCS_H_

#include <string>
#include <openssl/sha.h>

namespace backitup {

const std::string sha256(const std::string& filename);

class Hash {
 public:
  Hash();
  void update(char* buf, int count);
  std::string get();

 private:
  SHA256_CTX _context;
};
}

#endif
