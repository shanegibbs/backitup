/*
 * FileIndex.cpp
 *
 *  Created on: Jul 20, 2015
 *      Author: sgibbs
 */

#include <stdio.h>

#include DB_CXX_HEADER

#include "FileIndex.h"

namespace backitup {

FileIndex::FileIndex(const string &n) :
    name(n), db(unique_ptr<Db>(new Db(NULL, 0))) {

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

void FileIndex::getOrCreate() {

}

}
