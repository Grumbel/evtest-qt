// evtest-qt - A graphical joystick tester
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_EVTEST_QT_EVENT_GRAPH_WIDGET_HPP
#define HEADER_EVTEST_QT_EVENT_GRAPH_WIDGET_HPP

#include <QWidget>
#include <QElapsedTimer>

#include <cstdint>
#include <deque>
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

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override { return QSize(320, 160); }

  /** Reset traces for a new device (or clear when info has no channels). */
  void set_device(EvdevInfo const& info);
  void clear_device();

  /** Append one complete sample (call after EvdevState reports a sync). */
  void push_state(EvdevState const& state);

  void set_time_window_ms(int ms);
  int time_window_ms() const { return m_window_ms; }

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
    std::vector<int32_t> abs_values; // parallel to m_abs_channels
    std::vector<int32_t> key_values; // parallel to m_key_codes
  };

  void trim_old_samples(qint64 now_ms);
  int lane_count() const;
  int content_height() const;

  EvdevInfo m_info;
  std::vector<AbsChannel> m_abs_channels;
  std::vector<uint16_t> m_key_codes;
  std::deque<Sample> m_samples;

  QElapsedTimer m_clock;
  int m_window_ms;
  bool m_has_device;
};

} // namespace evtest_qt

#endif

/* EOF */
