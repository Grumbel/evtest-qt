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

#ifndef HEADER_EVTEST_QT_EVDEV_WIDGET_HPP
#define HEADER_EVTEST_QT_EVDEV_WIDGET_HPP

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QSocketNotifier>
#include <QVBoxLayout>
#include <QComboBox>

#include <fcntl.h>
#include <iostream>
#include <linux/input.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include "axis_widget.hpp"
#include "button_widget.hpp"
#include "evdev/evdev_device.hpp"
#include "evdev/evdev_enum.hpp"
#include "evdev/evdev_list.hpp"
#include "evdev/evdev_state.hpp"
#include "fwd.hpp"
#include "rel_widget.hpp"

namespace evtest_qt {

class EvdevWidget : public QWidget
{
  Q_OBJECT

public:
  EvdevWidget(EvdevInfo const& info, QWidget* parent = nullptr);
  virtual ~EvdevWidget();

  void set_state(EvdevState const& state);
  void set_verification_mode(bool value);

private:
  QVBoxLayout m_vbox_layout;

  QGridLayout m_info_layout;
  QGridLayout m_axis_layout;
  QGridLayout m_rel_layout;
  QGridLayout m_button_layout;

  QLabel m_driver_version_label;
  QLabel m_device_id_label;
  QLabel m_device_name_label;
  QLabel m_device_phys_label;

  QLabel m_driver_version_v_label;
  QLabel m_device_id_v_label;
  QLabel m_device_name_v_label;
  QLabel m_device_phys_v_label;

  std::vector<AxisWidget*> m_axis_widgets;
  std::vector<ButtonWidget*> m_button_widgets;
  std::vector<RelWidget*> m_rel_widgets;
  std::vector<MultitouchWidget*> m_multitouch_widgets;

private:
  EvdevWidget(EvdevWidget const&) = delete;
  EvdevWidget& operator=(EvdevWidget const&) = delete;
};

} // namespace evtest_qt

#endif

/* EOF */
