#include <array>
#include "common_error.hpp"
#include "Process.hpp"

Process::Process(const char* process)
{
	std::array<char, 512> buffer;
	strcpy_s(&buffer.at(0), buffer.size(), process);
	
	// プロセスを起動する
	const BOOL result = CreateProcessA
	(
		NULL,
		&buffer.at(0),
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS,
		NULL,
		NULL,
		&startUpInfo,
		&processInfo
	);

	if (result == 0)
	{
		LOG_AND_THROW(ProcessCreateException, getLastErrorString().c_str());
	}
}

Process::~Process()
{
	if (processInfo.hThread != NULL) { CloseHandle(processInfo.hThread); }
	if (processInfo.hProcess != NULL) { CloseHandle(processInfo.hProcess); }
}

// プロセスIDを返す
DWORD Process::getPid(void) const
{
	return processInfo.dwProcessId;
}

// プロセスの終了コードを確認する（プロセスが生きている場合は STILL_ACTIVE を返す）
DWORD Process::getExitCode(void) const
{
	DWORD exitCode;
	if (GetExitCodeProcess(processInfo.hProcess, &exitCode) == 0)
	{
		lout_ts << LOG_PREFIX << "Failed to GetExitCodeProcess with exit code : " << exitCode << std::endl;
		LOG_AND_THROW(std::runtime_error, getLastErrorString());
	}
	return exitCode;
}
