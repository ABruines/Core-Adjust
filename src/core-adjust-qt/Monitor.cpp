/*
 * This file is part of 'Core Adjust'.
 *
 * Core Adjust - Adjust various settings of Intel Processors.
 * Copyright (C) 2020, Alexander Bruines <alexander.bruines@gmail.com>
 *
 * Core Adjust is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Core Adjust is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Core Adjust. If not, see <https://www.gnu.org/licenses/>.
 */

#include <cmath>
#include <sstream>
#include <iomanip>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QFrame>
#include <QString>
#include <QTimer>
#include "Dbg.hpp"
#include "Monitor.hpp"

/*
 * Monitor::CpuActivity
 */

Monitor::CpuActivity::CpuActivity(QWidget* parent)
  : QWidget(parent) {
  auto* wrapper = new QVBoxLayout(this);
  auto* group_box = new QGroupBox();
  auto* layout = new QVBoxLayout();
  gauge_ = new Gauge(nullptr, 250, 250, 230., true);
  gauge_->setLabel("System load (%)");
  gauge_->setMinimumSize(gauge_->sizeHint());
  gauge_->resize(gauge_->sizeHint());
  layout->addWidget(gauge_);
  layout->setMargin(0);
  group_box->setLayout(layout);
  group_box->setFlat(true);
  wrapper->addWidget(group_box);
  wrapper->setMargin(0);
  previous_value_ = 0;
}

void Monitor::CpuActivity::refresh(const xxx::CpuActivity& cpu_activity) {
  auto total = cpu_activity[0].total;
  auto delta = (total > previous_value_) ?
      (total - previous_value_) : (previous_value_ - total);
  //delta = delta + delta + delta;
  gauge_->setValueAnimated(total, 150 + delta);
  previous_value_ = total;
}

/*
 * Monitor::CpuTemperature
 */

Monitor::CpuTemperature::CpuTemperature(
  const xxx::CpuTemperature& temp,
  QWidget *parent)
  : QWidget(parent) {
  auto* wrapper = new QVBoxLayout(this);
  auto* group_box = new QGroupBox();
  auto* layout = new QVBoxLayout();
  for (const auto& entry : temp) {
    auto* box = new QHBoxLayout();
    auto* name = new QLabel(std::move(QString::fromStdString(entry.label)));
    v_.push_back(std::move(new QLabel(std::move(QString::number(entry.value)))));
    auto* degrees = new QLabel("°C   ");
    box->addWidget(name);
    box->addStretch(1);
    box->addWidget(v_.back());
    box->addWidget(degrees);
    layout->addLayout(box);
  }
  group_box->setLayout(layout);
  group_box->setFlat(true);
  wrapper->addWidget(group_box);
  wrapper->setMargin(0);
}


void Monitor::CpuTemperature::refresh(const xxx::CpuTemperature& temp) {
  auto itemp = temp.begin();
  auto iv = v_.begin();
  for(; itemp != temp.end() && iv != v_.end(); ++itemp, ++iv) {
    (*iv)->setText(QString::number(itemp->value));
  }
}

/*
 * Monitor::CpuPower
 */

Monitor::CpuPower::CpuPower(
  const xxx::PowerCap::IntelRAPL& cpu_power,
  QWidget *parent)
  : QWidget(parent) {
  auto* wrapper = new QVBoxLayout(this);
  auto* group_box = new QGroupBox();
  auto* layout = new QVBoxLayout();
  for (const auto& power_zone : cpu_power) {
    Entry e;
    auto* box = new QHBoxLayout();
    auto* name = new QLabel(std::move(QString::fromStdString(power_zone.name())));
    auto* value = new QLabel("0");
    auto* watt = new QLabel("Watt");
    box->addWidget(name);
    box->addStretch(1);
    box->addWidget(value, 0, Qt::AlignRight);
    box->addWidget(watt);
    layout->addLayout(box);
    e.value = value;
    for (const auto& sub_zone : power_zone) {
      auto* box = new QHBoxLayout();
      auto* name = new QLabel(std::move(QString::fromStdString(sub_zone.name())));
      auto* value = new QLabel("0");
      auto* watt = new QLabel("Watt");
      box->addSpacing(8);
      box->addWidget(name, 0, Qt::AlignRight);
      box->addStretch(1);
      box->addWidget(value, 0, Qt::AlignRight);
      box->addWidget(watt);
      layout->addLayout(box);
      e.sub_zones.push_back(value);
    }
    v_.push_back(std::move(e));
  }
  group_box->setLayout(layout);
  group_box->setFlat(true);
  wrapper->addWidget(group_box);
  wrapper->setMargin(0);
}


