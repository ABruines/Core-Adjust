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

/**
  * @file src/core-adjust-qt/VoltageOffsetSlider.hpp
  * @brief A complete voltage slider widget for a single FIVR voltage plane.
  *
  * @file src/core-adjust-qt/VoltageOffsetSlider.cpp
  * @brief A complete voltage slider widget for a single FIVR voltage plane (implementation).
  *
  * @class VoltageOffsetSlider
  * @brief A complete voltage slider widget for a single FIVR voltage plane.
  *
  * @fn explicit VoltageOffsetSlider::VoltageOffsetSlider(const char* label, QWidget *parent = nullptr)
  * @param label The label for this slider.
  * @param parent The parent widget.
  *
  * @fn void VoltageOffsetSlider::setChecked(int state)
  * @brief Enable/disable the slider widget.
  *
  * @fn int VoltageOffsetSlider::isChecked()
  * @brief Test if the slider is enabled.
  *
  * @fn void VoltageOffsetSlider::setRange(enum Range range, bool allowPositiveValues)
  * @brief Set the range for the slider.
  *
  * @fn void VoltageOffsetSlider::setVoltage(double mv, bool autorange, bool allowPositiveValues)
  * @brief Set the voltage (slider position) of the slider
  *
  * @fn double VoltageOffsetSlider::getVoltage()
  * @brief Get the currently selected voltage (slider position).
  *
  * @fn void VoltageOffsetSlider::rangeSelected(enum VoltageOffsetSlider::Range)
  * @brief Signal emitted when a new range is selected.
  *
  * @fn void VoltageOffsetSlider::voltageChanged()
  * @brief Signal emitted when the voltage slider is moved.
  */
// STL
#include <iomanip>
// Qt
#include <QHBoxLayout>
#include <QVBoxLayout>
// App
#include "VoltageOffsetSlider.hpp"
#include "TabMember.hpp"

VoltageOffsetSlider::VoltageOffsetSlider(const char* label, QWidget *parent)
  : QWidget(parent) {

  /* widgets */

  enable_ = new QCheckBox(label, this);
  enable_->setChecked(true);

  range_125_ = new QRadioButton("125 mV", this);   // -128, 128
  range_250_ = new QRadioButton("250 mV", this);   // -256, 256
  range_1000_ = new QRadioButton("1000 mV", this); // -1023, 1023

  slider_ = new SaferSlider(Qt::Horizontal , this);
  value_ = new QLabel(this);


  label1_ = new QLabel("Voltage Offset:", this);
  label2_ = new QLabel("Range:", this);
  label3_ = new QLabel("", this);

  enable_->setMinimumWidth(110);
  enable_->setMaximumWidth(110);
  value_->setMinimumWidth(80);
  label3_->setMinimumWidth(80);
  value_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  slider_->setRange(-128, 128);
  slider_->setPageStep(1);
  range_125_->setChecked(true);

  /* layout */

  auto *hbox = new QHBoxLayout(this);
  auto *vbox = new QVBoxLayout(nullptr);
  auto *hbox_top = new QHBoxLayout(nullptr);
  auto *hbox_bottom = new QHBoxLayout(nullptr);

  hbox_top->addWidget(label1_, 0);
  hbox_top->addWidget(slider_, 1);
  hbox_top->addWidget(value_, 0);

  hbox_bottom->addWidget(label2_, 0);
  hbox_bottom->addStretch(1);
  hbox_bottom->addWidget(range_125_, 0);
  hbox_bottom->addWidget(range_250_, 0);
  hbox_bottom->addWidget(range_1000_, 0);
  hbox_bottom->addWidget(label3_, 0);
  hbox_bottom->addStretch(1);

  vbox->addStretch(1);
  vbox->addLayout(hbox_top, 0);
  vbox->addLayout(hbox_bottom, 0);
  vbox->addStretch(1);

  hbox->setMargin(0);
  hbox->addWidget(enable_, 0);
  hbox->addSpacing(3);
  hbox->addLayout(vbox, 0);

  /* signals */

  connect(enable_, SIGNAL(stateChanged(int)), this, SLOT(enableStateChangedSlot(int)));
  connect(slider_, SIGNAL(valueChanged(int)), this, SLOT(valueChangedSlot(int)));
  connect(range_125_, SIGNAL(clicked(bool)), this, SLOT(clickedRange125(bool)));
  connect(range_250_, SIGNAL(clicked(bool)), this, SLOT(clickedRange250(bool)));
  connect(range_1000_, SIGNAL(clicked(bool)), this, SLOT(clickedRange1000(bool)));
}

