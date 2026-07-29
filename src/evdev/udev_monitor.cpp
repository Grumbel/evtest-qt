// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "udev_monitor.hpp"

#include <cstring>
#include <fnmatch.h>

#include <libudev.h>

namespace evtest_qt {

namespace {

bool is_event_node(char const* devnode)
{
  if (!devnode) {
    return false;
  }
  // Match /dev/input/eventN (same policy as EvdevList::scan)
  char const* base = std::strrchr(devnode, '/');
  base = base ? base + 1 : devnode;
  return fnmatch("event[0-9]", base, 0) == 0
      || fnmatch("event[0-9][0-9]", base, 0) == 0
      || fnmatch("event[0-9][0-9][0-9]", base, 0) == 0
      || fnmatch("event[0-9][0-9][0-9][0-9]", base, 0) == 0;
}

QString device_display_name(struct udev_device* dev)
{
  if (char const* name = udev_device_get_property_value(dev, "NAME")) {
    // udev often quotes NAME with double quotes
    QString n = QString::fromUtf8(name);
    if (n.startsWith('"') && n.endsWith('"') && n.size() >= 2) {
      n = n.mid(1, n.size() - 2);
    }
    if (!n.isEmpty()) {
      return n;
    }
  }
  if (char const* model = udev_device_get_property_value(dev, "ID_MODEL_FROM_DATABASE")) {
    return QString::fromUtf8(model);
  }
  if (char const* model = udev_device_get_property_value(dev, "ID_MODEL")) {
    return QString::fromUtf8(model).replace('_', ' ');
  }
  return QString();
}

} // namespace

UdevInputMonitor::UdevInputMonitor(QObject* parent) :
  QObject(parent),
  m_udev(nullptr),
  m_monitor(nullptr),
  m_notifier()
{
  m_udev = udev_new();
  if (!m_udev) {
    emit monitor_error(QStringLiteral("udev_new() failed"));
    return;
  }

  m_monitor = udev_monitor_new_from_netlink(m_udev, "udev");
  if (!m_monitor) {
    emit monitor_error(QStringLiteral("udev_monitor_new_from_netlink() failed"));
    udev_unref(m_udev);
    m_udev = nullptr;
    return;
  }

  if (udev_monitor_filter_add_match_subsystem_devtype(m_monitor, "input", nullptr) < 0) {
    emit monitor_error(QStringLiteral("udev: failed to add input subsystem filter"));
  }

  if (udev_monitor_enable_receiving(m_monitor) < 0) {
    emit monitor_error(QStringLiteral("udev_monitor_enable_receiving() failed"));
    udev_monitor_unref(m_monitor);
    udev_unref(m_udev);
    m_monitor = nullptr;
    m_udev = nullptr;
    return;
  }

  int fd = udev_monitor_get_fd(m_monitor);
  m_notifier = std::make_unique<QSocketNotifier>(fd, QSocketNotifier::Read);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  connect(m_notifier.get(), &QSocketNotifier::activated,
          this, [this](QSocketDescriptor) { on_udev_readable(0); });
#elif QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
  connect(m_notifier.get(),
          QOverload<QSocketDescriptor>::of(&QSocketNotifier::activated),
          this, [this](QSocketDescriptor) { on_udev_readable(0); });
#else
  connect(m_notifier.get(),
          static_cast<void (QSocketNotifier::*)(int)>(&QSocketNotifier::activated),
          this, SLOT(on_udev_readable(int)));
#endif
}

UdevInputMonitor::~UdevInputMonitor()
{
  m_notifier.reset();
  if (m_monitor) {
    udev_monitor_unref(m_monitor);
    m_monitor = nullptr;
  }
  if (m_udev) {
    udev_unref(m_udev);
    m_udev = nullptr;
  }
}

void
UdevInputMonitor::on_udev_readable(int /*fd*/)
{
  while (m_monitor) {
    struct udev_device* dev = udev_monitor_receive_device(m_monitor);
    if (!dev) {
      break;
    }

    char const* action = udev_device_get_action(dev);
    char const* devnode = udev_device_get_devnode(dev);

    if (action && devnode && is_event_node(devnode)) {
      QString node = QString::fromUtf8(devnode);
      QString name = device_display_name(dev);

      if (std::strcmp(action, "add") == 0) {
        emit device_added(node, name);
      } else if (std::strcmp(action, "remove") == 0) {
        emit device_removed(node, name);
      }
      // ignore change/bind/unbind/etc. for the list; log only add/remove
    }

    udev_device_unref(dev);
  }
}

} // namespace evtest_qt

/* EOF */
