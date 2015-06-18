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

#include <QMainWindow>
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

class EvtestApp : public QObject
{
  Q_OBJECT

private:
  QMainWindow m_window;
  QWidget m_widget;

  QVBoxLayout m_vbox_layout;
  QComboBox m_evdev_list_box;

  std::unique_ptr<QWidget> m_ev_widget;

  std::unique_ptr<EvdevDevice> m_device;
  std::unique_ptr<EvdevState> m_state;
  std::unique_ptr<QSocketNotifier> m_notifier;

public:
  EvtestApp();

  void refresh_device_list();
  void select_device(const QString& device);

private:
  void on_data(EvdevDevice& device, EvdevState& state);
  void on_device_change(const std::string& filename);

public slots:
  void on_item_change(int index);

private:
  EvtestApp(const EvtestApp&) = delete;
  EvtestApp& operator=(const EvtestApp&) = delete;
};

#endif

/* EOF */
