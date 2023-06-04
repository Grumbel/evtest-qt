// evtest-qt - A graphical joystick tester
// Copyright (C) 2022 Ingo Ruhnke <grumbel@gmail.com>
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

#include "evgraph_widget.hpp"

#include <QPainter>

#include "evdev/evdev_info.hpp"
#include "evdev/evdev_state.hpp"

namespace evtest_qt {

EvgraphWidget::EvgraphWidget(QWidget* parent) :
  QWidget(parent),
  m_history()
{
}

EvgraphWidget::~EvgraphWidget()
{
}

void
EvgraphWidget::paintEvent(QPaintEvent* event)
{
  if (m_history.empty()) {
    return;
  }

  if (width() == 0 || height() == 0) {
    return;
  }

  std::vector<QColor> const rainbow = {
    {255, 0, 0},
    {0, 255, 0},
    {0, 0, 255},
    {255, 255, 0},
    {0, 255, 255},
    {255, 0, 255},
  };

  QPainter painter(this);

  EvdevInfo const& info = m_history.front().get_info();

  painter.fillRect(0, 0, width(), height(), QColor(0, 0, 0));

  for (size_t i = 0; i < info.keys.size(); ++i)
  {
    uint16_t const code = info.keys[i];
    for (size_t h_i = 0; h_i < m_history.size(); ++h_i)
    {
      EvdevState const& state = m_history[h_i];

      int const x = width() * static_cast<int>(h_i + 1) / static_cast<int>(m_history.size());
      int const y = height() * static_cast<int>(i + 1) / static_cast<int>(info.keys.size());

      if (state.get_key_value(code) == 1) {
        painter.fillRect(x - 1,
                         y - 2, 3,
                         5,
                         QColor(255, 0, 0));
      } else if (state.get_key_value(code) == 2) {
        painter.fillRect(x - 1,
                         y - 2, 3,
                         5,
                         QColor(255, 0, 255));
      }
    }
  }

  { // rel events
    for (size_t i = 0; i < info.rels.size(); ++i)
    {
      uint16_t const code = info.rels[i];

      int min = 0;
      int max = 0;
      for (size_t h_i = 0; h_i < m_history.size(); ++h_i)
      {
        EvdevState const& state = m_history[h_i];

        min = std::min(min, state.get_rel_value(code));
        max = std::max(max, state.get_rel_value(code));
      }
      min = -std::min(std::abs(min), std::abs(max));
      max = std::max(std::abs(min), std::abs(max));

      painter.setCompositionMode(QPainter::CompositionMode_Plus);
      painter.setPen(QColor(rainbow[i % rainbow.size()]));

      QPoint last_pos;
      for (size_t h_i = 0; h_i < m_history.size(); ++h_i)
      {
        EvdevState const& state = m_history[h_i];

        int const x = width() * static_cast<int>(h_i + 1) / static_cast<int>(m_history.size());
        int const y = height() * (state.get_rel_value(code) - min) / (max - min + 1);

        if (!last_pos.isNull()) {
          // painter.fillRect(x - 1, y - 1, 3, 3, rainbow[i % rainbow.size()]);
          painter.drawLine(last_pos, QPoint(x, y));
        }
        last_pos = QPoint(x, y);
      }
    }
  }

  for (size_t i = 0; i < info.abss.size(); ++i)
  {
    uint16_t const code = info.abss[i];
    for (size_t h_i = 0; h_i < m_history.size(); ++h_i)
    {
      EvdevState const& state = m_history[h_i];

      int const x = width() * static_cast<int>(h_i + 1) / static_cast<int>(m_history.size());
      int const y = (state.get_abs_value(code) - info.get_absinfo(code).minimum) * height() / info.get_absinfo(code).maximum;
      painter.fillRect(x - 1, y - 1, 3, 3, QColor(255, 0, 0));
    }
  }
}

void
EvgraphWidget::set_state(EvdevState const& state)
{
  m_history.push_back(state);
  update();
}

} // namespace evtest_qt

/* EOF */
