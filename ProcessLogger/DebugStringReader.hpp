#ifndef PROCESS_LOGGER_DEBUG_STRING_READER_HPP_
#define PROCESS_LOGGER_DEBUG_STRING_READER_HPP_

#include <stdexcept>
#include <string>
#include <Windows.h>

// OutputDebugString で出力されたデバッグ出力を読み取る
class DebugStringReader
{
public:
	// 初期化失敗の例外
	class InitializeException : public std::runtime_error
	{
	public:
		InitializeException(const char* message) : runtime_error(message) {}
	};

	// デバッグ文字列取得失敗の例外
	class ReadException : public std::runtime_error
	{
	public:
		ReadException(const char* message) : runtime_error(message) {};
	};

	DebugStringReader();
	~DebugStringReader();

	// 書き込まれたデバッグ文字列を読み取る
	DWORD read(std::string &debugString) const;

private:
	// DBWIN_BUFFER をマッピングするデータ領域
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
