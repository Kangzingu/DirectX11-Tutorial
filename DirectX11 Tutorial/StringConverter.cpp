#include "StringConverter.h"

// wstring은 유니코드
std::wstring StringConverter::StringToWide(std::string str) {
	std::wstring wide_string(str.begin(), str.end());
	return wide_string;
}
