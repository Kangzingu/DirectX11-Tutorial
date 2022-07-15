#pragma once
#include <comdef.h>
#include "StringHelper.h"

#define ERROR_IF_FAILED(hr, msg) if (FAILED(hr)) throw Exception (hr, msg, __FILE__, __FUNCTION__, __LINE__)
#define ERROR_IF(isTrue, msg) if (isTrue) throw Exception (msg)
//COMException
class Exception
{
public:
	Exception(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
	{
		_com_error error(hr);
		whatmsg = L"Msg: " + StringHelper::StringToWide(std::string(msg)) + L"\n";
		whatmsg += error.ErrorMessage();
		whatmsg += L"\nFile: " + StringHelper::StringToWide(file);
		whatmsg += L"\nFunction: " + StringHelper::StringToWide(function);
		whatmsg += L"\nLine: " + StringHelper::StringToWide(std::to_string(line));
	}
	Exception(const std::string& msg)
	{
		whatmsg = L"Msg: " + StringHelper::StringToWide(std::string(msg));
	}
	const wchar_t* what() const
	{
		return whatmsg.c_str();
	}
private:
	std::wstring whatmsg;
};