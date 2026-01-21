# doctest 統合ガイド（ヘッダオンリー）

doctest はヘッダオンリーの C++ テストフレームワークです。サブモジュールとして配置し、テスト用のコンソールアプリ（別プロジェクト）から `#include <doctest/doctest.h>` で利用できます。

## 導入（サブモジュール）
```
# プロジェクトルートで
git submodule add https://github.com/doctest/doctest.git third_party/doctest
```

## 最小テストランナー例
以下のファイルを `tests/ConfigTests.cpp` として作成します（例）。

```cpp
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

// 例: rapidyaml があれば簡単なパースを試す（ヘッダ群）
#if __has_include(<ryml.hpp>)
  #include <ryml.hpp>
TEST_CASE("ryml parses simple map") {
  auto tree = ryml::parse("{a: 1, b: 2}");
  CHECK(tree.rootref().is_map());
  int a=0, b=0; tree["a"] >> a; tree["b"] >> b;
  CHECK(a == 1);
  CHECK(b == 2);
}
#endif
```

## Visual Studio プロジェクト（推奨構成）
- 新規コンソールアプリ（例: `ConfigTests.vcxproj`）を作成
  - 言語標準: `/std:c++latest`
  - 文字セット: マルチバイト
  - サブシステム: Console
  - 追加インクルードディレクトリ: `third_party/doctest; third_party/rapidyaml_src/src; third_party/rapidyaml_src/ext/c4core/src`
- ソリューションに追加し、Debug/Release x64 のみビルド対象にしておくと管理が楽です。

## 実行
- ビルド後、生成された `ConfigTests.exe` を実行するとテストが走ります。
- 失敗時は非ゼロ終了コードとなるため、将来的にCIやタスク連携が容易です。

## どれを採用すべき？
- 本リポジトリでは導入コスト最小の doctest を第一選択肢とします。
- 大規模化やGoogleTest依存のサンプル流用が必要になったら、後からGoogleTestへ移行/併用可能です。
