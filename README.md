# disable_google_drive_new_contextmenu_injector

GoogleDriveアプリが追加した新規作成コンテキストメニューを表示させないようにするソフトです。
![メインイメージ](https://user-images.githubusercontent.com/29115538/139759774-4686e7f6-dc21-40eb-94b4-1bcf2dcf87b6.PNG)

# 使い方

※パソコンを再起動(厳密にはGoogleDriveアプリの再起動)すると新規作成コンテキストメニューが復活してしまうため、
  タスクスケジューラ等を使ってログイン時に自動実行されるように設定してください。

## 簡単パターン
1. Releasesから「disable_google_drive_new_contextmenu 1.0.0.zip」をダウンロードしてください。
2. Zipを解凍してください。
3. GoogleDriveアプリが起動している状態で「injector.exe」を起動してください。
4. 新規作成コンテキストメニューから「Google Docs」等が消えていれば成功です。

## 手動でビルドパターン
1. Visual Studioにて本リポジトリをビルドし、「injector.exe」を作成してください。
2. 同じくVisual Studioにて https://github.com/forest-soft/disable_google_drive_new_contextmenu_dll をビルドし、「disable_google_drive_new_contextmenu.dll」を作成してください。
3. 「injector.exe」と「disable_google_drive_new_contextmenu.dll」を同じフォルダにコピーしてください。
　　folder
  　　∟injector.exe
     ∟disable_google_drive_new_contextmenu.dll
4. GoogleDriveアプリが起動している状態で「injector.exe」を起動してください。
5. 新規作成コンテキストメニューから「Google Docs」等が消えていれば成功です。

# タスクスケジューラへの登録方法
Todo 後で書く
