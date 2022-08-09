#include "StringHelper.h"

// wstring은 유니코드
std::wstring StringHelper::StringToWString(const std::string& str)
{
	std::wstring wString(str.begin(), str.end());
	return wString;
}
std::string StringHelper::GetDirectoryFromPath(const std::string& path)
{
	size_t offset1 = path.find_last_of('\\');
	size_t offset2 = path.find_last_of('/');
	if (offset1 == std::string::npos && offset2 == std::string::npos)
		return "";
	else if (offset1 == std::string::npos)
		return path.substr(0, offset2);
	else if (offset2 == std::string::npos)
		return path.substr(0, offset1);
	else
		return path.substr(0, std::max(offset1, offset2) - 1);
}
std::string StringHelper::GetFileExtension(const std::string& name)
{
	size_t offset = name.find_last_of('.');
	if (offset == std::string::npos)
		return {};

	return std::string(name.substr(offset + 1));
}