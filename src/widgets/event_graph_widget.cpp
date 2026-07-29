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

constexpr int kLabelWidth = 96;
constexpr int kFooterHeight = 18;
constexpr int kPadding = 4;

QColor channel_color(size_t index)
{
  static int const hues[] = { 210, 0, 120, 30, 280, 180, 60, 330, 150, 240 };
  int h = hues[index % (sizeof(hues) / sizeof(hues[0]))];
  return QColor::fromHsv(h, 200, 220);
}

QColor key_value_color(int32_t value)
{
  switch (value) {
    case 0:  return QColor(0, 0, 0, 0);
    case 1:  return QColor(220, 50, 50, 200);   // pressed
    case 2:  return QColor(200, 40, 200, 200);  // repeat
    default: return QColor(220, 200, 40, 200);  // other
  }
}

} // namespace

EventGraphWidget::EventGraphWidget(QWidget* parent) :
  QWidget(parent),
  m_info(),
  m_abs_channels(),
  m_key_codes(),
  m_samples(),
  m_keys_seen_pressed(),
  m_clock(),
  m_window_ms(8000),
  m_has_device(false),
  m_paused(false),
  m_pause_now_ms(0),
  m_only_used_buttons(false)
{
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_clock.start();
}

void
EventGraphWidget::set_time_window_ms(int ms)
{
  m_window_ms = std::max(500, ms);
  trim_old_samples(display_now_ms());
  update();
}

void
EventGraphWidget::set_paused(bool paused)
{
  if (paused == m_paused) {
    return;
  }
  if (paused) {
    m_pause_now_ms = m_clock.elapsed();
  }
  m_paused = paused;
  update();
}

void
EventGraphWidget::clear_history()
{
  m_samples.clear();
  m_keys_seen_pressed.clear();
  update();
}

void
EventGraphWidget::set_only_used_buttons(bool enabled)
{
  m_only_used_buttons = enabled;
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
  m_keys_seen_pressed.clear();
  update();
}

void
EventGraphWidget::set_device(EvdevInfo const& info)
{
  m_info = info;
  m_abs_channels.clear();
  m_key_codes.clear();
  m_samples.clear();
  m_keys_seen_pressed.clear();
  m_has_device = true;
  m_clock.restart();
  m_paused = false;

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

  update();
}

qint64
EventGraphWidget::display_now_ms() const
{
  return m_paused ? m_pause_now_ms : m_clock.elapsed();
}

