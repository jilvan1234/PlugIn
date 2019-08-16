#pragma once

int MmGetSectionMemoryAnWriteFile(SIZE_T &dwErrorCode, const HANDLE &hProcess, std::wstring &PathName, std::wstring &DirPath, CProcessOpt &PsOpt, const DWORD &dwPid, CFileManger &FsOpt, bool &retflag);
