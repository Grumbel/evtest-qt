// jstest-qt - A graphical joystick tester
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

#include <QApplication>
#include <QSocketNotifier>
#include <QMainWindow>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <linux/input.h>

#include "axis_widget.hpp"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  QMainWindow window;
  AxisWidget axis_widget;

  int fd = open(argv[1], O_RDWR | O_NONBLOCK);
  QSocketNotifier notifier(fd, QSocketNotifier::Read);
  QObject::connect(&notifier, &QSocketNotifier::activated,
                   [&axis_widget](int fd){
                     struct input_event ev;
                     if (read(fd, &ev, sizeof(ev)) < 0)
                     {
                       std::cout << "error" << std::endl;
                     }
                     else
                     {
                       if (ev.type == EV_ABS && ev.code == ABS_X)
                       {
                         axis_widget.set_axis_pos(ev.value);

                         std::cout << "ev type:" << ev.type
                                   << " code:" << ev.code
                                   << " value:" << ev.value << std::endl;
                       }
                     }
                   });

  window.show();
  axis_widget.show();

  return app.exec();
}

/* EOF */
