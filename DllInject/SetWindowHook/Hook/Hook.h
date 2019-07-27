#pragma once
#include <windows.h>

extern "C" __declspec(dllexport)   BOOL Hook();

extern "C" __declspec(dllexport)   BOOL UnHook();
