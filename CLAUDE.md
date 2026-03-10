# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## ビルドコマンド

```bash
make          # コンパイラをビルド（./mcc が生成される）
make test     # ビルドして全テスト実行（test.sh）
make clean    # ビルド成果物を削除
```

## 概要

**mcc (mizucc)** はCのサブセットをx86-64アセンブリ（Intel構文）にコンパイルするCコンパイラ。コマンドライン引数でCプログラムを受け取り、アセンブリを標準出力に出力する。

使い方: `./mcc "1+2;"` → x86-64アセンブリが出力される。

## アーキテクチャ

3段階のコンパイラパイプライン。`mcc.h`で定義されたグローバル変数を介して各段階が連携する。

1. **tokenizer.c** — 字句解析。入力文字列を`Token`の連結リストに変換。キーワード（`return`, `if`, `else`, `while`, `for`）は専用の`TokenKind`で識別される。

2. **parser.c** — 再帰下降構文解析。トークン列を`Node`のAST（抽象構文木）に変換。文法はファイル先頭のコメントに定義されている。演算子の優先順位は呼び出し階層で表現: `program → stmt → expr → assign → equality → relational → add → mul → unary → primary`。

3. **generator.c** — ASTを走査してコード生成。**スタックマシン**モデルを採用 — 各式の結果をx86スタックにpushし、演算子がオペランドをpopして結果をpushする。制御フロー用のユニークラベルは`label_count`で管理。

4. **main.c** — エントリポイント。tokenize → program（構文解析） → gen を呼び出し、関数プロローグ/エピローグで出力を囲む。ローカル変数用に208バイトのスタック領域を確保。

### 主要なグローバル変数（`mcc.h`）

- `Token *token` — トークン列の現在位置（パーサが消費）
- `Node *code[100]` — トップレベルの文のASTノード配列
- `LVar *locals` — ローカル変数の連結リスト（各変数はRBPからのオフセットを持つ）

### 現在サポートしている言語機能

整数、四則演算（`+`,`-`,`*`,`/`）、比較演算子（`==`,`!=`,`<`,`<=`,`>`,`>=`）、ローカル変数（英字のみ）、代入、`return`、`if`/`else`、`while`、`for`、ブロック文`{}`（実装中）。

## テスト

`test.sh`の`assert`関数が: `./mcc`で入力をコンパイル → `cc`でアセンブル → 実行 → 終了コードを検証する。終了コードで結果を確認するため、テスト値は0〜255の範囲に限られる。

## 備考

- Linux x86-64をターゲットとし、`-static`リンクを使用。C11標準。
- CodeRabbitは`master`ブランチに対する日本語レビューに設定されている。