void Monitor::CpuPower::refresh(const xxx::PowerCap::IntelRAPL& cpu_power) {
  /* loop over the power zones */
  auto icpu = cpu_power.begin();
  auto iv = v_.begin();
  for(; icpu != cpu_power.end() && iv != v_.end(); ++icpu, ++iv) {
    std::stringstream ss;
    ss << std::setw(7) << std::setprecision(3) << std::fixed << std::noshowpos
       << icpu->average_power();
    (*iv).value->setText(QString::fromStdString(ss.str()));
    /* loop over the sub zones for this power zone */
    auto icpusub = (*icpu).begin();
    auto ivsub = (*iv).sub_zones.begin();
    for (; icpusub != (*icpu).end() && ivsub != (*iv).sub_zones.end();
        ++icpusub, ++ivsub) {
      std::stringstream ss;
      ss << std::setw(7) << std::setprecision(3) << std::fixed
         << std::noshowpos << icpusub->average_power();
      (*ivsub)->setText(QString::fromStdString(ss.str()));
    }
  }
}

/*
 * Monitor::CpuFrequency
 */

Monitor::CpuFrequency::CpuFrequency(
  const xxx::CpuFrequency& cpu_frequency, QWidget* parent)
  : QWidget(parent) {
  auto* wrapper = new QVBoxLayout(this);
  auto* group_box = new QGroupBox();
  auto* layout = new QVBoxLayout();
  size_t i = 0;
  for (auto freq : cpu_frequency) {
    std::stringstream ss;
    ss << "cpu" << cpu_frequency.logical(i);
    ++i;
    auto* box = new QHBoxLayout();
    auto* cpu_label = new QLabel(std::move(QString::fromStdString(ss.str())));
    vload_.push_back(std::move(new QLabel(std::move(QString::number(0)))));
    vload_.back()->setMinimumSize(40,0);
    vload_.back()->setAlignment(Qt::AlignRight);
    auto* load_label = new QLabel("%");
    vfreq_.push_back(std::move(new QLabel(std::move(QString::number(freq)))));
    vfreq_.back()->setMinimumSize(40,0);
    vfreq_.back()->setAlignment(Qt::AlignRight);
    auto* freq_label = new QLabel("MHz");
    box->addWidget(cpu_label);
    box->addStretch(1);
    box->addWidget(vload_.back());
    box->addWidget(load_label, 0);
    box->addStretch(1);
    box->addWidget(vfreq_.back(), 0, Qt::AlignRight);
    box->addWidget(freq_label, 0);
    layout->addLayout(box, 0);
  }
  group_box->setLayout(layout);
  group_box->setFlat(true);
  wrapper->addWidget(group_box);
  wrapper->setMargin(0);
}


void Monitor::CpuFrequency::refresh(const xxx::CpuFrequency& cpu_frequency, const xxx::CpuActivity& cpu_activity) {
  auto iv = vfreq_.begin();
  auto il = vload_.begin();
  auto ifreq = cpu_frequency.begin();
  auto iload = cpu_activity.begin();
  ++iload;
  for (;iv != vfreq_.end() && ifreq != cpu_frequency.end() &&
      il != vload_.end() && iload != cpu_activity.end();
      ++iv, ++ifreq, ++il, ++iload) {
    (*il)->setText(std::move(QString::number(iload->total)));
    (*iv)->setText(std::move(QString::number(*ifreq)));
  }
}

