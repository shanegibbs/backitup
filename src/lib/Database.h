/*
 * Database.h
 *
 *  Created on: Jul 27, 2015
 *      Author: sgibbs
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <memory>
#include <string>

using namespace std;

class Db;

namespace backitup {

typedef pair<string, string> StringPair;

class Database {
 public:
  static shared_ptr<Database> open(const string &filename);

  const StringPair getPrimaryRecord(const string &key);
  const string getRecord(const string &key);
  void putRecord(const string &key, const string &value);

  shared_ptr<Db> getDb() {
    return db;
  }

  void compact();
  void close();

 private:
  Database(const string &f);

  void openDb();

  const string filename;
  shared_ptr<Db> db;
};

class ExistsDatabaseException : public exception {
  virtual const char *what() const throw() { return "Already exists"; }
};

class NotFoundDatabaseException : public exception {
  virtual const char *what() const throw() { return "Key not found"; }
};

class GeneralDatabaseException : public exception {
 public:
  GeneralDatabaseException(int err);
  virtual const char *what() const throw() { return errStr.c_str(); }
  string errStr;
  const int err;
};
}
#endif /* DATABASE_H_ */
