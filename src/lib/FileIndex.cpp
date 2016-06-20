/*
 * FileIndex.cpp
 *
 *  Created on: Jul 20, 2015
 *      Author: sgibbs
 */

#include <stdio.h>
#include <string.h>
#include DB_CXX_HEADER

#include "FileIndex.h"
#include "Node.h"
#include "Database.pb.h"

namespace backitup {

FileIndex::FileIndex(const string &n) :
    name(n), db(shared_ptr<Db>(new Db(NULL, 0))) {

  u_int32_t oFlags = DB_CREATE;

  try {
    // Open the database
    db->open(NULL,                // Transaction pointer
        "index.db",          // Database file name
        NULL,                // Optional logical database name
        DB_BTREE,            // Database access method
        oFlags,              // Open flags
        0);                  // File mode (using defaults)
    // DbException is not subclassed from std::exception, so
    // need to catch both of these.
  } catch (DbException &e) {
    // Error handling code goes here
  } catch (std::exception &e) {
    // Error handling code goes here
  }

}

FileIndex::~FileIndex() {
  try {
    // Close the database
    db->close(0);
    // DbException is not subclassed from std::exception, so
    // need to catch both of these.
  } catch (DbException &e) {
    // Error handling code goes here
  } catch (std::exception &e) {
    // Error handling code goes here
  }
}

void FileIndex::getNodeRecord(const ::google::protobuf::Message &k) {

  // cout << endl << "Reading Key:" << endl << k.DebugString() << endl;

  string keyBuffer;
  k.SerializeToString(&keyBuffer);

  Dbt key(&keyBuffer, keyBuffer.length());

  int size = 1024;
  char buffer[size];

  Dbt data;
  data.set_data(buffer);
  data.set_ulen(size);
  data.set_flags(DB_DBT_USERMEM);

  db->get(NULL, &key, &data, 0);

  NodeRecord r;
  r.ParseFromArray(buffer, size);

  // cout << endl << "Read:" << endl << r.DebugString() << endl;

}

void save(Db *db, const ::google::protobuf::Message &k, const ::google::protobuf::Message &v) {

  // cout << endl << "Saving Key:" << endl << k.DebugString() << endl;
  // cout << endl << "Saving Value:" << endl << v.DebugString() << endl;

  string keyBuffer;
  k.SerializeToString(&keyBuffer);

  string valueBuffer;
  v.SerializeToString(&valueBuffer);

  Dbt key(&keyBuffer, keyBuffer.length());
  Dbt data(&valueBuffer, valueBuffer.length());

  int ret = db->put(NULL, &key, &data, DB_NOOVERWRITE);

  if (ret == DB_KEYEXIST) {
    printf("Put failed because key already exists");
  } else {
    // puts("DB-OK");
  }

}

void FileIndex::saveNodeRecord(const NodeRecord &r) {
}

shared_ptr<Node> FileIndex::getNode(unsigned int parentId, const string &name) {

  DatabaseSimpleKey k;
  k.set_type(NODE);
  k.set_id(1);

  NodeRecord v;
  v.set_parentid(parentId);
  v.set_name(name);

  save(db.get(), k, v);

  getNodeRecord(k);

  return shared_ptr<Node>();
}

}
