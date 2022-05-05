#ifndef PROCESS_LOGGER_DEBUG_STRING_READER_HPP_
#define PROCESS_LOGGER_DEBUG_STRING_READER_HPP_

#include <stdexcept>
#include <string>
#include <Windows.h>

// OutputDebugString �ŏo�͂��ꂽ�f�o�b�O�o�͂�ǂݎ��
class DebugStringReader
{
public:
	// ���������s�̗�O
	class InitializeException : public std::runtime_error
	{
	public:
		InitializeException(const char* message) : runtime_error(message) {}
	};

	// �f�o�b�O������擾���s�̗�O
	class ReadException : public std::runtime_error
	{
	public:
		ReadException(const char* message) : runtime_error(message) {};
	};

	DebugStringReader();
	~DebugStringReader();

	// �������܂ꂽ�f�o�b�O�������ǂݎ��
	DWORD read(std::string &debugString) const;

private:
	// DBWIN_BUFFER ���}�b�s���O����f�[�^�̈�
	struct DBWinBufferMap
	{
		DWORD pid;
		char data[4096 - sizeof(DWORD)];
	};

	DebugStringReader(const DebugStringReader&);
	DebugStringReader& operator=(const DebugStringReader&);

	void initialize(void);
	void release(void);

	HANDLE dbwinBufferReady, dbwinDataReady, dbwinBuffer;
	DBWinBufferMap* dbwinBufferMap;
};

#endif // PROCESS_LOGGER_DEBUG_STRING_READER_HPP_