void
EventGraphWidget::push_state(EvdevState const& state)
{
  if (!m_has_device || m_paused) {
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
    int32_t v = state.get_key_value(code);
    s.key_values.push_back(v);
    if (v != 0) {
      m_keys_seen_pressed.insert(code);
    }
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

std::vector<size_t>
EventGraphWidget::visible_key_indices() const
{
  std::vector<size_t> out;
  out.reserve(m_key_codes.size());
  for (size_t i = 0; i < m_key_codes.size(); ++i) {
    if (!m_only_used_buttons || m_keys_seen_pressed.count(m_key_codes[i]) > 0) {
      out.push_back(i);
    }
  }
  return out;
}

void
EventGraphWidget::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  painter.fillRect(rect(), palette().color(QPalette::Base));

  if (!m_has_device) {
    painter.setPen(palette().color(QPalette::Text));
    painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("No device selected"));
    return;
  }

  auto visible_keys = visible_key_indices();
  size_t const n_abs = m_abs_channels.size();
  size_t const n_key = visible_keys.size();

  if (n_abs + n_key == 0) {
    painter.setPen(palette().color(QPalette::Text));
    painter.drawText(rect(), Qt::AlignCenter,
                     m_only_used_buttons
                       ? QStringLiteral("No axes; no buttons used yet")
                       : QStringLiteral("Device has no axes or buttons"));
    return;
  }

  qint64 now = display_now_ms();
  trim_old_samples(now);
  qint64 t0 = now - m_window_ms;

  int const plot_left = kLabelWidth;
  int const plot_right = width() - kPadding;
  int const plot_width = std::max(1, plot_right - plot_left);
  int const plot_top = kPadding;
  int const plot_bottom = height() - kFooterHeight;
  int const plot_height = std::max(1, plot_bottom - plot_top);

  // Weighted layout: axes get more vertical space than buttons
  int const abs_weight = 4;
  int const key_weight = 1;
  int total_weight = static_cast<int>(n_abs) * abs_weight
                   + static_cast<int>(n_key) * key_weight;
  if (total_weight < 1) {
    total_weight = 1;
  }

  auto x_for_t = [&](qint64 t) -> int {
    double u = static_cast<double>(t - t0) / static_cast<double>(m_window_ms);
    u = std::clamp(u, 0.0, 1.0);
    return plot_left + static_cast<int>(u * plot_width);
  };

  // Time grid
  painter.setPen(QPen(QColor(210, 210, 210), 1, Qt::DotLine));
  int step_ms = 1000;
  if (m_window_ms <= 2000) {
    step_ms = 250;
  } else if (m_window_ms <= 5000) {
    step_ms = 500;
  } else if (m_window_ms >= 20000) {
    step_ms = 2000;
  }
  for (int t = 0; t <= m_window_ms; t += step_ms) {
    int x = x_for_t(t0 + t);
    painter.drawLine(x, plot_top, x, plot_bottom);
  }

  int y = plot_top;

  auto lane_height = [&](int weight) -> int {
    return std::max(1, plot_height * weight / total_weight);
  };

  // --- Axes ---
  for (size_t ci = 0; ci < n_abs; ++ci) {
    int lh = lane_height(abs_weight);
    // last lane absorbs remainder
    if (ci == n_abs - 1 && n_key == 0) {
      lh = plot_bottom - y;
    }
    lh = std::max(lh, 1);

    AbsChannel const& ch = m_abs_channels[ci];
    QRect lane(0, y, width(), lh);
    painter.fillRect(lane.adjusted(0, 0, 0, 0),
                     (ci % 2 == 0) ? QColor(250, 250, 252) : QColor(244, 244, 248));

    painter.setPen(palette().color(QPalette::Text));
    QFont f = painter.font();
    f.setPointSizeF(std::max(7.0, std::min(10.0, lh / 3.0)));
    painter.setFont(f);
    painter.drawText(QRect(kPadding, y, kLabelWidth - 2 * kPadding, lh),
                     Qt::AlignVCenter | Qt::AlignLeft,
                     QString::fromStdString(evdev_abs_name(ch.code)));

    int y_mid = y + lh / 2;
    painter.setPen(QPen(QColor(220, 220, 220), 1));
    painter.drawLine(plot_left, y_mid, plot_right, y_mid);

    int range = ch.maximum - ch.minimum;
    if (range == 0) {
      range = 1;
    }

    QColor col = channel_color(ci);
    painter.setPen(QPen(col, 1));
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPoint prev;
    bool has_prev = false;
    for (Sample const& s : m_samples) {
      int x = x_for_t(s.t_ms);
      double n = static_cast<double>(s.abs_values[ci] - ch.minimum) / static_cast<double>(range);
      n = std::clamp(n, 0.0, 1.0);
      int py = y + lh - 2 - static_cast<int>(n * std::max(1, lh - 4));
      QPoint pt(x, py);
      if (has_prev) {
        painter.drawLine(prev, pt);
      }
      prev = pt;
      has_prev = true;
    }
    painter.setRenderHint(QPainter::Antialiasing, false);

    if (!m_samples.empty()) {
      painter.setPen(col);
      painter.drawText(QRect(plot_right - 56, y, 52, lh),
                       Qt::AlignVCenter | Qt::AlignRight,
                       QString::number(m_samples.back().abs_values[ci]));
    }

    y += lh;
  }

  // --- Buttons as filled rectangles ---
  for (size_t vk = 0; vk < n_key; ++vk) {
    size_t ki = visible_keys[vk];
    int lh = lane_height(key_weight);
    if (vk == n_key - 1) {
      lh = plot_bottom - y;
    }
    lh = std::max(lh, 1);

    painter.fillRect(0, y, width(), lh,
                     (vk % 2 == 0) ? QColor(252, 252, 252) : QColor(248, 248, 250));

    QFont f = painter.font();
    f.setPointSizeF(std::max(6.0, std::min(9.0, lh * 0.7)));
    painter.setFont(f);
    painter.setPen(QColor(90, 90, 90));
    painter.drawText(QRect(kPadding, y, kLabelWidth - 2 * kPadding, lh),
                     Qt::AlignVCenter | Qt::AlignLeft,
                     QString::fromStdString(evdev_key_name(m_key_codes[ki])));

    // Walk samples and draw filled runs of non-zero values
    if (!m_samples.empty()) {
      size_t i = 0;
      while (i < m_samples.size()) {
        int32_t v = m_samples[i].key_values[ki];
        if (v == 0) {
          ++i;
          continue;
        }
        size_t j = i + 1;
        while (j < m_samples.size() && m_samples[j].key_values[ki] == v) {
          ++j;
        }
        // Segment [i, j) has constant value v; extend to next sample or now
        qint64 t_start = m_samples[i].t_ms;
        qint64 t_end = (j < m_samples.size()) ? m_samples[j].t_ms : now;
        int x0 = x_for_t(t_start);
        int x1 = x_for_t(t_end);
        if (x1 <= x0) {
          x1 = x0 + 1;
        }
        QColor fill = key_value_color(v);
        painter.fillRect(x0, y + 1, x1 - x0, std::max(1, lh - 2), fill);
        i = j;
      }
    }

    y += lh;
  }

  // Footer: time scale
  painter.setPen(QColor(110, 110, 110));
  QFont ft = painter.font();
  ft.setPointSizeF(8);
  painter.setFont(ft);
  double secs = m_window_ms / 1000.0;
  painter.drawText(QRect(plot_left, height() - kFooterHeight, plot_width, kFooterHeight),
                   Qt::AlignLeft | Qt::AlignVCenter,
                   QStringLiteral("−%1 s").arg(secs, 0, 'g', 3));
  painter.drawText(QRect(plot_left, height() - kFooterHeight, plot_width, kFooterHeight),
                   Qt::AlignRight | Qt::AlignVCenter,
                   m_paused ? QStringLiteral("paused") : QStringLiteral("now"));

  painter.setPen(QColor(0, 0, 0));
  painter.drawRect(0, 0, width() - 1, height() - 1);
}

} // namespace evtest_qt

/* EOF */
