// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_EVDEV_ENUM_HPP
#define HEADER_EVTEST_QT_EVDEV_ENUM_HPP

#include <cstdint>
#include <string>

namespace evtest_qt {

std::string evdev_abs_name(uint16_t code);
std::string evdev_key_name(uint16_t code);
std::string evdev_rel_name(uint16_t code);

} // namespace evtest_qt

#endif

/* EOF */
