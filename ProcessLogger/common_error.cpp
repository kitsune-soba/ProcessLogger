#include <array>
#include <cassert>
#include <Windows.h>
#include "common_error.hpp"

// GetLastError()‚Å“¾‚ç‚ê‚éƒGƒ‰[î•ñ‚ğ•¶š—ñ‚Åæ“¾‚·‚é
std::string getLastErrorString()
{
	const DWORD errorCode = GetLastError();
	assert(errorCode != NO_ERROR);
	std::array<char, 512> errorMessage;
	const DWORD result = FormatMessageA
	(
		FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		errorCode,
		0,
		&errorMessage.at(0),
		static_cast<DWORD>(errorMessage.size()),
		NULL
	);
		
	assert(result != 0);
	return std::string(&errorMessage.at(0));
}
