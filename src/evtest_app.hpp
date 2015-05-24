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

#ifndef HEADER_EVTEST_APP_HPP
#define HEADER_EVTEST_APP_HPP

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

#include "axis_widget.hpp"
#include "button_widget.hpp"
#include "evdev_device.hpp"
#include "evdev_enum.hpp"
#include "evdev_list.hpp"
#include "evdev_state.hpp"

class EvdevState;
class EvdevDevice;

class EvtestApp
{
private:
  QWidget m_window;
  QVBoxLayout m_vbox_layout;
  QComboBox m_evdev_list_box;

  QGridLayout m_info_layout;
  QGridLayout m_axis_layout;
  QGridLayout m_button_layout;

  QLabel m_driver_version_label;
  QLabel m_device_id_label;
  QLabel m_device_name_label;

  QLabel m_driver_version_v_label;
  QLabel m_device_id_v_label;
  QLabel m_device_name_v_label;

  std::unique_ptr<EvdevDevice> m_device;
  std::unique_ptr<EvdevState> m_state;
  std::unique_ptr<QSocketNotifier> m_notifier;

public:
  EvtestApp();

  void refresh_device_list();

private:
  void clear_ev_widgets();
  void refresh_ev_widgets(const EvdevInfo& info);
  void on_data(EvdevDevice& device, EvdevState& state);
  void on_item_change(int index);
  void on_device_change(const std::string& filename);

private:
  EvtestApp(const EvtestApp&) = delete;
  EvtestApp& operator=(const EvtestApp&) = delete;
};

#endif

/* EOF */
