#ifndef PROCESS_LOGGER_COMMON_ERROR_HPP_
#define PROCESS_LOGGER_COMMON_ERROR_HPP_

#include "LogStream.hpp"

// ���b�Z�[�W�����O�o�͂���O�𓊂���
#define LOG_AND_THROW(ExceptionType, message)\
{\
	lout_ts << LOG_PREFIX << "An exception is thrown : " << typeid(ExceptionType).name() << " (" << message << ')' << std::endl;\
	throw new ExceptionType(message);\
}

// GetLastError()�œ�����G���[���𕶎���Ŏ擾����
std::string getLastErrorString();

#endif // PROCESS_LOGGER_COMMON_ERROR_HPP_
