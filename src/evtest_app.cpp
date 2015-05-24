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

#include "evtest_app.hpp"

EvtestApp::EvtestApp() :
  m_window(),
  m_vbox_layout(&m_window),
  m_evdev_list_box(),
  m_driver_version_label("Input driver version:"),
  m_device_id_label("Input device ID:"),
  m_device_name_label("Input device name:"),
  m_driver_version_v_label(),
  m_device_id_v_label(),
  m_device_name_v_label(),
  m_device()
{
  m_info_layout.addWidget(&m_driver_version_label, 0, 0);
  m_info_layout.addWidget(&m_driver_version_v_label, 0, 1);

  m_info_layout.addWidget(&m_device_id_label, 1, 0);
  m_info_layout.addWidget(&m_device_id_v_label, 1, 1);

  m_info_layout.addWidget(&m_device_name_label, 2, 0);
  m_info_layout.addWidget(&m_device_name_v_label, 2, 1);

  m_vbox_layout.addWidget(&m_evdev_list_box);
  m_vbox_layout.addLayout(&m_info_layout);
  m_vbox_layout.addLayout(&m_axis_layout);
  m_vbox_layout.addLayout(&m_button_layout);

  m_vbox_layout.setSizeConstraint(QLayout::SetFixedSize);

  QObject::connect(
    &m_evdev_list_box, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
    [this](int index){ on_item_change(index); });

  m_window.show();
}

void
EvtestApp::refresh_device_list()
{
  m_evdev_list_box.clear();
  for(auto dev : EvdevList::scan("/dev/input"))
  {
    auto dev_fp = EvdevDevice::open(dev);
    auto info = dev_fp->read_evdev_info();

    std::ostringstream str;
    str << dev.substr(11) << ": " << info.name;
    m_evdev_list_box.addItem(str.str().c_str(), QString::fromStdString(dev));
  }
}

void
EvtestApp::on_item_change(int index)
{
  std::string dev = m_evdev_list_box.itemData(index).toString().toUtf8().data();
  std::cout << "activated: " << dev << std::endl;
  on_device_change(dev);
}

void
EvtestApp::on_data(EvdevDevice& device, EvdevState& state)
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

void
EvtestApp::refresh_ev_widgets(const EvdevInfo& info)
{
  QLayoutItem* item;
  while((item = m_axis_layout.itemAt(0)) != nullptr)
  {
    delete item->widget();
  }

  while((item = m_button_layout.itemAt(0)) != nullptr)
  {
    delete item->widget();
  }

  // axis widgets
  for(size_t i = 0; i < info.abss.size(); ++i)
  {
    AbsInfo absinfo = info.get_absinfo(info.abss[i]);
    auto label = std::make_unique<QLabel>(QString::fromStdString(evdev_abs_names[info.abss[i]] + ":"));
    auto axis_widget = std::make_unique<AxisWidget>(info.abss[i], absinfo.minimum, absinfo.maximum);

    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    axis_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QObject::connect(m_state.get(), &EvdevState::sig_change,
                     axis_widget.get(), &AxisWidget::on_change);

    m_axis_layout.addWidget(label.release(), i, 0, Qt::AlignRight);
    m_axis_layout.addWidget(axis_widget.release(), i, 1);
  }

  // button widgets
  int row = 0;
  int col = 0;
  for(size_t i = 0; i < info.keys.size(); ++i)
  {
    auto button_widget = std::make_unique<ButtonWidget>(info.keys[i]);

    QObject::connect(m_state.get(), &EvdevState::sig_change,
                     button_widget.get(), &ButtonWidget::on_change);

    button_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_button_layout.addWidget(button_widget.release(), row, col);

    col += 1;
    if (col > 6)
    {
      col = 0;
      row += 1;
    }
  }
}

void
EvtestApp::on_device_change(const std::string& filename)
{
  m_notifier.reset();

  m_device = EvdevDevice::open(filename);
  auto info = m_device->read_evdev_info();

  m_driver_version_v_label.setText("(placeholder) 1.0.1");
  m_device_id_v_label.setText("(placeholder) bus 0x3 vendor 0x45e product 0x28e version 0x110");
  m_device_name_v_label.setText(info.name.c_str());

  m_state = std::make_unique<EvdevState>(info);

  refresh_ev_widgets(info);

  m_notifier = std::make_unique<QSocketNotifier>(m_device->get_fd(), QSocketNotifier::Read);
  QObject::connect(
    m_notifier.get(), &QSocketNotifier::activated,
    [this](int fd) { on_data(*m_device, *m_state); });
}

/* EOF */
