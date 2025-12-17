# DigitShowBasic (OpenSource Edition)

![Github License](https://img.shields.io/github/license/mkt-kuno/DigitShowBasic)  [![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com) 

<img width="1019" height="739" alt="image" src="https://github.com/user-attachments/assets/2d86bec5-5eb6-4870-9612-685556bf65e4" />


## 簡単な説明
東京大学の地盤研で使用されている、三軸試験機制御ソフトウェアのオープンソース版です。  
ContecのAD/DAボードで動作させることを前提としています。  
このリポジトリのライセンスは **GPLv3** となっているため、注意してください。  

## 動作環境
- Windows 11  
x64のみ, ARM64不可、ARM64ECはなんとか可能、、、かもしれない
- Visual Studio 2022
Community版でOK, MFCライブラリ必須  
- CONTEC API-AIO(WDM) Ver.8.90  
適宜、CAIO.H, CAIO.LIBを置き換えて使用するDLLバージョン一致させれば最新版でも可。
- CPU: x64 Intel/AMD問わず  
[Passmark性能(マルチスレッド)](https://www.cpubenchmark.net/multithread/) 最低5000 推奨8000以上
- RAM: 最低4GB 推奨8GB以上  
他に動かすアプリケーション次第。MS Officeは重い。
- GPU: 依存なし、iGPU/dGPU/APU いずれも可
- 液晶: 最低HD 推奨FHD以上  
縦長画面だと表示が見切れると思います。
- 記憶媒体: 最低HDD 推奨SSD  
容量はビルドPCと動作PCが同じなら最低128GB 推奨256GB


## 注意点
- 本ソフトウェアの動作について、一切の保証を行いません。
- 本ソフトウェアの使用により発生したいかなる損害についても、一切の責任を負いません。
- 本ソフトウェアを使用する場合は、自己責任で行ってください。
- 本ソフトウェアの動作または初期設定についてのサポートは行いません。
- 本ソフトウェアの改変、再配布はGPLv3の条件に従って行ってください。

## ライセンスについて
一部の大学・企業・研究所では秘伝のタレ状態のDigitShowBasicをお持ちだと思います。  
当時(2010年頃)配布されたDigitShowBasicのソースコードは何もライセンスが決められていませんでした。  
そのため、お持ちの古いDigitShowBasicは高確率でライセンスフリーのハズです。  
ですが、このリポジトリはGPLv3です。このコードを安易に参考・参照・引用した場合GPLv3に感染するので、  
GPLv3とは何か知ったうえで、覚悟して使い始めてください。  
ざっくりといえば、改変部分がある場合、ソースコードを公開する必要があります。

## バグ報告やプルリクエストについて
デバッガ、コーダ、メンテナ、などなどが複数人、現れた場合のみ、管理・サポートを行おうと思います。  
「どう使うの？」「ボードが認識しない」「設定方法を教えてほしい」などの初歩的な質問は避けてください。無視します。  
「うちのコードとかなり違う」「そもそも動作しないち落ちる」などの場合は、  
AI協業でリファクタリングする前の[legacy版](https://github.com/mkt-kuno/DigitShowBasic/tree/legacy)で試してみて下さい。  
「初期設定や困った部分を文章化したので載せてほしい」「AIOボードの初期化を自動にしたコードをマージしてほしい」など、  
貢献する意思のある、オープンソースの理念に沿った要求は大歓迎します。  
「根幹設計から新しいの作りたい」というやる気とコーディング能力のある方は、  
[ぜひこちら](https://github.com/mkt-kuno/DSMng)、もしくは東大地盤研までご連絡を。

## リポジトリの運用方針について
- 基本的に新機能の追加は行いません。プルリクがあればコードレビューはします。
- AIによるバイブコーディングを禁止しませんが、推奨もしません。
- Pull Requestを送る場合は、簡単で良いので動作確認を行い、変更点を記載してください。
- バグなどのIssueを送る場合は、必ず考えつく限り詳細な、問題を再現するのに必要な情報を提供してください。
- main/developブランチが荒れない事が第一なので、差分が多くなければfeature/****などに独自のforkコードを置いてOKです。
- Git Worktreeに必ずしも従う必要はありません。
- commitは何となく変更点が分かればいいです。コメントの書き方も自由で良いです。日本語でもOKです。
- 無茶な要求が続くようであれば公開をやめ、コードを放棄します。libxml2のように。
