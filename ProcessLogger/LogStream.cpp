#include <sstream>
#include "LogStream.hpp"

bool LogStream::initialized = false;
std::string LogStream::logPath;
std::ios_base::openmode LogStream::logMode;

// ストリームの初期化に必要なパラメータを設定する
void LogStream::setInitialParameters(const char* path, const std::ios_base::openmode mode)
{
	logPath = path;
	logMode = mode;
	initialized = true;
}

// Singletonなインスタンスを返す
LogStream& LogStream::instance()
{
	// 事前の setInitialParameters() の呼び出しを強制する
	if (!initialized)
	{
		const char* message = "The log stream is used before initialized.";
		std::cout << message << std::endl;
		throw std::logic_error(message);
	}

	static LogStream instance(logPath.c_str(), logMode);
	return instance;
}

LogStream::LogStream(const char* path, const std::ios_base::openmode mode)
	: ofs(path, mode)
	, beginTime(std::chrono::system_clock::now())
{
	if (!ofs)
	{
		std::ostringstream oss;
		oss << "Cannot open the log file : " << logPath;
		std::cout << oss.str() << std::endl;
		throw FileOpenException(oss.str().c_str());
	}
}

// マニピュレータを受け取るオーバーロード
// 参考：C++で2つの ostream に同時出力する - akihiko's tech note <<https://aki-yam.hatenablog.com/entry/20080630/1214801872>>
LogStream& LogStream::operator<<(std::ostream& (*pf)(std::ostream&))
{
	pf(std::cout);
	pf(ofs);
	return *this;
}

// 経過時間（秒）を返す
double LogStream::getTimestamp(void) const
{
	const std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - beginTime;
	return elapsed.count();
}