/*
 * Monitor
 */

Monitor::Monitor(QWidget* parent)
  : QWidget(parent) {
  /* update the sensors to ensure valid values */
  sensors_.update();
  /* create the layouts/widgets */
  auto* layout = new QVBoxLayout(this);
  auto* widget = new QFrame();
  auto* scroll_area = new QScrollArea();
  auto* box = new QVBoxLayout();
  cpu_activity_ = new CpuActivity();
  cpu_power_ = new CpuPower(sensors_.cpu_power());
  cpu_temp_ = new CpuTemperature(sensors_.cpu_temperature());
  cpu_frequency_ = new CpuFrequency(sensors_.cpu_frequency());
  /* assemble the layouts/widgets */
  box->addWidget(cpu_power_);
  box->addWidget(cpu_temp_);
  box->addWidget(cpu_frequency_);
  box->addStretch(1);
  widget->setLayout(box);
  scroll_area->setWidget(widget);
  scroll_area->setWidgetResizable(true);
  layout->addWidget(cpu_activity_);
  layout->addWidget(scroll_area, 1);
  /* setup a timer slot that is called every 500ms */
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timerCallback()));
  timer->start(500);
}

void Monitor::timerCallback() {
  /* update all sensors and widgets */
  sensors_.update();
  cpu_activity_->refresh(sensors_.cpu_activity());
  cpu_temp_->refresh(sensors_.cpu_temperature());
  cpu_power_->refresh(sensors_.cpu_power());
  cpu_frequency_->refresh(sensors_.cpu_frequency(), sensors_.cpu_activity());
}

/*
 * MonitorTab
 */

MonitorTab::MonitorTab(QWidget* parent)
  : TabMemberWidget(parent) {
  grid_ = new QGridLayout(this);
}

void MonitorTab::timed(bool is_active_tab) {
  /* Set a new value for each gauge. */
  if (monitor_ && is_active_tab) {
    auto ip = previous_values_.begin();
    auto it = monitor_->sensors_.cpu_activity().begin() + 1;
    auto ig = gauges_.begin();
    for (;it != monitor_->sensors_.cpu_activity().end() &&
        ig != gauges_.end(); ++ip, ++it, ++ig) {
      auto delta = (it->total > *ip) ? (it->total - *ip) : (*ip - it->total);
      //delta = delta + delta + delta;
      (*ig)->setValueAnimated(it->total, 150 + delta);
      *ip = it->total;
    }
  }
}

void MonitorTab::setMonitor(Monitor* m) {
  /* delete all gauge widgets */
  for (auto* g : gauges_) {
    grid_->removeWidget(g);
    g->setParent(nullptr);
    delete g;
  }
  gauges_.clear();
  previous_values_.clear();
  monitor_ = m;
  if (monitor_) {
    /* Add as many gauges as there are logical cpus */
    int width = 3;
    if ((monitor_->sensors_.cpu_activity().size() - 1) <= 4) width = 2;
    if ((monitor_->sensors_.cpu_activity().size() - 1) > 8) width = 4;
    int grid_x = 0;
    int grid_y = 0;
    for (size_t i = 0; i < monitor_->sensors_.cpu_activity().size() - 1; ++i) {
      auto* g = new Gauge(nullptr, 100, 100, 230., true);
      /* the cpu nr to display */      
      size_t c = monitor_->sensors_.cpu_frequency().logical(i);
      /* if no logical id could be fetched then use the count of the CpuActivity instead */
      if (c == ULONG_MAX) c = i;
      g->setLabel(QString("cpu%1 load (%)").arg(c));
      grid_->addWidget(g, grid_y, grid_x);
      gauges_.push_back(g);
      previous_values_.push_back(0);
      ++grid_x;
      if (grid_x == width) {
        grid_x = 0;
        ++grid_y;
      }
    }
  }
}

