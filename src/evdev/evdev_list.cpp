// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "evdev_list.hpp"

#include <algorithm>
#include <format>
#include <stdexcept>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>

namespace evtest_qt {

std::vector<std::string>
EvdevList::scan(std::string const& evdev_directory)
{
  DIR* dirp = opendir(evdev_directory.c_str());
  if (!dirp)
  {
    throw std::runtime_error(std::format("{}: {}", evdev_directory, strerror(errno)));
  }

  std::vector<std::string> devices;

  struct dirent* dentry;
  while ((dentry = readdir(dirp)) != nullptr)
  {
    // Valid event device names are event0-31,256-1023, see:
    // https://github.com/torvalds/linux/blob/master/Documentation/admin-guide/devices.txt
    // https://github.com/torvalds/linux/blob/master/drivers/input/input.c
    if (fnmatch("event[0-9]", dentry->d_name, FNM_PATHNAME) == 0 ||
        fnmatch("event[0-9][0-9]", dentry->d_name, FNM_PATHNAME) == 0 ||
        fnmatch("event[0-9][0-9][0-9]", dentry->d_name, FNM_PATHNAME) == 0 ||
        fnmatch("event[0-9][0-9][0-9][0-9]", dentry->d_name, FNM_PATHNAME) == 0)
    {
      devices.push_back(std::format("{}/{}", evdev_directory, dentry->d_name));
    }
  }
  closedir(dirp);

  // numerical sort (i.e. event2 < event10)
  std::sort(devices.begin(), devices.end(),
            [](std::string const& lhs, std::string const& rhs) {
              if (lhs.size() != rhs.size()) {
                return lhs.size() < rhs.size();
              }
              return lhs < rhs;
            });

  return devices;
}

} // namespace evtest_qt

/* EOF */
