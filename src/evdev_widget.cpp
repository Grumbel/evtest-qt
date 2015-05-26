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

#include "evdev_widget.hpp"

#include <iostream>

EvdevWidget::EvdevWidget(const EvdevState& state, const EvdevInfo& info, QWidget* parent_) :
  QWidget(parent_),
  m_vbox_layout(this),
  m_info_layout(),
  m_axis_layout(),
  m_rel_layout(),
  m_button_layout(),
  m_driver_version_label("Input driver version:"),
  m_device_id_label("Input device ID:"),
  m_device_name_label("Input device name:"),
  m_driver_version_v_label(),
  m_device_id_v_label(),
  m_device_name_v_label()
{
  m_info_layout.addWidget(&m_driver_version_label, 0, 0);
  m_info_layout.addWidget(&m_driver_version_v_label, 0, 1);

  m_info_layout.addWidget(&m_device_id_label, 1, 0);
  m_info_layout.addWidget(&m_device_id_v_label, 1, 1);

  m_info_layout.addWidget(&m_device_name_label, 2, 0);
  m_info_layout.addWidget(&m_device_name_v_label, 2, 1);

  m_vbox_layout.addLayout(&m_info_layout);
  m_vbox_layout.addLayout(&m_axis_layout);
  m_vbox_layout.addLayout(&m_rel_layout);
  m_vbox_layout.addLayout(&m_button_layout);

  m_driver_version_v_label.setText("(placeholder) 1.0.1");
  m_device_id_v_label.setText("(placeholder) bus 0x3 vendor 0x45e product 0x28e version 0x110");
  m_device_name_v_label.setText(info.name.c_str());

  // axis widgets
  for(size_t i = 0; i < info.abss.size(); ++i)
  {
    AbsInfo absinfo = info.get_absinfo(info.abss[i]);
    auto label = std::make_unique<QLabel>(QString::fromStdString(evdev_abs_names[info.abss[i]] + ":"));
    auto axis_widget = std::make_unique<AxisWidget>(info.abss[i], absinfo.minimum, absinfo.maximum);

    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    axis_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QObject::connect(&state, &EvdevState::sig_change,
                     axis_widget.get(), &AxisWidget::on_change);

    m_axis_layout.addWidget(label.release(), static_cast<int>(i), 0, Qt::AlignRight);
    m_axis_layout.addWidget(axis_widget.release(), static_cast<int>(i), 1);
  }

  // rel widgets
  for(size_t i = 0; i < info.rels.size(); ++i)
  {
    auto label = std::make_unique<QLabel>(QString::fromStdString(evdev_rel_names[info.rels[i]] + ":"));
    auto rel_widget = std::make_unique<RelWidget>(info.rels[i]);

    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    rel_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QObject::connect(&state, &EvdevState::sig_change,
                     rel_widget.get(), &RelWidget::on_change);

    m_rel_layout.addWidget(label.release(), static_cast<int>(i), 0, Qt::AlignRight);
    m_rel_layout.addWidget(rel_widget.release(), static_cast<int>(i), 1);
  }

  // button widgets
  int row = 0;
  int col = 0;
  for(size_t i = 0; i < info.keys.size(); ++i)
  {
    auto button_widget = std::make_unique<ButtonWidget>(info.keys[i]);

    QObject::connect(&state, &EvdevState::sig_change,
                     button_widget.get(), &ButtonWidget::on_change);

    button_widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_button_layout.addWidget(button_widget.release(), row, col);

    col += 1;
    if (col > 6)
    {
      col = 0;
      row += 1;
    }
  }
}

EvdevWidget::~EvdevWidget()
{
}

/* EOF */
