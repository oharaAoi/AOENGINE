#pragma once
#include <string>

/// <summary> 
/// ファイルを保存する際に使用するDialog
/// </summary> 
/// <returns>: 保存先のパス </returns>
std::string FileSaveDialogFunc();

/// <summary>
/// ファイルを開く際に使用するDialog
/// </summary>
/// <returns>: 開きたいファイルまでのパス</returns>
std::string FileOpenDialogFunc();