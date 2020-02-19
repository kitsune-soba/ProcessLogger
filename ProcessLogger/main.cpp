/*
	特定のプロセスの開始から終了までのデバッグ出力のログを取るプログラム。

	マルチバイト文字列とワイド文字列が混在すると面倒なので、マルチバイト文字列で統一する。
	文字コードもデフォルトで。
*/

#define _CRTDBG_MAP_ALLOC

#include <array>
#include <Windows.h>
#include "DebugStringReader.hpp"
#include "LogStream.hpp"
#include "Process.hpp"

// コマンドライン引数で指定するオプション
struct Options
{
	std::string target; // 起動するプロセスのパス（と引数）
	bool nofilter = false; // プロセスIDによるフィルタリングを無効化
	std::string log = "process.log"; // 出力するログファイルのパス
	std::ios_base::openmode logMode = std::ios::out; // ログファイルを開く際のモード
};

// オプションを指定する文字列から値の部分文字列を取り出す
// 例えば arg:"foo=hogehoge"、option:"foo="であれば、"hogehoge"を返す
bool getValueString(const std::string& arg, const std::string& name, std::string& value)
{
	if (arg.find(name) != 0)
	{
		return false;
	}
	else
	{
		value = arg.substr(name.size());
		return true;
	}
}

// コマンドライン引数からオプションを読み取る
bool args2options(const int argc, char* argv[], Options& options, std::string& errorMessage)
{
	std::string valueString;

	// 必須のオプションを確認
	if (argc < 2)
	{
		// 第二引数（起動するプロセス）が指定されていない
		errorMessage = "The target process is not specified.";
		return false;
	}
	options.target = argv[1];

	// 任意のオプションを判定（順不同）
	bool nofilter = false, log = false, append = false; // オプションの重複チェックのフラグ
	const std::string repeated = "The same option is specified repeatedly: ";
	const std::string empty = "The option is specified but the value is empty: ";
	for (int i = 2; i < argc; ++i)
	{
		if (std::strcmp(argv[i], "nofilter") == 0)
		{
			if (nofilter == true)
			{
				errorMessage = repeated + "nofilter";
				return false;
			}
			options.nofilter = true;
			nofilter = true;
		}
		else if (getValueString(argv[i], "log=", valueString))
		{
			if (log == true)
			{
				errorMessage = repeated + "log=";
				return false;
			}
			else if (valueString.empty())
			{
				errorMessage = empty + "log=";
				return false;
			}
			options.log = valueString;
			log = true;
		}
		else if (std::strcmp(argv[i], "append") == 0)
		{
			if (append == true)
			{
				errorMessage = repeated + "append";
				return false;
			}
			options.logMode = std::ios::app;
			append = true;
		}
		else
		{
			errorMessage = std::string("Invalid option : ") + argv[i];
			return false;
		}
	}

	return true;
}

// 現在の日時の文字列を得る
std::string datetime(void)
{
	tm time;
	__time64_t localtime;
	_time64(&localtime);
	std::array<char, 64> buffer;
	errno_t errorno = _localtime64_s(&time, &localtime);
	if (errorno == 0)
	{
		errorno = asctime_s(&buffer.at(0), buffer.size(), &time);
	}
	if (errorno == 0)
	{
		return std::string(&buffer.at(0));
	}
	else
	{
		return std::string("Failed to get process start time.\n");
	}
}

// アプリケーションエントリ
int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// 受け取った引数からオプションを解釈
	Options options;
	std::string optionErrorMessage;
	bool optionError = !args2options(argc, argv, options, optionErrorMessage);

	// オプションの指定が不正であれば、使い方を表示して終了する
	const std::string applicationName = "ProcessLogger 1.0.0";
	if (optionError)
	{
		std::cout << 
			applicationName << "\n"
			"Information and latest version: https://github.com/kitsune-soba/ProcessLogger\n"
			"\nError: " << optionErrorMessage << "\n"
			"\n"
			"Usage\tDrag and drop the target application to ProcessLogger.exe or use following command.\n"
			"\n"
			"\tProcessLogger.exe [Target] [Options]\n"
			"\n"
			"\t[Target]\tThe target process path (required) and arguments for the process. (optional)\n"
			"\t[Options]\tSpecify following options. (optional, multiple)\n"
			"\t\t\tnofilter  \tDisable PID filter.\n"
			"\t\t\t          \t(default: debug strings are filtered with PID of the target process)\n"
			"\t\t\tlog=[path]\tLog file path. (default: process.log)\n"
			"\t\t\tappend    \tOpens the log file with append mode. (default: write mode)\n"
			"\n"
			"\tCommand example : ProcessLogger.exe \"foobar.exe arg1 arg2\" nofilter log=foobar.log append" << std::endl;

		std::cout << "\nPress enter key to exit..." << std::endl;
		const int maybeenter = std::getchar();
		return 0;
	}

	// ログ書き込みの準備
	LogStream::setInitialParameters(options.log.c_str(), options.logMode);

	// プログラム名や起動時の情報を表示
	lout << applicationName << std::endl;
	lout << "Information and latest version: https://github.com/kitsune-soba/ProcessLogger" << std::endl;
	lout << '\n' << datetime() << std::flush;
	lout << "Arguments: " << std::endl;
	for (int i = 0; i < argc; ++i)
	{
		lout << "    " << i << ": " << argv[i] << std::endl;;
	}

	// ログ収集の対象プロセスを起動
	lout << '\n';
	lout_ts << "Starting \"" << options.target << "\"...";
	Process process(options.target.c_str());
	lout << " Success. (PID: " << process.getPid() << ')' << std::endl;

	// 対象プロセスが終わるまでデバッグログを記録
	DebugStringReader debugStringReader;
	std::string debugString;
	DWORD pid = 0;
	DWORD exitCode = STILL_ACTIVE;
	const std::string line("--------------------------------------------------------------------------------");
	lout_ts << line << std::endl;
	while (exitCode == STILL_ACTIVE)
	{
		// 対象プロセスが出力したデバッグメッセージを記録
		while ((pid = debugStringReader.read(debugString)) != 0)
		{
			if (options.nofilter)
			{
				if (debugString.back() == '\n') { debugString.pop_back(); } // 末尾に改行コードが付いている場合、下の std::endl と合わさってレイアウト的によろしくないので省略する（DebugView.exe のような改行規則）
				lout_ts << pid << '\t' << debugString << std::endl;
			}
			else if (pid == process.getPid())
			{
				if (debugString.back() == '\n') { debugString.pop_back(); } // 末尾に改行コードが付いている場合、下の std::endl と合わさってレイアウト的によろしくないので省略する（DebugView.exe のような改行規則）
				lout_ts << debugString << std::endl;
			}
		}

		exitCode = process.getExitCode();
	}
	lout_ts << line << std::endl;
	lout_ts << "The process finished with exit code: " << exitCode << std::endl;

	return 0;
}
