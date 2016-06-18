#include "Record.h"

namespace backitup {

RecordList NewRecordList() { return RecordList(new std::vector<Record>()); }
}