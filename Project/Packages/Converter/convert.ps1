param(
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]] $InputPaths
)

# スクリプト自身のあるフォルダを取得
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# 出力先は「スクリプトのある場所\ConvertedDDS」
$outputDir = Join-Path $scriptDir "ConvertedDDS"

if (!(Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir | Out-Null
}

# 対応拡張子リスト
$extensions = @(".png", ".jpg", ".jpeg")

# ファイルとフォルダが混ざってもOKにする
$files = @()

foreach ($p in $InputPaths) {
    if (Test-Path $p) {
        if ((Get-Item $p).PSIsContainer) {
            # フォルダなら再帰検索（全ファイル拾ってから拡張子でフィルタ）
            $files += Get-ChildItem -Recurse -Path $p -File |
                      Where-Object { $extensions -contains $_.Extension.ToLower() }
        } else {
            # ファイルなら拡張子チェック
            $ext = [System.IO.Path]::GetExtension($p).ToLower()
            if ($extensions -contains $ext) {
                $files += Get-Item $p
            }
        }
    }
}

# スクリプト自身のあるフォルダを取得
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# TextureConverter.exe のフルパス
$converterExe = Join-Path $scriptDir "TextureConverter.exe"

foreach ($f in $files) {
    Start-Process -FilePath $converterExe `
        -ArgumentList @($f.FullName, "-ml", "3") `
        -Wait `
        -NoNewWindow

    $generated = [System.IO.Path]::ChangeExtension($f.FullName, ".dds")

    if (Test-Path $generated) {
        $destName = $f.BaseName + ".dds"
        $destPath = Join-Path $outputDir $destName
        Move-Item $generated $destPath -Force
    }
}

pause
