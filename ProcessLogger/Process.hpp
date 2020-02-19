#ifndef PROCESS_LOGGER_PROCESS_HPP_
#define PROCESS_LOGGER_PROCESS_HPP_

#include <Windows.h>

// �v���Z�X���Ǘ�����
class Process
{
public:
	// �v���Z�X�N�����s�̗�O
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
