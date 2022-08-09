#pragma once
#include <comdef.h>
#include "StringHelper.h"

#define ERROR_IF_FAILED(hResult, message) if (FAILED(hResult)) throw Exception (hResult, message, __FILE__, __FUNCTION__, __LINE__)
#define ERROR_IF(isTrue, message) if (isTrue) throw Exception (message)
//COMException
class Exception
{
private:
	std::wstring m_message;

public:
	Exception(HRESULT hResult, const std::string& message, const std::string& file, const std::string& function, int line)
	{
		_com_error error(hResult);
		m_message = L"Msg: " + StringHelper::StringToWString(std::string(message)) + L"\n";
		m_message += error.ErrorMessage();
		m_message += L"\nFile: " + StringHelper::StringToWString(file);
		m_message += L"\nFunction: " + StringHelper::StringToWString(function);
		m_message += L"\nLine: " + StringHelper::StringToWString(std::to_string(line));
	}
	Exception(const std::string& message)
	{
		m_message = L"Msg: " + StringHelper::StringToWString(std::string(message));
	}
	const wchar_t* what() const
	{
		return m_message.c_str();
	}
};