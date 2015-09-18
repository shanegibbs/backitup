#ifndef DATA_HELPER_FN_H_
#define DATA_HELPER_FN_H_

#include <tuple>
#include DB_CXX_HEADER

namespace backitup {

template <class T>
inline T parse(const Dbt *p) {
  string s((char *)p->get_data(), p->get_size());
  T val;
  if (!val.ParseFromString(s)) {
    cerr << "Failed to parse Dbt. Size was " << p->get_size() << endl;
    abort();
  }
  return val;
}

template <class T>
inline tuple<void *, size_t> copy(const T &t) {
  string str;
  t.SerializeToString(&str);

  size_t sz = sizeof(char) * str.size();

  void *buf = malloc(sz);
  copy(str.begin(), str.end(), (char *)buf);

  return tuple<void *, size_t>(buf, sz);
}

template <class T>
inline void copyToDbt(const T &t, Dbt *dbt) {
  auto mem = copy(t);
  dbt->set_data(get<0>(mem));
  dbt->set_size(get<1>(mem));
  dbt->set_flags(DB_DBT_APPMALLOC);
}

}
#endif /* DATA_HELPER_FN_H_ */
