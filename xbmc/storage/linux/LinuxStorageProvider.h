#pragma once
/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <vector>

#include "storage/IStorageProvider.h"
#include "UDevProvider.h"
#ifdef HAS_DBUS
#include "UDisksProvider.h"
#endif
#include "PosixMountProvider.h"

class CLinuxStorageProvider : public IStorageProvider
{
public:
  CLinuxStorageProvider()
  {
    m_instance = NULL;

#ifdef HAS_DBUS
    if (CUDisksProvider::HasUDisks())
      m_instance = new CUDisksProvider();
#endif
#ifdef HAVE_LIBUDEV
    if (m_instance == NULL)
      m_instance = new CUDevProvider();
#endif

    if (m_instance == NULL)
      m_instance = new CPosixMountProvider();
  }

  ~CLinuxStorageProvider() override
  {
    delete m_instance;
  }

  void Initialize() override
  {
    m_instance->Initialize();
  }

  void Stop() override
  {
    m_instance->Stop();
  }

  void GetLocalDrives(VECSOURCES &localDrives) override
  {
    // Home directory
    CMediaSource share;
    share.strPath = getenv("HOME");
    share.strName = g_localizeStrings.Get(21440);
    share.m_ignore = true;
    share.m_iDriveType = CMediaSource::SOURCE_TYPE_LOCAL;
    localDrives.push_back(share);
    share.strPath = "/";
    share.strName = g_localizeStrings.Get(21453);
    localDrives.push_back(share);

    m_instance->GetLocalDrives(localDrives);
  }

  void GetRemovableDrives(VECSOURCES &removableDrives) override
  {
    m_instance->GetRemovableDrives(removableDrives);
  }

  bool Eject(const std::string& mountpath) override
  {
    return m_instance->Eject(mountpath);
  }

  std::vector<std::string> GetDiskUsage() override
  {
    return m_instance->GetDiskUsage();
  }

  bool PumpDriveChangeEvents(IStorageEventsCallback *callback) override
  {
    return m_instance->PumpDriveChangeEvents(callback);
  }

private:
  IStorageProvider *m_instance;
};
