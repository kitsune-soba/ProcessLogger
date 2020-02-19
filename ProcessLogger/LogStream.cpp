#include <sstream>
#include "LogStream.hpp"

bool LogStream::initialized = false;
std::string LogStream::logPath;
std::ios_base::openmode LogStream::logMode;

// �X�g���[���̏������ɕK�v�ȃp�����[�^��ݒ肷��
void LogStream::setInitialParameters(const char* path, const std::ios_base::openmode mode)
{
	logPath = path;
	logMode = mode;
	initialized = true;
}

// Singleton�ȃC���X�^���X��Ԃ�
LogStream& LogStream::instance()
{
	// ���O�� setInitialParameters() �̌Ăяo������������
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

// �}�j�s�����[�^���󂯎��I�[�o�[���[�h
// �Q�l�FC++��2�� ostream �ɓ����o�͂��� - akihiko's tech note <<https://aki-yam.hatenablog.com/entry/20080630/1214801872>>
LogStream& LogStream::operator<<(std::ostream& (*pf)(std::ostream&))
{
	pf(std::cout);
	pf(ofs);
	return *this;
}

// �o�ߎ��ԁi�b�j��Ԃ�
double LogStream::getTimestamp(void) const
{
	const std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - beginTime;
	return elapsed.count();
}
