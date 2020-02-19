/*
	����̃v���Z�X�̊J�n����I���܂ł̃f�o�b�O�o�͂̃��O�����v���O�����B

	�}���`�o�C�g������ƃ��C�h�����񂪍��݂���Ɩʓ|�Ȃ̂ŁA�}���`�o�C�g������œ��ꂷ��B
	�����R�[�h���f�t�H���g�ŁB
*/

#define _CRTDBG_MAP_ALLOC

#include <array>
#include <Windows.h>
#include "DebugStringReader.hpp"
#include "LogStream.hpp"
#include "Process.hpp"

// �R�}���h���C�������Ŏw�肷��I�v�V����
struct Options
{
	std::string target; // �N������v���Z�X�̃p�X�i�ƈ����j
	bool nofilter = false; // �v���Z�XID�ɂ��t�B���^�����O�𖳌���
	std::string log = "process.log"; // �o�͂��郍�O�t�@�C���̃p�X
	std::ios_base::openmode logMode = std::ios::out; // ���O�t�@�C�����J���ۂ̃��[�h
};

// �I�v�V�������w�肷�镶���񂩂�l�̕�������������o��
// �Ⴆ�� arg:"foo=hogehoge"�Aoption:"foo="�ł���΁A"hogehoge"��Ԃ�
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

// �R�}���h���C����������I�v�V������ǂݎ��
bool args2options(const int argc, char* argv[], Options& options, std::string& errorMessage)
{
	std::string valueString;

	// �K�{�̃I�v�V�������m�F
	if (argc < 2)
	{
		// �������i�N������v���Z�X�j���w�肳��Ă��Ȃ�
		errorMessage = "The target process is not specified.";
		return false;
	}
	options.target = argv[1];

	// �C�ӂ̃I�v�V�����𔻒�i���s���j
	bool nofilter = false, log = false, append = false; // �I�v�V�����̏d���`�F�b�N�̃t���O
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

// ���݂̓����̕�����𓾂�
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

// �A�v���P�[�V�����G���g��
int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// �󂯎������������I�v�V����������
	Options options;
	std::string optionErrorMessage;
	bool optionError = !args2options(argc, argv, options, optionErrorMessage);

	// �I�v�V�����̎w�肪�s���ł���΁A�g������\�����ďI������
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

	// ���O�������݂̏���
	LogStream::setInitialParameters(options.log.c_str(), options.logMode);

	// �v���O��������N�����̏���\��
	lout << applicationName << std::endl;
	lout << "Information and latest version: https://github.com/kitsune-soba/ProcessLogger" << std::endl;
	lout << '\n' << datetime() << std::flush;
	lout << "Arguments: " << std::endl;
	for (int i = 0; i < argc; ++i)
	{
		lout << "    " << i << ": " << argv[i] << std::endl;;
	}

	// ���O���W�̑Ώۃv���Z�X���N��
	lout << '\n';
	lout_ts << "Starting \"" << options.target << "\"...";
	Process process(options.target.c_str());
	lout << " Success. (PID: " << process.getPid() << ')' << std::endl;

	// �Ώۃv���Z�X���I���܂Ńf�o�b�O���O���L�^
	DebugStringReader debugStringReader;
	std::string debugString;
	DWORD pid = 0;
	DWORD exitCode = STILL_ACTIVE;
	const std::string line("--------------------------------------------------------------------------------");
	lout_ts << line << std::endl;
	while (exitCode == STILL_ACTIVE)
	{
		// �Ώۃv���Z�X���o�͂����f�o�b�O���b�Z�[�W���L�^
		while ((pid = debugStringReader.read(debugString)) != 0)
		{
			if (options.nofilter)
			{
				if (debugString.back() == '\n') { debugString.pop_back(); } // �����ɉ��s�R�[�h���t���Ă���ꍇ�A���� std::endl �ƍ��킳���ă��C�A�E�g�I�ɂ�낵���Ȃ��̂ŏȗ�����iDebugView.exe �̂悤�ȉ��s�K���j
				lout_ts << pid << '\t' << debugString << std::endl;
			}
			else if (pid == process.getPid())
			{
				if (debugString.back() == '\n') { debugString.pop_back(); } // �����ɉ��s�R�[�h���t���Ă���ꍇ�A���� std::endl �ƍ��킳���ă��C�A�E�g�I�ɂ�낵���Ȃ��̂ŏȗ�����iDebugView.exe �̂悤�ȉ��s�K���j
				lout_ts << debugString << std::endl;
			}
		}

		exitCode = process.getExitCode();
	}
	lout_ts << line << std::endl;
	lout_ts << "The process finished with exit code: " << exitCode << std::endl;

	return 0;
}
