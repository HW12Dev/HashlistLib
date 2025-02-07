#pragma once

#include <string>

#ifdef HASHLIB_INTERNAL
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __declspec(dllimport)
#endif

namespace dsl {
	API_EXPORT unsigned long long hash64(
		char* k,     /* the key */
		unsigned long long length, /* the length of the key */
		unsigned long long level  /* the previous hash, or an arbitrary value */
	);
	API_EXPORT unsigned long long hash64(const std::string& s);

	API_EXPORT class idstring {
	public:
		unsigned long long _id;
		API_EXPORT idstring() : _id(0) {}
		API_EXPORT idstring(unsigned long long _id) : _id(_id) {}
		API_EXPORT idstring(const dsl::idstring& other);
		API_EXPORT idstring(const std::string& source);
		API_EXPORT idstring(const char* source);

		API_EXPORT bool operator<(const dsl::idstring& b) const;
		API_EXPORT bool operator==(const dsl::idstring& b) const;
	};
}

API_EXPORT void __stdcall load_hashlist();


API_EXPORT std::string attempt_to_find_source_idstring_from_hashlist(unsigned long long str, bool file_printable = true);
API_EXPORT std::string attempt_to_find_source_idstring_from_hashlist(dsl::idstring str, bool file_printable = true);

API_EXPORT bool hashlist_contains(dsl::idstring str);