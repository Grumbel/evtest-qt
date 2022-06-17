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

#ifndef HEADER_EVDEV_DEVICE_HPP
#define HEADER_EVDEV_DEVICE_HPP

#include <memory>
#include <string>

#include "evdev_info.hpp"

namespace evtest_qt {

class EvdevDevice
{
private:
  int m_fd;
  std::string m_filename;

public:
  static std::unique_ptr<EvdevDevice> open(const std::string& filename);
  EvdevDevice(int fd, const std::string& filename);
  ~EvdevDevice();

  EvdevInfo read_evdev_info();
  ssize_t read_events(struct input_event* ev, size_t count);
  int get_fd() const { return m_fd; }

private:
  EvdevDevice(const EvdevDevice&) = delete;
  EvdevDevice& operator=(const EvdevDevice&) = delete;
};

} // namespace evtest_qt

#endif

/* EOF */
