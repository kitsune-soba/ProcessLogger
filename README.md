# ProcessLogger

## 概要

指定した対象プロセスのデバッグ出力<sup>[1](#note_DebugOutput) </sup>をファイルに記録するソフトウェア。

## 動作環境

Windows

## インストール

[ProcessLogger.exe をダウンロード](https://github.com/kitsune-soba/ProcessLogger/releases)して適当なところに置く。

## 使い方（基本）

1. 対象プロセスの実行ファイルを ProcessLogger.exe にドラッグアンドドロップする（ProcessLogger と対象プロセスが起動する）
1. 対象プロセスを使用する（この間、起動した ProcessLogger のウィンドウでデバッグ出力をリアルタイムに確認することもできる）
1. 対象のプロセスが終了すると ProcessLogger も自動的に終了し、対象プロセスの実行ファイルと同じディレクトリに process.log というファイル名でデバッグ出力が保存される

## 使い方（オプションを指定）

対象プロセスにコマンドライン引数を渡したい場合や、対象プロセス以外のデバッグ出力も記録したい場合は、以下のようにコマンドでオプション機能を利用する。

コマンド：```ProcessLogger.exe [Target] [Options]```

- [Target] 対象プロセスの実行ファイルのパス（必須）と、そのプロセスに渡す引数（任意、複数可）
- [Options] 以下のオプションを指定する（任意、複数可）
	- nofilter：フィルタを無効にして、全てのプロセスのデバッグ出力を記録する（このオプションが指定されていない場合は、プロセスIDのフィルタリングにより対象プロセスからのデバッグ出力のみを記録する。デフォルトはフィルタリング有効。）
	- log=\[path]：ログファイルのパスを指定する（デフォルトは process.log）
	- append：ログファイルを追記モードで開く（デフォルトは上書きモード）

オプションを全て指定した例：```ProcessLogger.exe "foobar.exe arg1 arg2" nofilter log=foobar.log append"```

## 使用時の注意

デバッグ出力を取得する他のツール<sup>[2](#note_DebugTool) </sup>と同時に使用しないこと。
デバッグ出力はシステム内の共有リソースに書き込まれるが、読み取られたデバッグ出力は共有リソースから消えるため、ツール間でデバッグ出力の取り合いが発生して情報を取りこぼしてしまうため。
同じ理由で、ProcessLogger の多重起動もしないこと。

---
<sup name="note_DebugOutput">1</sup> [OutputDebugStinrg](https://docs.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-outputdebugstringw) などによる DBWIN_BUFFER への出力を指す。  
<sup name="note_DebugTool">2</sup> [DebugView](https://docs.microsoft.com/en-us/sysinternals/downloads/debugview) など。
