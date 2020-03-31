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

#ifndef CoreAdjust_ThermalStatus
#define CoreAdjust_ThermalStatus

// STL
#include <sstream>
#include <vector>
// Qt
#include <QLabel>
#include <QTabWidget>
#include <QPushButton>
#include <QScrollArea>
// App
#include "CpuNumber.hpp"
#include "TabMemberBase.hpp"

class ThermalStatusPackage final : public QWidget {
  Q_OBJECT
  public:
    explicit ThermalStatusPackage(
        const SingleCpuInfo&,
        const SingleCpuId&,
        const TabMemberValues&,
        QWidget *parent = nullptr);

    ~ThermalStatusPackage() override = default;

    QSize	sizeHint() const override;
    void timed(bool);

  private:
    const SingleCpuInfo& cpuInfo_;
    const SingleCpuId& cpuId_;
    const TabMemberValues& tabValues_;
    bool thermalStatus_reset_ { false };
    bool prochotEvent_reset_ { false };
    bool criticalTemp_reset_ { false };
    bool thermalThreshold1_reset_ { false };
    bool thermalThreshold2_reset_ { false };
    bool powerLimitation_reset_ { false };
    QLabel* thermalStatus_value_;
    QLabel* thermalStatusLog_value_;
    QLabel* prochotEvent_value_;
    QLabel* prochotEventLog_value_;
    QLabel* criticalTemp_value_;
    QLabel* criticalTempLog_value_;
    QLabel* thermalThreshold1_value_;
    QLabel* thermalThreshold1Log_value_;
    QLabel* thermalThreshold2_value_;
    QLabel* thermalThreshold2Log_value_;
    QLabel* powerLimitation_value_;
    QLabel* powerLimitationLog_value_;
    QLabel* digitalReadout_value_;
    QPushButton* thermalStatus_button_;
    QPushButton* prochotEvent_button_;
    QPushButton* criticalTemp_button_;
    QPushButton* thermalThreshold1_button_;
    QPushButton* thermalThreshold2_button_;
    QPushButton* powerLimitation_button_;
    QScrollArea* scroll_area_;
    QWidget* scroll_widget_;

  private slots:
    void thermalStatus_reset();
    void prochotEvent_reset();
    void criticalTemp_reset();
    void thermalThreshold1_reset();
    void thermalThreshold2_reset();
    void powerLimitation_reset();
};

class ThermalStatusCore final : public QWidget {
  Q_OBJECT
  public:
    explicit ThermalStatusCore(
        const SingleCpuInfo&,
        const SingleCpuId&,
        const TabMemberValues&,
        CpuCoreNr core_nr,
        QWidget *parent = nullptr);

    ~ThermalStatusCore() override = default;

    QSize	sizeHint() const override;
    void timed(bool);

  private:
    const SingleCpuInfo& cpuInfo_;
    const SingleCpuId& cpuId_;
    const TabMemberValues& tabValues_;
    LogicalCpuNr cpu_;
    bool thermalStatus_reset_ { false };
    bool prochotEvent_reset_ { false };
    bool criticalTemp_reset_ { false };
    bool thermalThreshold1_reset_ { false };
    bool thermalThreshold2_reset_ { false };
    bool powerLimitation_reset_ { false };
    bool currentLimitation_reset_ { false };
    bool domainLimitation_reset_ { false };
    QLabel* thermalStatus_value_;
    QLabel* thermalStatusLog_value_;
    QLabel* prochotEvent_value_;
    QLabel* prochotEventLog_value_;
    QLabel* criticalTemp_value_;
    QLabel* criticalTempLog_value_;
    QLabel* thermalThreshold1_value_;
    QLabel* thermalThreshold1Log_value_;
    QLabel* thermalThreshold2_value_;
    QLabel* thermalThreshold2Log_value_;
    QLabel* powerLimitation_value_;
    QLabel* powerLimitationLog_value_;
    QLabel* currentLimitation_value_;
    QLabel* currentLimitationLog_value_;
    QLabel* domainLimitation_value_;
    QLabel* domainLimitationLog_value_;
    QLabel* digitalReadout_value_;
    QLabel* resolution_value_;
    QLabel* readingValid_value_;
    QPushButton* thermalStatus_button_;
    QPushButton* prochotEvent_button_;
    QPushButton* criticalTemp_button_;
    QPushButton* thermalThreshold1_button_;
    QPushButton* thermalThreshold2_button_;
    QPushButton* powerLimitation_button_;
    QPushButton* currentLimitation_button_;
    QPushButton* domainLimitation_button_;
    QScrollArea* scroll_area_;
    QWidget* scroll_widget_;

  private slots:
    void thermalStatus_reset();
    void prochotEvent_reset();
    void criticalTemp_reset();
    void thermalThreshold1_reset();
    void thermalThreshold2_reset();
    void powerLimitation_reset();
    void currentLimitation_reset();
    void domainLimitation_reset();
};

class ThermalStatus final : public TabMemberTemplate {
  Q_OBJECT
  public:

    explicit ThermalStatus(
        const SingleCpuInfo&,
        const SingleCpuId&,
        TabMemberValues&,
        TabMemberSettings&,
        QWidget *parent = nullptr);

    ~ThermalStatus() override = default;

    void load() override {}
    void store() override {}
    void refresh() override {}
    bool read(std::ostringstream&) override { return true; }
    bool apply() override { return true; }
    bool compare() override { return false; }
    void timed(bool) override;

  private:
    int tabIndex_ { 0 };
    bool pkg_supported_;
    bool core_supported_;
    QTabWidget* tabs_;
    ThermalStatusPackage* package_;
    std::vector<ThermalStatusCore*> core_;

  private slots:
    void tabSwitchSlot(int);
};

#endif

