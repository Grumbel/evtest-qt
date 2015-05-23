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

#ifndef HEADER_XBOXDRV_EVDEV_CONTROLLER_HPP
#define HEADER_XBOXDRV_EVDEV_CONTROLLER_HPP

#include <QObject>
#include <QSocketNotifier>

#include <linux/input.h>
#include <string>
#include <queue>
#include <memory>

class EvdevController : public QObject
{
  Q_OBJECT

private:
  int m_fd;
  std::string m_filename;

  std::unique_ptr<QSocketNotifier> m_notifier;

  std::string m_name;
  bool m_grab;
  bool m_debug;

  typedef std::map<int, int> EvMap;

  std::vector<struct input_absinfo> m_absinfo;

public:
  static std::unique_ptr<EvdevController> open(const std::string& filename);

private:
  EvdevController(int fd, const std::string& filename,
                  bool grab = false,
                  bool debug = false);

  void start_qsocket_notifier();
  //bool parse(const struct input_event& ev, ControllerMessage& msg_inout) const;
  void read_data_to_buffer();
  void on_data();

public:
  ~EvdevController();

  int get_fd() const { return m_fd; }

signals:
  void event(const input_event& ev);

private:
  EvdevController(const EvdevController&) = delete;
  EvdevController& operator=(const EvdevController&) = delete;
};

#endif

/* EOF */
