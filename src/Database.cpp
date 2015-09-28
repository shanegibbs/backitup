/*
 * Database.cpp
 *
 *  Created on: Jul 27, 2015
 *      Author: sgibbs
 */

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <string>
#include <iostream>
#include DB_CXX_HEADER

#include "Log.h"
#include "Database.h"

namespace backitup {

shared_ptr<Database> Database::open(const string &filename) {
  auto database = shared_ptr<Database>(new Database(filename));
  database->openDb();

  return database;
}

Database::Database(const string &f) : filename(f) {}

void Database::openDb() {
  u_int32_t oFlags = DB_CREATE;

  db = shared_ptr<Db>(new Db(NULL, 0));

  try {
    // Open the database
    db->open(NULL,              // Transaction pointer
             filename.c_str(),  // Database file name
             NULL,              // Optional logical database name
             DB_BTREE,          // Database access method
             oFlags,            // Open flags
             0);                // File mode (using defaults)
    // DbException is not subclassed from std::exception, so
    // need to catch both of these.
  } catch (DbException &e) {
    // Error handling code goes here
  } catch (std::exception &e) {
    // Error handling code goes here
  }
}

void Database::close() { db->close(0); }

const string Database::getRecord(const string &keyBuffer) {
  assert(db);

  Dbt key((void *)keyBuffer.data(), keyBuffer.length());

  int size = 1024;
  shared_ptr<void> buffer(malloc(size), free);

  Dbt data;
  data.set_data(buffer.get());
  data.set_ulen(size);
  data.set_flags(DB_DBT_USERMEM);

  int ret = db->get(NULL, &key, &data, 0);
  if (ret == DB_NOTFOUND) {
    throw NotFoundDatabaseException();
  } else if (ret != 0) {
    throw GeneralDatabaseException(ret);
  }

  string result((char *)data.get_data(), data.get_size());
  return result;
}

const StringPair Database::getPrimaryRecord(const string &skeyBuffer) {
  assert(db);

  Dbt skey((void *)skeyBuffer.data(), skeyBuffer.length());

  int size = 1024;

  shared_ptr<void> pKeyBuffer(malloc(size), free);
  Dbt pkey;
  pkey.set_data(pKeyBuffer.get());
  pkey.set_ulen(size);
  pkey.set_flags(DB_DBT_USERMEM);

  shared_ptr<void> dataBuffer(malloc(size), free);
  Dbt data;
  data.set_data(dataBuffer.get());
  data.set_ulen(size);
  data.set_flags(DB_DBT_USERMEM);

  int ret = db->pget(NULL, &skey, &pkey, &data, 0);
  if (ret == DB_NOTFOUND) {
    throw NotFoundDatabaseException();
  } else if (ret != 0) {
    throw GeneralDatabaseException(ret);
  }

  string pkeyStr((char *)pkey.get_data(), pkey.get_size());
  string dataStr((char *)data.get_data(), data.get_size());

  return StringPair(pkeyStr, dataStr);
}

void Database::putRecord(const string &key, const string &value) {
  Dbt dbKey((void *)key.data(), key.length());
  Dbt dbValue((void *)value.data(), value.length());

  int ret = db->put(NULL, &dbKey, &dbValue, 0 /* DB_NOOVERWRITE */);
  if (ret == DB_KEYEXIST) {
    throw ExistsDatabaseException();
  } else if (ret != 0) {
    throw GeneralDatabaseException(ret);
  }
}

void Database::compact() {
  db->compact(NULL, NULL, NULL, NULL, DB_FREE_SPACE, NULL);
}

GeneralDatabaseException::GeneralDatabaseException(int err) : err(err) {
  ostringstream os;
  os << "Failure " << db_strerror(err);
  errStr = os.str();
}

}
