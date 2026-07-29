// evtest-qt - A graphical joystick tester
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "event_graph_widget.hpp"

#include <QPainter>
#include <QPaintEvent>

#include <algorithm>
#include <cmath>

#include "evdev/evdev_enum.hpp"
#include "evdev/evdev_state.hpp"

namespace evtest_qt {

namespace {

constexpr int kLabelWidth = 88;
constexpr int kAbsLaneHeight = 36;
constexpr int kKeyLaneHeight = 10;
constexpr int kPadding = 4;

QColor channel_color(size_t index)
{
  // Distinct hues cycling around the wheel
  static int const hues[] = { 210, 0, 120, 30, 280, 180, 60, 330, 150, 240 };
  int h = hues[index % (sizeof(hues) / sizeof(hues[0]))];
  return QColor::fromHsv(h, 200, 220);
}

} // namespace

EventGraphWidget::EventGraphWidget(QWidget* parent) :
  QWidget(parent),
  m_info(),
  m_abs_channels(),
  m_key_codes(),
  m_samples(),
  m_clock(),
  m_window_ms(8000),
  m_has_device(false)
{
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
  m_clock.start();
}

QSize
EventGraphWidget::sizeHint() const
{
  return QSize(640, content_height());
}

void
EventGraphWidget::set_time_window_ms(int ms)
{
  m_window_ms = std::max(500, ms);
  update();
}

void
EventGraphWidget::clear_device()
{
  m_has_device = false;
  m_info = EvdevInfo();
  m_abs_channels.clear();
  m_key_codes.clear();
  m_samples.clear();
  updateGeometry();
  update();
}

void
EventGraphWidget::set_device(EvdevInfo const& info)
{
  m_info = info;
  m_abs_channels.clear();
  m_key_codes.clear();
  m_samples.clear();
  m_has_device = true;
  m_clock.restart();

  for (uint16_t code : info.abss) {
    AbsInfo ai = info.get_absinfo(code);
    AbsChannel ch;
    ch.code = code;
    ch.minimum = ai.minimum;
    ch.maximum = ai.maximum;
    m_abs_channels.push_back(ch);
  }

  for (uint16_t code : info.keys) {
    m_key_codes.push_back(code);
  }

  updateGeometry();
  update();
}

void
EventGraphWidget::push_state(EvdevState const& state)
{
  if (!m_has_device) {
    return;
  }

  Sample s;
  s.t_ms = m_clock.elapsed();
  s.abs_values.reserve(m_abs_channels.size());
  for (AbsChannel const& ch : m_abs_channels) {
    s.abs_values.push_back(state.get_abs_value(ch.code));
  }
  s.key_values.reserve(m_key_codes.size());
  for (uint16_t code : m_key_codes) {
    s.key_values.push_back(state.get_key_value(code));
  }

  m_samples.push_back(std::move(s));
  trim_old_samples(m_clock.elapsed());
  update();
}

void
EventGraphWidget::trim_old_samples(qint64 now_ms)
{
  qint64 cutoff = now_ms - m_window_ms;
  while (!m_samples.empty() && m_samples.front().t_ms < cutoff) {
    m_samples.pop_front();
  }
}

int
EventGraphWidget::lane_count() const
{
  return static_cast<int>(m_abs_channels.size() + m_key_codes.size());
}

int
EventGraphWidget::content_height() const
{
  if (!m_has_device || lane_count() == 0) {
    return minimumSizeHint().height();
  }
  int h = kPadding * 2;
  h += static_cast<int>(m_abs_channels.size()) * kAbsLaneHeight;
  h += static_cast<int>(m_key_codes.size()) * kKeyLaneHeight;
  return std::max(h, minimumSizeHint().height());
}

void
EventGraphWidget::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, false);

  painter.fillRect(rect(), palette().color(QPalette::Base));

  if (!m_has_device) {
    painter.setPen(palette().color(QPalette::Text));
    painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("No device selected"));
    return;
  }

  if (lane_count() == 0) {
    painter.setPen(palette().color(QPalette::Text));
    painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("Device has no axes or buttons"));
    return;
  }

  qint64 now = m_clock.elapsed();
  trim_old_samples(now);
  qint64 t0 = now - m_window_ms;

  int const plot_left = kLabelWidth;
  int const plot_right = width() - kPadding;
  int const plot_width = std::max(1, plot_right - plot_left);

  auto x_for_t = [&](qint64 t) -> int {
    double u = static_cast<double>(t - t0) / static_cast<double>(m_window_ms);
    u = std::clamp(u, 0.0, 1.0);
    return plot_left + static_cast<int>(u * plot_width);
  };

  // Vertical grid every 1s
  painter.setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));
  for (int sec = 0; sec <= m_window_ms / 1000; ++sec) {
    qint64 t = t0 + sec * 1000;
    if (t < t0) continue;
    int x = x_for_t(t);
    painter.drawLine(x, 0, x, height());
  }

  int y = kPadding;

  // Axis lanes
  for (size_t ci = 0; ci < m_abs_channels.size(); ++ci) {
    AbsChannel const& ch = m_abs_channels[ci];
    QRect lane(0, y, width(), kAbsLaneHeight);
    painter.fillRect(lane.adjusted(0, 0, 0, -1),
                     (ci % 2 == 0) ? QColor(250, 250, 250) : QColor(245, 245, 248));

    painter.setPen(palette().color(QPalette::Text));
    QString label = QString::fromStdString(evdev_abs_name(ch.code));
    painter.drawText(QRect(kPadding, y, kLabelWidth - 2 * kPadding, kAbsLaneHeight),
                     Qt::AlignVCenter | Qt::AlignLeft, label);

    int y_mid = y + kAbsLaneHeight / 2;
    painter.setPen(QPen(QColor(220, 220, 220), 1));
    painter.drawLine(plot_left, y_mid, plot_right, y_mid);

    int range = ch.maximum - ch.minimum;
    if (range == 0) {
      range = 1;
    }

    QColor col = channel_color(ci);
    painter.setPen(QPen(col, 1));

    QPoint prev;
    bool has_prev = false;
    for (Sample const& s : m_samples) {
      int x = x_for_t(s.t_ms);
      double n = static_cast<double>(s.abs_values[ci] - ch.minimum) / static_cast<double>(range);
      n = std::clamp(n, 0.0, 1.0);
      int py = y + kAbsLaneHeight - 2 - static_cast<int>(n * (kAbsLaneHeight - 4));
      QPoint pt(x, py);
      if (has_prev) {
        painter.drawLine(prev, pt);
      }
      prev = pt;
      has_prev = true;
    }

    // Current value on the right
    if (!m_samples.empty()) {
      int32_t v = m_samples.back().abs_values[ci];
      painter.setPen(col);
      painter.drawText(QRect(plot_right - 64, y, 60, kAbsLaneHeight),
                       Qt::AlignVCenter | Qt::AlignRight, QString::number(v));
    }

    y += kAbsLaneHeight;
  }

  // Button lanes (digital)
  for (size_t ki = 0; ki < m_key_codes.size(); ++ki) {
    QRect lane(0, y, width(), kKeyLaneHeight);
    painter.fillRect(lane, (ki % 2 == 0) ? QColor(252, 252, 252) : QColor(248, 248, 250));

    painter.setPen(QColor(100, 100, 100));
    QFont small = painter.font();
    small.setPointSizeF(std::max(7.0, small.pointSizeF() - 2.0));
    painter.setFont(small);
    QString label = QString::fromStdString(evdev_key_name(m_key_codes[ki]));
    painter.drawText(QRect(kPadding, y, kLabelWidth - 2 * kPadding, kKeyLaneHeight),
                     Qt::AlignVCenter | Qt::AlignLeft, label);

    // Step plot
    painter.setPen(QPen(QColor(200, 40, 40), 1));
    QPoint prev;
    bool has_prev = false;
    int y_low = y + kKeyLaneHeight - 2;
    int y_high = y + 2;

    for (Sample const& s : m_samples) {
      int x = x_for_t(s.t_ms);
      int py = (s.key_values[ki] != 0) ? y_high : y_low;
      QPoint pt(x, py);
      if (has_prev) {
        // horizontal then vertical for step appearance
        painter.drawLine(prev, QPoint(x, prev.y()));
        painter.drawLine(QPoint(x, prev.y()), pt);
      }
      prev = pt;
      has_prev = true;
    }

    y += kKeyLaneHeight;
  }

  // Time scale label
  painter.setPen(QColor(120, 120, 120));
  painter.drawText(QRect(plot_left, height() - 14, plot_width, 12),
                   Qt::AlignLeft | Qt::AlignVCenter,
                   QStringLiteral("−%1 s").arg(m_window_ms / 1000));
  painter.drawText(QRect(plot_left, height() - 14, plot_width, 12),
                   Qt::AlignRight | Qt::AlignVCenter,
                   QStringLiteral("now"));

  // Border
  painter.setPen(QColor(0, 0, 0));
  painter.drawRect(0, 0, width() - 1, height() - 1);
}

} // namespace evtest_qt

/* EOF */
