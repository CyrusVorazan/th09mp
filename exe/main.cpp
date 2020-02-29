#include <Windows.h>
#include <iostream>
#include <string>
#include <Shlwapi.h>
#include "inject.h"

const std::string dll_name("inject.dll");
const std::string exe_path("Path_to_th09.exe_here");

bool InjectDll(HANDLE process, const std::string& dll_path) {
   size_t arg_size = ::strlen(dll_path.c_str()) + 1;
   void* arg_addr = ::VirtualAllocEx(process, NULL, arg_size, MEM_COMMIT, PAGE_READWRITE);
   if (!arg_addr) {
      std::cerr << "VirtualAllocEx failed" << std::endl;
      return false;
   }
   std::cerr << arg_size << std::endl;
   std::cerr << dll_path.c_str() << std::endl;
   SIZE_T written;
   if (!::WriteProcessMemory(process, arg_addr, dll_path.c_str(), arg_size, &written)) {
      std::cerr << "WriteProcessMemory failed" << std::endl;
      return false;
   }
   std::cerr << "written:" << written << std::endl;
   PTHREAD_START_ROUTINE thread_routine = reinterpret_cast<PTHREAD_START_ROUTINE>(
      ::GetProcAddress(::GetModuleHandleA("kernel32"), "LoadLibraryA"));
   if (!thread_routine) {
      std::cerr << "GetProcAddress failed" << std::endl;
      return false;
   }
   HANDLE thread = ::CreateRemoteThread(process, NULL, 0, thread_routine, arg_addr, 0, NULL);
   if (!thread) {
      std::cerr << "CreateRemoteThread failed" << std::endl;
      return false;
   }
   ::WaitForSingleObject(thread, INFINITE);
   ::CloseHandle(thread);
   ::VirtualFreeEx(process, arg_addr, arg_size, MEM_RELEASE);
   return true;
}

void BuildDllPath(std::string& out)
{
    char str[0x800];
    ::GetModuleFileNameA(NULL, str, sizeof(str)/sizeof(str[0]));
    out.append(str, strrchr(str, '\\'));
    out.append("\\");
    out.append(dll_name);
}

int main(int argc, char** argv){
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ::ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ::ZeroMemory(&pi, sizeof(pi));
    int buff_len = exe_path.size() + 1;
    char* current_dir = new char[buff_len];
    ::strcpy_s(current_dir, buff_len, exe_path.c_str());
    ::PathRemoveFileSpecA(current_dir);
    if(!::CreateProcess(exe_path.c_str(), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED, NULL, current_dir, &si, &pi)){
        return 1;
    }
    delete[] current_dir;
    std::string dll_path;
    BuildDllPath(dll_path);
    std::cerr << "DLL path:" << dll_path << std::endl;
    if(!InjectDll(pi.hProcess, dll_path)){
        ::CloseHandle(pi.hProcess);
        ::CloseHandle(pi.hThread);
        return 1;
    }
    ::ResumeThread(pi.hThread);
    ::CloseHandle(pi.hThread);
    ::CloseHandle(pi.hProcess);
    return 0;
}