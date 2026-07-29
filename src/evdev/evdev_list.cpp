// evtest-qt - A graphical joystick tester
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
