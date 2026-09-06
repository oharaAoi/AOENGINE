# サウンド定義表

Editor の **Setting → Sound Table** で、コードから呼ぶ名前と音声ファイルを登録します。

1. **Add** で行を追加し、重複しない名前を指定します。
2. Assets ウィンドウの `.mp3`・`.wave`・`.wav` のアイコンを、追加した行の **File** 欄（**Drop .mp3 / .wav / .wave here**）へドラッグ＆ドロップします。**Browse...** で選択することもできます。
3. 音量（0〜1）、ループ、分類（SE / BGM / UI）、同時再生上限（1〜128）を設定します。
4. **Preview** で保存前の設定を試聴します。試聴は同時に1音だけです。
5. **Save** で `Assets/Game/Audio/SoundTable.json` に保存します。

**Copy** は設定を複製し、一意の名前を付けます。**Delete** は編集表から行を削除し、Save で確定します。
**Reload** は保存済みの表を読み直します。未保存の編集がある場合は破棄の確認を表示します。
検索は名前とファイルに対して行います。検索で非表示になっている行も保存・検証の対象です。
名前の重複、空欄、空白を含む名前、存在しないファイル、不正な値は保存できません。

ファイルパスは `Assets` フォルダがあるプロジェクトディレクトリからの相対パスです。
起動時は作業ディレクトリと実行ファイルの親ディレクトリから `Assets` または `Project/Assets` を探索します。
配布時にも `Assets/Game/Audio/SoundTable.json` と、表が参照する音声ファイルを同じ相対配置で含めてください。

## コードから使う

```cpp
#include "Engine/Core/Engine.h"

// 同梱の定義。既存の fanfare.wav を再生する。
Engine::GetSoundManager()->Play("Fanfare");

// Editorで登録したループ音を再生し、この再生のハンドルを保持する。
AOENGINE::SoundHandle bgm = Engine::GetSoundManager()->Play("StageBGM");
Engine::GetSoundManager()->Pause(bgm);
Engine::GetSoundManager()->Resume(bgm);
Engine::GetSoundManager()->SetVolume(bgm, 0.5f); // 定義音量に掛ける個別倍率
Engine::GetSoundManager()->Stop(bgm);

Engine::GetSoundManager()->SetCategoryVolume(AOENGINE::SoundCategory::SE, 0.7f);
AOENGINE::Audio::SetMasterVolume(0.8f);
```

`Play` の戻り値を保持しなくても再生は継続します。ループOFFの音は終了時に自動回収します。
ループ音は Stop またはスコープ停止まで継続します。シーンをまたいで流すBGMも扱えるよう、
通常のシーン切替では自動停止しません。必要な場面でハンドルを停止するか、
`StopScope(SoundScope::Game)` でゲーム音を一括停止してください。

同名の音が上限に達した場合は新しい再生を追加しません。全体の同時再生上限は試聴を含め128音です。
名前が見つからない、重複している、ファイルを読み込めない場合はログを出し、無効ハンドルを返します。
`if (handle)` はハンドルが発行されたかを表します。再生中かは `IsPlaying(handle)`、
一時停止も含めた存続確認は `IsAlive(handle)` を使います。
古いハンドルへの Stop / Pause / Resume / SetVolume は無視されます。

音量は **定義音量 × 個別倍率 × 分類音量 × Master音量** です。
分類・Masterの変更は再生中にも反映します。EditorのMaster・分類スライダーはセッション中の設定で、
定義表には保存しません。

## Editorと再読み込み

- Edit状態ではゲームの名前指定再生を無効にし、Previewは利用できます。
- Play中のPauseはゲーム音を一時停止します。Previewには影響しません。
- Play終了でゲーム音を停止します。試聴音はStop preview、別のPreview、またはウィンドウを閉じたときに停止します。
- Saveした定義は次の再生から適用します。再生中の音は開始時の設定を維持します。
- 音声ファイルの変更は次の再生時に更新日時とサイズを検査し、必要なら再デコードします。
  既に鳴っている音は旧データを共有保持するので、途中で無効になりません。
- 名前を変更した場合、コード内の文字列も変更してください。コードの自動書き換えは行いません。

## 実装・制約

`SoundCatalog` がJSON、`SoundDatabase` がデコード済みデータ、`SoundManager` が再生を管理します。
SourceVoiceを破棄してからPCMの共有参照を解放します。
これは [XAudio2のバッファ寿命の要件](https://learn.microsoft.com/en-us/windows/win32/api/xaudio2/nf-xaudio2-ixaudio2sourcevoice-submitsourcebuffer) に合わせた所有関係です。
APIはエンジンのメインスレッドから呼び出してください。

現実装は全体をメモリへ読み込み、Media Foundationでステレオ16bit PCMに変換します。
長いBGMのストリーミング、フェード、3D音響、MP3のギャップレスループは対象外です。
初回再生時のデコードは同期処理です。音声キャッシュは終了時まで保持します。
旧 `AudioPlayer` APIは互換用に残していますが、新しいスコープ・分類制御は `SoundManager` の再生が対象です。

## テスト

`Tests/Audio/SoundTests.vcxproj` をDebug/x64でビルドし、プロジェクトディレクトリから
`Tests/Audio/Generated/SoundTests.exe` を実行します。音声出力デバイスが必要です。テスト中の音量は0です。
JSON検証と保存失敗時の保持、WAV / MP3デコード、キャッシュ共有・差し替え、同名ファイルの区別、
不正音声、名前再生、上限、世代付きハンドル、Pause/Resume、試聴スコープ、終了時回収を確認します。
