#include "rzxpch.h"
#include "Razix/Core/OS/RZFileSystem.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#ifdef RAZIX_PLATFORM_WINDOWS
#include <Windows.h>
#include <wtypes.h>

namespace Razix
{

    static HANDLE OpenFileForReading(const std::string& path)
    {
        return CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
    }

    static int64_t GetFileSizeInternal(const HANDLE file)
    {
        LARGE_INTEGER size;
        GetFileSizeEx(file, &size);
        return size.QuadPart;
    }

    static bool ReadFileInternal(const HANDLE file, void* buffer, const int64_t size)
    {
        OVERLAPPED ol = { 0 };
#pragma warning(push, 0)
        return ReadFileEx(file, buffer, static_cast<DWORD>(size), &ol, nullptr) != 0;
#pragma warning(pop)
    }

    bool RZRZFileSystem::FileExists(const std::string& path)
    {
        auto dwAttr = GetFileAttributes((LPCSTR)path.c_str());
        return (dwAttr != INVALID_FILE_ATTRIBUTES) && (dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }

    bool RZRZFileSystem::FolderExists(const std::string& path)
    {
        DWORD dwAttrib = GetFileAttributes(path.c_str());
        return dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    int64_t RZRZFileSystem::GetFileSize(const std::string& path)
    {
        const HANDLE file = OpenFileForReading(path);
        if(file == INVALID_HANDLE_VALUE)
            return -1;
        int64_t result = GetFileSizeInternal(file);
        CloseHandle(file);

        return result;
    }

    bool RZRZFileSystem::ReadFile(const std::string& path, void* buffer, int64_t size)
    {
        const HANDLE file = OpenFileForReading(path);
        if(file == INVALID_HANDLE_VALUE)
            return false;

        if(size < 0)
            size = GetFileSizeInternal(file);

        bool result = ReadFileInternal(file, buffer, size);
        CloseHandle(file);
        return result;
    }

    uint8_t* RZRZFileSystem::ReadFile(const std::string& path)
    {
        const HANDLE file = OpenFileForReading(path);
        const int64_t size = GetFileSizeInternal(file);
        uint8_t* buffer = new uint8_t[static_cast<uint32_t>(size)];
        const bool result = ReadFileInternal(file, buffer, size);
        CloseHandle(file);
        if(!result)
            delete[] buffer;
        return result ? buffer : nullptr;
    }

    std::string RZRZFileSystem::ReadTextFile(const std::string& path)
    {
        const HANDLE file = OpenFileForReading(path);
        const int64_t size = GetFileSizeInternal(file);
        std::string result(static_cast<uint32_t>(size), 0);
        const bool success = ReadFileInternal(file, &result[0], size);
        CloseHandle(file);
        if(success)
        {
            // Strip carriage returns
            result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
        }
        return success ? result : std::string();
    }

    bool RZRZFileSystem::WriteFile(const std::string& path, uint8_t* buffer)
    {
        const HANDLE file = CreateFile(path.c_str(), GENERIC_WRITE, NULL, nullptr, CREATE_NEW | OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if(file == INVALID_HANDLE_VALUE)
            return false;

        const int64_t size = GetFileSizeInternal(file);
        DWORD written;
        const bool result = ::WriteFile(file, buffer, static_cast<DWORD>(size), &written, nullptr) != 0;
        CloseHandle(file);
        return result;
    }

    bool RZRZFileSystem::WriteTextFile(const std::string& path, const std::string& text)
    {
        return WriteFile(path, (uint8_t*)&text[0]);
    }
}

#endif

namespace Razix
{
static bool ReadFileInternal(FILE* file, void* buffer, int64_t size, bool readbytemode)
    {
        int64_t read_size;
        if(readbytemode)
            read_size = fread(buffer, sizeof(uint8_t), size, file);
        else
            read_size = fread(buffer, sizeof(char), size, file);

        if(size != read_size)
        {
            //delete[] buffer;
            //buffer = NULL;
            return false;
        }
        else
            return true;
    }

    bool RZFileSystem::FileExists(const std::string& path)
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
        // return access( path.c_str(), F_OK ) == 0
    }

    bool RZFileSystem::FolderExists(const std::string& path)
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }

    int64_t RZFileSystem::GetFileSize(const std::string& path)
    {
        if(!FileExists(path))
            return -1;
        struct stat buffer;
        stat(path.c_str(), &buffer);
        return buffer.st_size;
    }

    bool RZFileSystem::ReadFile(const std::string& path, void* buffer, int64_t size)
    {
        if(!FileExists(path))
            return false;
        if(size < 0)
            size = GetFileSize(path);
        buffer = new uint8_t[size + 1];
        FILE* file = fopen(path.c_str(), RZFileSystem::GetFileOpenModeString(FileOpenFlags::READ));
        bool result = false;
        if(file)
        {
            result = ReadFileInternal(file, buffer, size, true);
            fclose(file);
        }
        //        else
        //        {
        //            delete[] buffer;
        //            return false;
        //        }
        return result;
    }

    uint8_t* RZFileSystem::ReadFile(const std::string& path)
    {
        if(!FileExists(path))
            return nullptr;
        int64_t size = GetFileSize(path);
        FILE* file = fopen(path.c_str(), RZFileSystem::GetFileOpenModeString(FileOpenFlags::READ));
        uint8_t* buffer = new uint8_t[size];
        bool result = ReadFileInternal(file, buffer, size, true);
        fclose(file);
        if(!result && buffer)
            delete[] buffer;
        return result ? buffer : nullptr;
    }

    std::string RZFileSystem::ReadTextFile(const std::string& path)
    {
        if(!FileExists(path))
            return std::string();
        int64_t size = GetFileSize(path);
        FILE* file = fopen(path.c_str(), RZFileSystem::GetFileOpenModeString(FileOpenFlags::READ));
        std::string result(size, 0);
        bool success = ReadFileInternal(file, &result[0], size, false);
        fclose(file);
        if(success)
        {
            // Strip carriage returns
            result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
        }
        return success ? result : std::string();
    }

    bool RZFileSystem::WriteFile(const std::string& path, uint8_t* buffer, uint32_t size)
    {
        FILE* file = fopen(path.c_str(), RZFileSystem::GetFileOpenModeString(FileOpenFlags::WRITE));
        if(file == NULL) //if file does not exist, create it
        {
            file = fopen(path.c_str(), RZFileSystem::GetFileOpenModeString(FileOpenFlags::WRITE_READ));
        }

        if(file == nullptr)
        {
            switch(errno)
            {
            case ENOENT:
            {
                RAZIX_CORE_ERROR("File not found : {0}", path);
            }
            break;
            default:
            {
                RAZIX_CORE_ERROR("File can't open : {0}", path);
            }
            break;
            }
            return false;
        }

        size_t output = 0;
        if(buffer)
        {
            output = fwrite(buffer, 1, size, file);
        }
        fclose(file);
        return output > 0;
    }

    bool RZFileSystem::WriteTextFile(const std::string& path, const std::string& text)
    {
        FILE* file = fopen(path.c_str(), RZFileSystem::GetFileOpenModeString(FileOpenFlags::WRITE));
        if(file == NULL) //if file does not exist, create it
        {
            file = fopen(path.c_str(), RZFileSystem::GetFileOpenModeString(FileOpenFlags::WRITE_READ));
        }

        if(file == nullptr)
        {
            switch(errno)
            {
            case ENOENT:
            {
                RAZIX_CORE_ERROR("File not found : {0}", path);
            }
            break;
            default:
            {
                RAZIX_CORE_ERROR("File can't open : {0}", path);
            }
            break;
            }
            return false;
        }
        else
        {
            size_t size = fwrite(text.c_str(), 1, strlen(text.c_str()), file);
            fclose(file);
            return size > 0;
        }
    }

//    std::string RZFileSystem::GetWorkingDirectory()
//    {
//        const size_t pathSize = 4096;
//        char currentDir[pathSize];
//        if(getcwd(currentDir, pathSize) != NULL)
//            RAZIX_CORE_ERROR(currentDir);
//
//        return std::string(currentDir);
//    }
}
