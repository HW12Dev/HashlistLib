#define HASHLIB_INTERNAL
#include <HashlistLib.h>
#include <fstream>
#include <sstream>
#include <stdint.h>

#include <filesystem>

#include <map>

std::map<dsl::idstring, std::string> hashlist;

dsl::idstring::idstring(const dsl::idstring& other) {
	this->_id = other._id;
}
dsl::idstring::idstring(const std::string& source) {
	this->_id = dsl::hash64(source);
}
dsl::idstring::idstring(const char* source) {
  this->_id = dsl::hash64((char*)source, strlen(source), 0);
}
bool dsl::idstring::operator<(const dsl::idstring& b) const {
  return this->_id < b._id;
}
bool dsl::idstring::operator==(const dsl::idstring& b) const {
  return this->_id == b._id;
}

/*
--------------------------------------------------------------------
lookup8.c, by Bob Jenkins, January 4 1997, Public Domain.
hash(), hash2(), hash3, and mix() are externally useful functions.
Routines to test the hash are included if SELF_TEST is defined.
You can use this free for any purpose.  It has no warranty.

2009: This is obsolete.  I recently timed lookup3.c as being faster
at producing 64-bit results.
--------------------------------------------------------------------
*/

#define hashsize(n) ((unsigned long long)1 << (n))
#define hashmask(n) (hashsize(n) - 1)

#define mix64(a, b, c) \
  {                    \
    a -= b;            \
    a -= c;            \
    a ^= (c >> 43);    \
    b -= c;            \
    b -= a;            \
    b ^= (a << 9);     \
    c -= a;            \
    c -= b;            \
    c ^= (b >> 8);     \
    a -= b;            \
    a -= c;            \
    a ^= (c >> 38);    \
    b -= c;            \
    b -= a;            \
    b ^= (a << 23);    \
    c -= a;            \
    c -= b;            \
    c ^= (b >> 5);     \
    a -= b;            \
    a -= c;            \
    a ^= (c >> 35);    \
    b -= c;            \
    b -= a;            \
    b ^= (a << 49);    \
    c -= a;            \
    c -= b;            \
    c ^= (b >> 11);    \
    a -= b;            \
    a -= c;            \
    a ^= (c >> 12);    \
    b -= c;            \
    b -= a;            \
    b ^= (a << 18);    \
    c -= a;            \
    c -= b;            \
    c ^= (b >> 22);    \
  }

unsigned long long dsl::hash64(
  char* k,     /* the key */
  unsigned long long length, /* the length of the key */
  unsigned long long level  /* the previous hash, or an arbitrary value */
) {
  unsigned long long a, b, c, len;

  /* Set up the internal state */
  len = length;
  a = b = level;            /* the previous hash value */
  c = 0x9e3779b97f4a7c13LL; /* the golden ratio; an arbitrary value */

  /*---------------------------------------- handle most of the key */
  while (len >= 24) {
    a += (k[0] + ((unsigned long long)k[1] << 8) + ((unsigned long long)k[2] << 16) + ((unsigned long long)k[3] << 24) + ((unsigned long long)k[4] << 32) + ((unsigned long long)k[5] << 40) + ((unsigned long long)k[6] << 48) + ((unsigned long long)k[7] << 56));
    b += (k[8] + ((unsigned long long)k[9] << 8) + ((unsigned long long)k[10] << 16) + ((unsigned long long)k[11] << 24) + ((unsigned long long)k[12] << 32) + ((unsigned long long)k[13] << 40) + ((unsigned long long)k[14] << 48) + ((unsigned long long)k[15] << 56));
    c += (k[16] + ((unsigned long long)k[17] << 8) + ((unsigned long long)k[18] << 16) + ((unsigned long long)k[19] << 24) + ((unsigned long long)k[20] << 32) + ((unsigned long long)k[21] << 40) + ((unsigned long long)k[22] << 48) + ((unsigned long long)k[23] << 56));
    mix64(a, b, c);
    k += 24;
    len -= 24;
  }

  /*------------------------------------- handle the last 23 bytes */
  c += length;
  switch (len) /* all the case statements fall through */
  {
  case 23:
    c += ((unsigned long long)k[22] << 56);
  case 22:
    c += ((unsigned long long)k[21] << 48);
  case 21:
    c += ((unsigned long long)k[20] << 40);
  case 20:
    c += ((unsigned long long)k[19] << 32);
  case 19:
    c += ((unsigned long long)k[18] << 24);
  case 18:
    c += ((unsigned long long)k[17] << 16);
  case 17:
    c += ((unsigned long long)k[16] << 8);
    /* the first byte of c is reserved for the length */
  case 16:
    b += ((unsigned long long)k[15] << 56);
  case 15:
    b += ((unsigned long long)k[14] << 48);
  case 14:
    b += ((unsigned long long)k[13] << 40);
  case 13:
    b += ((unsigned long long)k[12] << 32);
  case 12:
    b += ((unsigned long long)k[11] << 24);
  case 11:
    b += ((unsigned long long)k[10] << 16);
  case 10:
    b += ((unsigned long long)k[9] << 8);
  case 9:
    b += ((unsigned long long)k[8]);
  case 8:
    a += ((unsigned long long)k[7] << 56);
  case 7:
    a += ((unsigned long long)k[6] << 48);
  case 6:
    a += ((unsigned long long)k[5] << 40);
  case 5:
    a += ((unsigned long long)k[4] << 32);
  case 4:
    a += ((unsigned long long)k[3] << 24);
  case 3:
    a += ((unsigned long long)k[2] << 16);
  case 2:
    a += ((unsigned long long)k[1] << 8);
  case 1:
    a += ((unsigned long long)k[0]);
    /* case 0: nothing left to add */
  }
  mix64(a, b, c);
  /*-------------------------------------------- report the result */
  return c;
}

unsigned long long dsl::hash64(const std::string& s) {
  char* temp = new char[s.length() + 1];
  strcpy(temp, s.c_str());
  auto hash = dsl::hash64(temp, s.length(), 0);
  delete[] temp;
  return hash;
}

void load_hashlist() {
  if (hashlist.size() > 0)
    return;
  std::ifstream hash;
  if (std::filesystem::exists("./hashlist.txt")) {
    hash = std::ifstream("hashlist.txt");
  } else if(std::filesystem::exists("./hashlist"))
  {
    hash = std::ifstream("hashlist");
  } else
  {
    return;
  }

  std::string line;

  while (std::getline(hash, line)) {
    hashlist[dsl::idstring(line)] = line;
  }
}
std::string attempt_to_find_source_idstring_from_hashlist(unsigned long long str_, bool file_printable) {
  dsl::idstring str = dsl::idstring(str_);
  if (hashlist.contains(str)) return hashlist[str];
  std::stringstream sstream;
  if (!file_printable)
    sstream << "(hex)";
  sstream << std::hex << str._id;
  return sstream.str();
}
std::string attempt_to_find_source_idstring_from_hashlist(dsl::idstring str, bool file_printable) {
  if (hashlist.contains(str)) return hashlist[str];
  std::stringstream sstream;
  if (!file_printable)
    sstream << "(hex)";
  sstream << std::hex << str._id;
  return sstream.str();
}

bool hashlist_contains(dsl::idstring str) {
  return hashlist.contains(str);
}