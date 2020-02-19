#ifndef PROCESS_LOGGER_PROCESS_HPP_
#define PROCESS_LOGGER_PROCESS_HPP_

#include <Windows.h>

// プロセスを管理する
class Process
{
public:
	// プロセス起動失敗の例外
	class ProcessCreateException : public std::runtime_error
	{
	public:
		ProcessCreateException(const char* message) : std::runtime_error(message) {};
	};

	Process(const char* process);
	~Process();

	DWORD getPid(void) const;
	DWORD getExitCode(void) const;

private:
	Process(const Process&);
	Process& operator=(const Process&);

	STARTUPINFOA startUpInfo = { 0 };
	PROCESS_INFORMATION processInfo = { 0 };
};

#endif // PROCESS_LOGGER_PROCESS_HPP_
