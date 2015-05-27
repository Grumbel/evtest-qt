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

#ifndef HEADER_MULTITOUCH_WIDGET_HPP
#define HEADER_MULTITOUCH_WIDGET_HPP

#include <QWidget>

class EvdevState;

class MultitouchWidget : public QWidget
{
  Q_OBJECT

private:
  int m_max_x;
  int m_max_y;
  std::vector<int> m_mt_x;
  std::vector<int> m_mt_y;

public:
  MultitouchWidget(QWidget* parent_=0);
  virtual ~MultitouchWidget();

  QSize sizeHint() const  override { return QSize(400, 225); };

public slots:
  void on_change(const EvdevState& state);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  MultitouchWidget(const MultitouchWidget&) = delete;
  MultitouchWidget& operator=(const MultitouchWidget&) = delete;
};

#endif

/* EOF */
