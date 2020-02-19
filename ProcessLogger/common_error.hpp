#ifndef PROCESS_LOGGER_COMMON_ERROR_HPP_
#define PROCESS_LOGGER_COMMON_ERROR_HPP_

#include "LogStream.hpp"

// メッセージをログ出力しつつ例外を投げる
#define LOG_AND_THROW(ExceptionType, message)\
{\
	lout_ts << LOG_PREFIX << "An exception is thrown : " << typeid(ExceptionType).name() << " (" << message << ')' << std::endl;\
	throw new ExceptionType(message);\
}

// GetLastError()で得られるエラー情報を文字列で取得する
std::string getLastErrorString();

#endif // PROCESS_LOGGER_COMMON_ERROR_HPP_
