// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_EVENT_GRAPH_WIDGET_HPP
#define HEADER_EVTEST_QT_EVENT_GRAPH_WIDGET_HPP

#include <QWidget>
#include <QElapsedTimer>

#include <cstdint>
#include <deque>
#include <unordered_set>
#include <vector>

#include "evdev/evdev_info.hpp"
#include "fwd.hpp"

namespace evtest_qt {

/** Multi-channel oscilloscope-style plot of axis and button values over time. */
class EventGraphWidget : public QWidget
{
  Q_OBJECT

public:
  explicit EventGraphWidget(QWidget* parent = nullptr);

  QSize sizeHint() const override { return QSize(640, 360); }
  QSize minimumSizeHint() const override { return QSize(320, 120); }

  void set_device(EvdevInfo const& info);
  void clear_device();
  void push_state(EvdevState const& state);

  void set_time_window_ms(int ms);
  int time_window_ms() const { return m_window_ms; }

  void set_paused(bool paused);
  bool is_paused() const { return m_paused; }

  void clear_history();

  void set_only_used_buttons(bool enabled);
  bool only_used_buttons() const { return m_only_used_buttons; }

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  struct AbsChannel {
    uint16_t code = 0;
    int32_t minimum = 0;
    int32_t maximum = 0;
  };

  struct Sample {
    qint64 t_ms = 0;
    std::vector<int32_t> abs_values;
    std::vector<int32_t> key_values;
  };

  void trim_old_samples(qint64 now_ms);
  qint64 display_now_ms() const;
  std::vector<size_t> visible_key_indices() const;

  EvdevInfo m_info;
  std::vector<AbsChannel> m_abs_channels;
  std::vector<uint16_t> m_key_codes;
  std::deque<Sample> m_samples;
  std::unordered_set<uint16_t> m_keys_seen_pressed;

  QElapsedTimer m_clock;
  int m_window_ms;
  bool m_has_device;
  bool m_paused;
  qint64 m_pause_now_ms;
  bool m_only_used_buttons;
};

} // namespace evtest_qt

#endif

/* EOF */
