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

#ifndef HEADER_EVTEST_QT_EVTEST_APP_HPP
#define HEADER_EVTEST_QT_EVTEST_APP_HPP

#include <QAction>
#include <QApplication>
#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QSocketNotifier>
#include <QTimer>
#include <QVBoxLayout>

#include <memory>
#include <string>

#include "evdev/evdev_device.hpp"
#include "evdev/evdev_list.hpp"
#include "evdev/evdev_state.hpp"
#include "evdev/udev_monitor.hpp"
#include "fwd.hpp"

namespace evtest_qt {

class EvtestApp : public QObject
{
  Q_OBJECT

public:
  EvtestApp(QApplication& app);
  ~EvtestApp() override;

  void refresh_device_list();
  void select_device(QString const& device);
  void set_verification_mode(bool value);

private:
  void on_data(EvdevDevice& device, EvdevState& state);
  void on_device_change(std::string const& filename);
  void clear_device_view(QString const& message);
  void append_log(QString const& line);
  void schedule_device_list_refresh();

public slots:
  void on_item_change(int index);
  void on_shrink_action();
  void on_notification(int);
  void on_udev_device_added(QString const& devnode, QString const& name);
  void on_udev_device_removed(QString const& devnode, QString const& name);
  void on_udev_monitor_error(QString const& message);
  void on_refresh_timeout();

private:
  QApplication& m_app;
  QMainWindow m_window;
  QWidget m_widget;

  QAction m_action_exit;
  QAction m_action_show_about;
  QAction m_action_verification_mode;

  QVBoxLayout m_vbox_layout;
  QComboBox m_evdev_list_box;
  QPlainTextEdit m_device_log;

  std::unique_ptr<EvdevWidget> m_ev_widget;
  std::unique_ptr<QLabel> m_ev_widget_placeholder;

  std::unique_ptr<EvdevDevice> m_device;
  std::unique_ptr<EvdevState> m_state;
  std::unique_ptr<QSocketNotifier> m_notifier;

  std::unique_ptr<UdevInputMonitor> m_udev_monitor;
  QTimer m_refresh_timer;
  QString m_current_device; // path of device shown in the UI, if any

private:
  EvtestApp(EvtestApp const&) = delete;
  EvtestApp& operator=(EvtestApp const&) = delete;
};

} // namespace evtest_qt

#endif

/* EOF */