void VoltageOffsetSlider::setVoltage(double mv, bool autorange, bool allowPositiveValues)
{
  /* Calculate the integer value that equals the given voltage in units of 1/1024 volt. */
  int value = static_cast<int>(mv / VoltageOffsets::Settings::VOLTAGE_OFFSET_STEP);

  /* Recalculate the voltage to ignore any rounding error in the given voltage */
  double voltage = value * VoltageOffsets::Settings::VOLTAGE_OFFSET_STEP;

  /* Format the text to place in QLabel value_ */
  std::stringstream ss;
  ss << std::setprecision(1) << std::setw(6) << std::fixed << std::showpos
     << voltage << " mV";

  /* Set the new value */

  if (autorange) {
    int v = abs(value);
    if (v > 128) {
      if (v > 256) {
        /* range is 1000mV */
        slider_->setRange(-1023, (allowPositiveValues) ? 1023 : 0);
        range_1000_->setChecked(true);
      }
      else {
        /* range is 250mV */
        slider_->setRange(-256, (allowPositiveValues) ? 256 : 0);
        range_250_->setChecked(true);
      }
    }
    else {
      /* range is 125mV */
      slider_->setRange(-128, (allowPositiveValues) ? 128 : 0);
      range_125_->setChecked(true);
    }
  }

  slider_->setValue(value);
  value_->setText(QString::fromStdString(ss.str()));
}

double VoltageOffsetSlider::getVoltage() {
  return slider_->value() * VoltageOffsets::Settings::VOLTAGE_OFFSET_STEP;
}

void VoltageOffsetSlider::setRange(enum Range range, bool allowPositiveValues) {
  switch (range) {
    case KeepCurrent:
      if (allowPositiveValues) {
        if (range_125_->isChecked()) {
          slider_->setRange(-128, 128);
        }
        else if (range_250_->isChecked()) {
          slider_->setRange(-256, 256);
        }
        else {
          slider_->setRange(-1023, 1023);
        }
      }
      else {
        if (range_125_->isChecked()) {
          slider_->setRange(-128, 0);
        }
        else if (range_250_->isChecked()) {
          slider_->setRange(-256, 0);
        }
        else {
          slider_->setRange(-1023, 0);
        }
      }
      break;
    case Range125:
      if (allowPositiveValues) {
        slider_->setRange(-128, 128);
      }
      else {
        slider_->setRange(-128, 0);
      }
      range_125_->setChecked(true);
      break;
    case Range250:
      if (allowPositiveValues) {
        slider_->setRange(-256, 256);
      }
      else {
        slider_->setRange(-256, 0);
      }
      range_250_->setChecked(true);
      break;
    case Range1000:
      if (allowPositiveValues) {
        slider_->setRange(-1023, 1023);
      }
      else {
        slider_->setRange(-1023, 0);
      }
      range_1000_->setChecked(true);
      break;
  }
}

void VoltageOffsetSlider::setChecked(int state) {
  enable_->setChecked(state);
}

int VoltageOffsetSlider::isChecked() {
  return enable_->isChecked();
}

void VoltageOffsetSlider::enableStateChangedSlot(int state) {
  slider_->setEnabled(state);
  value_->setEnabled(state);
  range_125_->setEnabled(state);
  range_250_->setEnabled(state);
  range_1000_->setEnabled(state);
  label1_->setEnabled(state);
  label2_->setEnabled(state);
  label3_->setEnabled(state);
  emit voltageChanged();
}

void VoltageOffsetSlider::valueChangedSlot(int v) {
  /* Calculate the voltage using the given value (= units of 1/1024 volt) */
  double voltage = v * VoltageOffsets::Settings::VOLTAGE_OFFSET_STEP;
  /* Update the value displayed in the UI */
  std::stringstream ss;
  ss << std::setprecision(1) << std::fixed << std::setw(6) << std::showpos << voltage << " mV";
  value_->setText(QString::fromStdString(ss.str()));
  /* Emit a signal */
  emit voltageChanged();
}

void VoltageOffsetSlider::clickedRange125(bool) {
  emit rangeSelected(Range125);
}

void VoltageOffsetSlider::clickedRange250(bool) {
  emit rangeSelected(Range250);
}

void VoltageOffsetSlider::clickedRange1000(bool) {
  emit rangeSelected(Range1000);
}

