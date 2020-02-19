#ifndef PROCESS_LOGGER_LOG_STREAM_HPP_
#define PROCESS_LOGGER_LOG_STREAM_HPP_

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

// 標準出力とログファイルへ同時に出力するストリーム（Singleton パターン）
// ログを取り始める前に setInitialParameters() を呼び出すこと。
class LogStream
{
public:
	// ファイルオープン失敗の例外
	class FileOpenException : public std::runtime_error
	{
	public:
		FileOpenException(const char* message) : std::runtime_error(message) {};
	};

	// Singleton関連
	static void setInitialParameters(const char* path, const std::ios_base::openmode mode);
	static LogStream& instance();

	// 演算子オーバーロード
	template<typename T>
	LogStream& operator<<(const T& rhs)
	{
		std::cout << rhs;
		ofs << rhs;
		return *this;
	}
	LogStream& operator<<(std::ostream& (*pf)(std::ostream&));

	double getTimestamp(void) const;

private:
	LogStream(const char* logPath, std::ios_base::openmode logMode);
	LogStream(const LogStream&);
	~LogStream() = default;
	LogStream& operator=(const LogStream&);

	// Singleton関連
	static bool initialized;
	static std::string logPath;
	static std::ios_base::openmode logMode;

	std::ofstream ofs;
	const std::chrono::system_clock::time_point beginTime;
};

// coutっぽくログ出力するためのマクロ（lout_ts は出力にタイムスタンプを加える）
#define lout LogStream::instance()
#define lout_ts LogStream::instance() << std::fixed << std::setprecision(3) << LogStream::instance().getTimestamp() << std::defaultfloat << " | "
#define LOG_PREFIX "[ProcessLogger]"

#endif // PROCESS_LOGGER_LOG_STREAM_HPP_
