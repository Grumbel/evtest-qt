// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_EVDEV_LIST_HPP
#define HEADER_EVTEST_QT_EVDEV_LIST_HPP

#include <string>
#include <vector>

namespace evtest_qt {

class EvdevList
{
public:
  static std::vector<std::string> scan(std::string const& evdev_directory);

private:
  EvdevList(EvdevList const&) = delete;
  EvdevList& operator=(EvdevList const&) = delete;
};

} // namespace evtest_qt

#endif

/* EOF */
