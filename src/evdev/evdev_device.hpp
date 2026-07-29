// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_EVDEV_DEVICE_HPP
#define HEADER_EVTEST_QT_EVDEV_DEVICE_HPP

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
  static std::unique_ptr<EvdevDevice> open(std::string const& filename);
  EvdevDevice(int fd, std::string const& filename);
  ~EvdevDevice();

  EvdevInfo read_evdev_info();
  ssize_t read_events(struct input_event* ev, size_t count);
  int get_fd() const { return m_fd; }

private:
  EvdevDevice(EvdevDevice const&) = delete;
  EvdevDevice& operator=(EvdevDevice const&) = delete;
};

} // namespace evtest_qt

#endif

/* EOF */
