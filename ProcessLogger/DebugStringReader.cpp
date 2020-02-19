/*
	OutputDebugString は、デバッグ文字列を DBWIN_BUFFER という4KBのプロセス間共有オブジェクトに書き込む。
	概ね以下の様な処理によって、OutputDebugString が出力したデバッグ文字列を読み取れる。

	- OutputDebugString の動作：
		1. DBWIN_BUFFER_READY イベントを待機
		2. DBWIN_BUFFER にデバッグ文字列を書き込む
		3. DBWIN_DATA_READY イベントをセットする

	- デバッグ文字列を読み取る処理（このファイルのコードで実装）：
		1. DBWIN_DATA_READY イベントを待機
		2. DBWIN_BUFFER からデバッグ文字列を読み取る
		3. DBWIN_BUFFER_READY イベントをセットする

	なお、DBWIN_BUFFER に書き込まれたデバッグ文字列は読み取られた後に消えるため、
	デバッグ文字列を読み取るプログラム（DebugViewなど）をシステム内で同時に走らせるべきではない。
	（いずれかのプログラムで読み取られたデバッグ文字列は、他のプログラムで読み取れない。）

	参考：
	- Understanding Win32 "OutputDebugString" - Unixwiz.net <<http://www.unixwiz.net/techtips/outputdebugstring.html>>
	- Mechanism of OutputDebugString - CodeProject <<https://www.codeproject.com/Articles/23776/Mechanism-of-OutputDebugString>>
*/

#include "common_error.hpp"
#include "DebugStringReader.hpp"

DebugStringReader::DebugStringReader()
{
	try
	{
		initialize();
	}
	catch (...)
	{
		release();
		throw;
	}
}

DebugStringReader::~DebugStringReader()
{
	release();
}

// デバッグ文字列を取得する
// 戻り値：デバッグ文字列を出力したプロセスのPID。デバッグ文字列を取得しなかった場合は0。
DWORD DebugStringReader::read(std::string &debugString) const
{
	// DBWIN_DATA_READY イベントを待機
	const DWORD state = WaitForSingleObject(dbwinDataReady, 100);

	switch (state)
	{
	case WAIT_OBJECT_0:
		// DBWIN_BUFFER からデバッグ文字列を読み取り、DBWIN_BUFFER_READY イベントをセット
		debugString = dbwinBufferMap->data;
		if (SetEvent(dbwinBufferReady) == 0)
		{
			lout << LOG_PREFIX << getLastErrorString() << std::endl;
		}
		return dbwinBufferMap->pid;
	case WAIT_TIMEOUT:
		return 0;
	case WAIT_FAILED:
		LOG_AND_THROW(ReadException, getLastErrorString().c_str());
	}

	LOG_AND_THROW(std::logic_error, "Unknown error."); // ここには来ないはず
}

// 初期化
void DebugStringReader::initialize(void)
{
	// DBWIN_BUFFER_READY
	// デバッグ文字列を書き込んでよい状態になった事を知らせるイベント
	// デバッグ文字列の読み取りを完了したらこのイベントをセットすること
	const char* dbwinBufferReadyName = "DBWIN_BUFFER_READY";
	dbwinBufferReady = OpenEventA(EVENT_ALL_ACCESS, FALSE, dbwinBufferReadyName);
	if (dbwinBufferReady == NULL)
	{
		dbwinBufferReady = CreateEventA(NULL, FALSE, TRUE, dbwinBufferReadyName);
	}
	if (dbwinBufferReady == NULL)
	{
		LOG_AND_THROW(InitializeException, getLastErrorString().c_str());
	}

	// DBWIN_DATA_READY
	// デバッグ文字列の書き込みが完了した事を通知するイベント
	// このイベントがセットされるのを待ってデバッグ文字列を読み取ること
	const char* dbwinDataReadyName = "DBWIN_DATA_READY";
	dbwinDataReady = OpenEventA(SYNCHRONIZE, FALSE, dbwinDataReadyName);
	if (dbwinDataReady == NULL)
	{
		dbwinDataReady = CreateEventA(NULL, FALSE, FALSE, dbwinDataReadyName);
	}
	if (dbwinDataReady == NULL)
	{
		LOG_AND_THROW(InitializeException, getLastErrorString().c_str());
	}

	// DBWIN_BUFFER
	// デバッグ文字列が書き込まれるファイルマッピングオブジェクト
	const char* dbwinBufferName = "DBWIN_BUFFER";
	dbwinBuffer = OpenFileMappingA(FILE_MAP_READ, FALSE, dbwinBufferName);
	if (dbwinBuffer == NULL)
	{
		dbwinBuffer = CreateFileMappingA
		(
			INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE,
			0,
			4096,
			dbwinBufferName
		);
	}
	if (dbwinBuffer == NULL)
	{
		LOG_AND_THROW(InitializeException, getLastErrorString().c_str());
	}

	// DBWIN_BUFFER をこのプロセスのアドレス空間にマッピング
	dbwinBufferMap = (DBWinBufferMap*)MapViewOfFile
	(
		dbwinBuffer,
		FILE_MAP_READ,
		0,
		0,
		sizeof(DBWinBufferMap)
	);
	if (dbwinBufferMap == NULL)
	{
		LOG_AND_THROW(InitializeException, getLastErrorString().c_str());
	}
}

// リソースを開放する
void DebugStringReader::release(void)
{
	if (dbwinBufferMap != NULL)
	{
		UnmapViewOfFile(dbwinBufferMap);
		dbwinBufferMap = NULL;
	}

	if (dbwinBuffer != NULL)
	{
		CloseHandle(dbwinBuffer);
		dbwinBuffer = NULL;
	}

	if (dbwinDataReady != NULL)
	{
		CloseHandle(dbwinDataReady);
		dbwinDataReady = NULL;
	}

	if (dbwinBufferReady != NULL)
	{
		CloseHandle(dbwinBufferReady);
		dbwinBufferReady = NULL;
	}
}
