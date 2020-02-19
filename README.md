# ProcessLogger

## 概要

簡易な手順で、指定した対象プロセスのデバッグ出力<sup>[1](#note_DebugOutput) </sup>をファイルに記録するソフトウェア。

対象プロセスの実行ファイルを ProcessLogger.exe にドラッグアンドドロップすると対象プロセスが起動し、そのプロセスからのデバッグ出力が自動的にファイルに保存される。これをユーザから送って貰えば、ユーザ環境でのデバッグ出力を知ることができ、不具合発生時のサポートに役立つ。一般ユーザに専門的なツール<sup>[2](#note_DebugTool) </sup>の使い方を説明する必要も無い。
また、パグリックドメインなので自由に再配布でき、サポート側からユーザへ直接 ProcessLogger.exe を渡すことができる。

## 動作環境

Windows

## インストール

DebugLogger.exe を適当なところに置く。

## 使い方（基本）

1. 対象プロセスの実行ファイルを、ProcessLogger.exe にドラッグアンドドロップする（ProcessLogger と対象のプロセスが起動する）
1. 不具合発生時のデバッグ出力を得るために、対象プロセスを操作して不具合を再現する
1. 対象のプロセスが終了すると ProcessLogger も自動的に終了し、対象プロセスと同じディレクトリの process.log にデバッグ出力が記録されている

## 使い方（オプションを指定）

対象プロセス起動時にコマンドライン引数を渡したい場合や、対象以外のプロセスのデバッグ出力も必要な場合などは、以下のコマンドによって起動することでオプション機能を利用する。
ProcessLogger.exe と、コマンドを記述したバッチファイルをセットでユーザに渡すと楽でよいと思われる。

コマンド：```ProcessLogger.exe [Target] [Options]```

- [Target] 対象プロセスの実行ファイルのパス（必須）と、そのプロセスに渡す引数（任意、複数可）
- [Options] 以下のオプションを指定する（任意、複数可）
	- nofilter：フィルタを無効にして、全てのプロセスのデバッグ出力を記録する（標準ではデバッグ出力をプロセスIDでフィルタリングして、対象プロセスからの出力のみを記録する）
	- log=\[path]：ログファイルのパスを指定する（標準では process.log）
	- append：ログファイルを追記モードで開く（標準では上書きモード）

オプションを全て指定した例：```ProcessLogger.exe "foobar.exe arg1 arg2" nofilter log=foobar.log append"```

## 使用時の注意

デバッグ出力を取得する他のツール<sup>[2](#note_DebugTool) </sup>と同時に使用しないこと。
デバッグ出力はシステム内の共有リソースに書き込まれるが、読み取られたデバッグ出力は共有リソースから消えるため、ツール間でデバッグ出力の取り合いが発生し、情報を取りこぼしてしまうため。
同じ理由で、ProcessLogger の多重起動もしないこと。

---
<sup name="note_DebugOutput">1</sup> [OutputDebugStinrg](https://docs.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-outputdebugstringw) などによる DBWIN_BUFFER への出力を指す。
<sup name="note_DebugTool">2</sup> [DebugView](https://docs.microsoft.com/en-us/sysinternals/downloads/debugview) など。
