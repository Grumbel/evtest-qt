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

#include "multitouch_widget.hpp"

#include <iostream>

namespace evtest_qt {

EvdevWidget::EvdevWidget(EvdevInfo const& info, QWidget* parent_) :
  QWidget(parent_),
  m_vbox_layout(this),
  m_info_layout(),
  m_axis_layout(),
  m_rel_layout(),
  m_button_layout(),
  m_driver_version_label("Input driver version:"),
  m_device_id_label("Input device ID:"),
  m_device_name_label("Input device name:"),
  m_device_phys_label("Input device phys:"),
  m_driver_version_v_label(),
  m_device_id_v_label(),
  m_device_name_v_label(),
  m_device_phys_v_label(),
  m_axis_widgets(),
  m_button_widgets(),
  m_rel_widgets(),
  m_multitouch_widgets()
{
  m_info_layout.setColumnStretch(0, 0);
  m_info_layout.setColumnStretch(1, 1);
  m_info_layout.addWidget(&m_driver_version_label, 0, 0);
  m_info_layout.addWidget(&m_driver_version_v_label, 0, 1);

  m_info_layout.addWidget(&m_device_id_label, 1, 0);
  m_info_layout.addWidget(&m_device_id_v_label, 1, 1);

  m_info_layout.addWidget(&m_device_name_label, 2, 0);
  m_info_layout.addWidget(&m_device_name_v_label, 2, 1);

  m_info_layout.addWidget(&m_device_phys_label, 3, 0);
  m_info_layout.addWidget(&m_device_phys_v_label, 3, 1);

  m_vbox_layout.addLayout(&m_info_layout);
  m_vbox_layout.addLayout(&m_axis_layout);
  m_vbox_layout.addLayout(&m_rel_layout);

  if (info.has_abs(ABS_MT_SLOT))
  {
    auto multitouch_widget = std::make_unique<MultitouchWidget>();
    multitouch_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    m_multitouch_widgets.emplace_back(multitouch_widget.get());
    m_vbox_layout.addWidget(multitouch_widget.release());
  }

  m_vbox_layout.addLayout(&m_button_layout);

  {
    auto str = QString("%1.%2.%3")
      .arg(info.version >> 16)
      .arg((info.version >> 8) & 0xff)
      .arg(info.version & 0xff);
    m_driver_version_v_label.setText(str);
  }

  {
    auto str = QString("bus: 0x%1 vendor: 0x%2 product: 0x%3 version: 0x%4")
      .arg(info.id.bustype, 0, 16)
      .arg(info.id.vendor, 0, 16)
      .arg(info.id.product, 0, 16)
      .arg(info.id.version, 0, 16);
    m_device_id_v_label.setText(str);
  }
  m_device_name_v_label.setText(QString::fromStdString(info.name));
  m_device_phys_v_label.setText(QString::fromStdString(info.phys));

  // axis widgets
  for(size_t i = 0; i < info.abss.size(); ++i)
  {
    AbsInfo absinfo = info.get_absinfo(info.abss[i]);
    auto label = std::make_unique<QLabel>(QString::fromStdString(evdev_abs_name(info.abss[i]) + ":"));
    auto axis_widget = std::make_unique<AxisWidget>(info.abss[i], absinfo.minimum, absinfo.maximum);

    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    axis_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    m_axis_widgets.emplace_back(axis_widget.get());
    m_axis_layout.addWidget(label.release(), static_cast<int>(i), 0, Qt::AlignRight);
    m_axis_layout.addWidget(axis_widget.release(), static_cast<int>(i), 1);
  }

  // rel widgets
  for(size_t i = 0; i < info.rels.size(); ++i)
  {
    auto label = std::make_unique<QLabel>(QString::fromStdString(evdev_rel_name(info.rels[i]) + ":"));
    auto rel_widget = std::make_unique<RelWidget>(info.rels[i]);

    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    rel_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    m_rel_widgets.emplace_back(rel_widget.get());
    m_rel_layout.addWidget(label.release(), static_cast<int>(i), 0, Qt::AlignRight);
    m_rel_layout.addWidget(rel_widget.release(), static_cast<int>(i), 1);
  }

  // button widgets
  int row = 0;
  int col = 0;
  for(size_t i = 0; i < info.keys.size(); ++i)
  {
    auto button_widget = std::make_unique<ButtonWidget>(info.keys[i]);

    button_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_button_widgets.emplace_back(button_widget.get());
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

void
EvdevWidget::set_state(EvdevState const& state)
{
  for (auto* const widget : m_axis_widgets) {
    widget->on_change(state);
  }

  for (auto* const widget : m_button_widgets) {
    widget->on_change(state);
  }

  for (auto* const widget : m_rel_widgets) {
    widget->on_change(state);
  }

  for (auto* const widget : m_multitouch_widgets) {
    widget->on_change(state);
  }
}

void
EvdevWidget::set_verification_mode(bool value)
{
  for (auto* const widget : m_axis_widgets) {
    widget->set_verification_mode(value);
  }

  for (auto* const widget : m_button_widgets) {
    widget->set_verification_mode(value);
  }

  for (auto* const widget : m_rel_widgets) {
    widget->set_verification_mode(value);
  }

  for (auto* const widget : m_multitouch_widgets) {
    widget->set_verification_mode(value);
  }

}

} // namespace evtest_qt

/* EOF */
