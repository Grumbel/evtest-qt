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

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QSocketNotifier>
#include <QVBoxLayout>

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
#include "evdev_state.hpp"

void on_data(EvdevDevice& device, EvdevState& state)
{
  std::array<struct input_event, 128> ev;
  while(true)
  {
    ssize_t num_events = device.read_events(ev.data(), ev.size());
    if (num_events < 0)
    {
      std::cout << "error: " << num_events << ": " << strerror(errno) << std::endl;
      return;
    }
    else if (num_events == 0)
    {
      return;
    }
    else
    {
      for(ssize_t i = 0; i < num_events; ++i)
      {
        state.update(ev[i]);
      }
    }
  }
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  auto device = EvdevDevice::open(argv[1]);
  auto info = device->read_evdev_info();

  EvdevState state(info);

  QWidget window;

  QVBoxLayout vbox_layout(&window);

  QGridLayout axis_layout;
  //std::vector<std::unique_ptr<QWidget> > widgets;
  for(size_t i = 0; i < info.abss.size(); ++i)
  {
    AbsInfo absinfo = info.get_absinfo(info.abss[i]);
    auto label = std::make_unique<QLabel>(QString::fromStdString(evdev_abs_names[info.abss[i]] + ":"));
    auto axis_widget = std::make_unique<AxisWidget>(info.abss[i], absinfo.minimum, absinfo.maximum);

    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    axis_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // connect all the signals
    //state.on_update().connect(axis_widget.set_value());
    QObject::connect(&state, &EvdevState::sig_change,
                     axis_widget.get(), &AxisWidget::on_change);


    axis_layout.addWidget(label.release(), i, 0, Qt::AlignRight);
    axis_layout.addWidget(axis_widget.release(), i, 1);

    //widgets.push_back(std::move(label));
    //widgets.push_back(std::move(axis_widget));
  }

  QGridLayout button_layout;
  int row = 0;
  int col = 0;
  for(size_t i = 0; i < info.keys.size(); ++i)
  {
    auto button_widget = std::make_unique<ButtonWidget>(info.keys[i]);

    QObject::connect(&state, &EvdevState::sig_change,
                     button_widget.get(), &ButtonWidget::on_change);

    button_layout.addWidget(button_widget.release(), row, col);

    col += 1;
    if (col > 6)
    {
      col = 0;
      row += 1;
    }

    //widgets.push_back(std::move(button_widget));
  }

  vbox_layout.addLayout(&axis_layout);
  vbox_layout.addLayout(&button_layout);

  int retval;
  {
    QSocketNotifier notifier(device->get_fd(), QSocketNotifier::Read);
    QObject::connect(&notifier, &QSocketNotifier::activated,
                     [&device, &state](int fd)
                     {
                       on_data(*device, state);
                     }
      );

    window.show();
    retval = app.exec();
  }

  return retval;
}

/* EOF */
