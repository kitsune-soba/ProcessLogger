/*
	ProcessLogger の動作確認の為にデバッグ文字列を出力する。
	DebugView <<https://docs.microsoft.com/en-us/sysinternals/downloads/debugview>> による表示をリファレンスとして、
	ProcessLogger が同じ内容のデバッグ文字列を取得できればテスト成功とする。
	（デバッグ文字列の取り合いになるので、ProcessLogger と DebugView は同時に走らせないこと。）
*/

#include <iostream>
#include <sstream>
#include <Windows.h>

int main(int argc, char* argv[])
{
	// コマンドライン引数を出力するテスト
	std::ostringstream oss;
	oss << "[DebugOutputTest]Arguments: ";
	for (int i = 0; i < argc; ++i)
	{
		oss << argv[i];
		if (i < argc - 1) { oss << ' '; }
	}
	std::cout << oss.str() << std::endl;
	OutputDebugStringA(oss.str().c_str());

	// 一度に複数行を出力するテスト
	for (size_t i = 0; i < 5; ++i)
	{
		std::string string(i + 1, '*');
		string = "[DebugOutputTest]" + string;
		std::cout << string << std::endl;
		OutputDebugStringA(string.c_str());
	}

	// 時間をずらして何度か出力するテスト
	for (size_t i = 0; i < 5; ++i)
	{
		Sleep(100);
		std::string string(i + 1, '*');
		string = "[DebugOutputTest]" + string;
		std::cout << string << std::endl;
		OutputDebugStringA(string.c_str());
	}

	// 改行をテスト
	// （タイムスタンプを付けるため、改行コードが無くとも一度の出力毎に必ず改行する。その際に、二重で改行しないように文字列末尾の改行コードは無視される。）
	OutputDebugStringA("この直後に空行（一部空白文字を含む）が6行↓\n"); // この末尾の改行は無視される（一度だけ改行される）
	OutputDebugStringA(""); // 空行 * 0（空の文字列は OutputDebugStinrg() が出力しないっぽい。）
	OutputDebugStringA("\n"); // 空行 * 1（文字列末尾の改行コードは無視される。）
	OutputDebugStringA("\n "); // 空行 * 2（文字列末尾以外の改行コードは有効。）
	OutputDebugStringA("\n\n\n"); // 空行 * 3（最初の2個の改行コード + 出力終わりの改行で3行）

	// 長い文字列のテスト
	std::string longString(512, '*');
	longString.back() = '#'; // 最後まで表示されたことを確認するためのマーカ
	OutputDebugStringA(longString.c_str());

	// 日本語を出力するテスト
	const char* charString = "[DebugOutputTest]日本語も出力してみるテスト（OutputDebugStringA）";
	std::cout << charString << std::endl;
	OutputDebugStringA(charString);

	// ついでにワイド文字もテスト
	const wchar_t* wcharString = L"[DebugOutputTest]日本語も出力してみるテスト（OutputDebugStringW）";
	std::wcout << wcharString << std::endl;
	OutputDebugStringW(wcharString);

	return 0;
}
