#ifndef PROCESS_LOGGER_LOG_STREAM_HPP_
#define PROCESS_LOGGER_LOG_STREAM_HPP_

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>

// �W���o�͂ƃ��O�t�@�C���֓����ɏo�͂���X�g���[���iSingleton �p�^�[���j
// ���O�����n�߂�O�� setInitialParameters() ���Ăяo�����ƁB
class LogStream
{
public:
	// �t�@�C���I�[�v�����s�̗�O
	class FileOpenException : public std::runtime_error
	{
	public:
		FileOpenException(const char* message) : std::runtime_error(message) {};
	};

	// Singleton�֘A
	static void setInitialParameters(const char* path, const std::ios_base::openmode mode);
	static LogStream& instance();

	// ���Z�q�I�[�o�[���[�h
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

	// Singleton�֘A
	static bool initialized;
	static std::string logPath;
	static std::ios_base::openmode logMode;

	std::ofstream ofs;
	const std::chrono::system_clock::time_point beginTime;
};

// cout���ۂ����O�o�͂��邽�߂̃}�N���ilout_ts �͏o�͂Ƀ^�C���X�^���v��������j
#define lout LogStream::instance()
#define lout_ts LogStream::instance() << std::fixed << std::setprecision(3) << LogStream::instance().getTimestamp() << std::defaultfloat << " | "
#define LOG_PREFIX "[ProcessLogger]"

#endif // PROCESS_LOGGER_LOG_STREAM_HPP_
