/*
 *  The ManaPlus Client
 *  Copyright (C) 2013-2017  The ManaPlus Developers
 *
 *  This file is part of The ManaPlus Client.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef USE_PHYSFS

#include "fs/virtfs.h"

#include "fs/physfs/virtfsphys.h"
#include "fs/virtfile.h"
#include "fs/virtfsfuncs.h"
#include "fs/virtlist.h"

#include "debug.h"

const char *dirSeparator = nullptr;

namespace VirtFs
{
    void init(const std::string &restrict name)
    {
        VirtFsPhys::init(name);
        updateDirSeparator();
    }

    void updateDirSeparator()
    {
        dirSeparator = VirtFsPhys::getDirSeparator();
    }

    const char *getDirSeparator()
    {
        return dirSeparator;
    }

    const char *getBaseDir()
    {
        return VirtFsPhys::getBaseDir();
    }

    const char *getUserDir()
    {
        return VirtFsPhys::getUserDir();
    }

    bool exists(std::string name)
    {
        return VirtFsPhys::exists(name);
    }

    VirtList *enumerateFiles(std::string dirName)
    {
        return VirtFsPhys::enumerateFiles(dirName);
    }

    bool isDirectory(std::string name)
    {
        return VirtFsPhys::isDirectory(name);
    }

    bool isSymbolicLink(const std::string &restrict name)
    {
        return VirtFsPhys::isSymbolicLink(name);
    }

    void freeList(VirtList *restrict const handle)
    {
        delete handle;
    }

    VirtFile *openRead(std::string filename)
    {
        return VirtFsPhys::openRead(filename);
    }

    VirtFile *openWrite(std::string filename)
    {
        return VirtFsPhys::openWrite(filename);
    }

    VirtFile *openAppend(std::string filename)
    {
        return VirtFsPhys::openAppend(filename);
    }

    bool setWriteDir(const std::string &restrict newDir)
    {
        return VirtFsPhys::setWriteDir(newDir);
    }

    bool addDirToSearchPath(std::string newDir,
                            const Append append)
    {
        return VirtFsPhys::addDirToSearchPath(newDir, append);
    }

    bool addDirToSearchPathSilent(std::string newDir,
                                  const Append append)
    {
        return VirtFsPhys::addDirToSearchPath(newDir, append);
    }

    bool removeDirFromSearchPath(std::string oldDir)
    {
        return VirtFsPhys::removeDirFromSearchPath(oldDir);
    }

    bool removeDirFromSearchPathSilent(std::string oldDir)
    {
        return VirtFsPhys::removeDirFromSearchPath(oldDir);
    }

    bool addZipToSearchPath(std::string newDir,
                            const Append append)
    {
        return VirtFsPhys::addZipToSearchPath(newDir, append);
    }

    bool removeZipFromSearchPath(std::string oldDir)
    {
        return VirtFsPhys::removeZipFromSearchPath(oldDir);
    }

    std::string getRealDir(std::string filename)
    {
        return VirtFsPhys::getRealDir(filename);
    }

    bool mkdir(const std::string &restrict dirname)
    {
        return VirtFsPhys::mkdir(dirname);
    }

    bool remove(const std::string &restrict filename)
    {
        return VirtFsPhys::remove(filename);
    }

    bool deinit()
    {
        return VirtFsPhys::deinit();
    }

    void permitLinks(const bool val)
    {
        VirtFsPhys::permitLinks(val);
    }

    const char *getLastError()
    {
        return VirtFsPhys::getLastError();
    }

    int close(VirtFile *restrict const file)
    {
        if (file == nullptr)
            return 0;
        return file->funcs->close(file);
    }

    int64_t read(VirtFile *restrict const file,
                 void *restrict const buffer,
                 const uint32_t objSize,
                 const uint32_t objCount)
    {
        return file->funcs->read(file,
            buffer,
            objSize,
            objCount);
    }

    int64_t write(VirtFile *restrict const file,
                  const void *restrict const buffer,
                  const uint32_t objSize,
                  const uint32_t objCount)
    {
        return file->funcs->write(file,
            buffer,
            objSize,
            objCount);
    }

    int64_t fileLength(VirtFile *restrict const file)
    {
        return file->funcs->fileLength(file);
    }

    int64_t tell(VirtFile *restrict const file)
    {
        return file->funcs->tell(file);
    }

    int seek(VirtFile *restrict const file,
             const uint64_t pos)
    {
        return file->funcs->seek(file,
            pos);
    }

    int eof(VirtFile *restrict const file)
    {
        return file->funcs->eof(file);
    }
}  // namespace VirtFs
#endif  // USE_PHYSFS