/*
	OutputDebugString �́A�f�o�b�O������� DBWIN_BUFFER �Ƃ���4KB�̃v���Z�X�ԋ��L�I�u�W�F�N�g�ɏ������ށB
	�T�ˈȉ��̗l�ȏ����ɂ���āAOutputDebugString ���o�͂����f�o�b�O�������ǂݎ���B

	- OutputDebugString �̓���F
		1. DBWIN_BUFFER_READY �C�x���g��ҋ@
		2. DBWIN_BUFFER �Ƀf�o�b�O���������������
		3. DBWIN_DATA_READY �C�x���g���Z�b�g����

	- �f�o�b�O�������ǂݎ�鏈���i���̃t�@�C���̃R�[�h�Ŏ����j�F
		1. DBWIN_DATA_READY �C�x���g��ҋ@
		2. DBWIN_BUFFER ����f�o�b�O�������ǂݎ��
		3. DBWIN_BUFFER_READY �C�x���g���Z�b�g����

	�Ȃ��ADBWIN_BUFFER �ɏ������܂ꂽ�f�o�b�O������͓ǂݎ��ꂽ��ɏ����邽�߁A
	�f�o�b�O�������ǂݎ��v���O�����iDebugView�Ȃǁj���V�X�e�����œ����ɑ��点��ׂ��ł͂Ȃ��B
	�i�����ꂩ�̃v���O�����œǂݎ��ꂽ�f�o�b�O������́A���̃v���O�����œǂݎ��Ȃ��B�j

	�Q�l�F
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

// �f�o�b�O��������擾����
// �߂�l�F�f�o�b�O��������o�͂����v���Z�X��PID�B�f�o�b�O��������擾���Ȃ������ꍇ��0�B
DWORD DebugStringReader::read(std::string &debugString) const
{
	// DBWIN_DATA_READY �C�x���g��ҋ@
	const DWORD state = WaitForSingleObject(dbwinDataReady, 100);

	switch (state)
	{
	case WAIT_OBJECT_0:
		// DBWIN_BUFFER ����f�o�b�O�������ǂݎ��ADBWIN_BUFFER_READY �C�x���g���Z�b�g
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

	LOG_AND_THROW(std::logic_error, "Unknown error."); // �����ɂ͗��Ȃ��͂�
}

// ������
void DebugStringReader::initialize(void)
{
	// DBWIN_BUFFER_READY
	// �f�o�b�O���������������ł悢��ԂɂȂ�������m�点��C�x���g
	// �f�o�b�O������̓ǂݎ������������炱�̃C�x���g���Z�b�g���邱��
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
	// �f�o�b�O������̏������݂�������������ʒm����C�x���g
	// ���̃C�x���g���Z�b�g�����̂�҂��ăf�o�b�O�������ǂݎ�邱��
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
	// �f�o�b�O�����񂪏������܂��t�@�C���}�b�s���O�I�u�W�F�N�g
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

	// DBWIN_BUFFER �����̃v���Z�X�̃A�h���X��ԂɃ}�b�s���O
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

// ���\�[�X���J������
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
