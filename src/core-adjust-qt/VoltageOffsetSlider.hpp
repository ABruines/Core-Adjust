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

#ifndef CoreAdjust_VoltageOffsetSlider
#define CoreAdjust_VoltageOffsetSlider

// Qt
#include <QObject>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>
#include <QWidget>
// App
#include "SaferSlider.hpp"

/*
 * MSR 0x150 holds a (2nd complement) 11 bit value that represent the
 * offset voltage (of each voltage plane), where each step in this range
 * equals 1/1024 Volt.
 *
 * This means our (maximum) slider range is 2048 steps where each step has a
 * decimal value of 0.9765625 mV, ie:
 *
 * binary range, 2048 steps  ===   decimal range (stepping = 0.9765625)
 * -1024 ... 0 ... +1024     ===   -1000 mV ... 0 ... +1000mV
 *   
 * Class VoltageOffsetSlider provides a slider with a range of 125mV, 250mV
 * or 1000mV that can be adjusted in steps of 1/1024 Volt.
 */

class VoltageOffsetSlider : public QWidget
{
  Q_OBJECT

  public:
    enum Range {
      KeepCurrent, /**< Keep the currently selected range setting */
      Range125,    /**< -125mV ... 125mV range */
      Range250,    /**< -250mV ... 250mV range */
      Range1000    /**< -1000mV ... 1000mV range */
    };

    explicit VoltageOffsetSlider(const char* label, QWidget *parent = nullptr);
    virtual ~VoltageOffsetSlider() = default;

    void setChecked(int state);
    int isChecked();

    void setRange(enum Range range, bool allowPositiveValues);
    void setVoltage(double mv, bool autorange, bool allowPositiveValues);
    double getVoltage();

  signals:
    void rangeSelected(enum VoltageOffsetSlider::Range);
    void voltageChanged();

  private:
    QCheckBox *enable_ {nullptr};
    SaferSlider *slider_ {nullptr};
    QLabel *value_ {nullptr};
    QRadioButton *range_125_ {nullptr};
    QRadioButton *range_250_ {nullptr};
    QRadioButton *range_1000_ {nullptr};
    QLabel *label1_ {nullptr};
    QLabel *label2_ {nullptr};
    QLabel *label3_ {nullptr};

  private slots:
    void enableStateChangedSlot(int state);
    void valueChangedSlot(int value);
    void clickedRange125(bool checked);
    void clickedRange250(bool checked);
    void clickedRange1000(bool checked);
};

#endif

