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

#ifndef CoreAdjust_SaferSlider
#define CoreAdjust_SaferSlider

#include <QSlider>
#include <QEvent>

/**
  * @file src/core-adjust-qt/SaferSlider.hpp
  * @brief  A QSlider class that only moves on scrollwheel events when it has keyboard focus.
  *
  * @file src/core-adjust-qt/SaferSlider.cpp
  * @brief  A QSlider class that only moves on scrollwheel events when it has keyboard focus (implementation).
  *
  * @class SaferSlider
  * @brief A QSlider class that only moves on scrollwheel events when it has keyboard focus.
  *
  * It also loses keyboard focus as soon as the mousepointer
  * leaves the QSlider widget. */
class SaferSlider : public QSlider {
  Q_OBJECT
  public:
    using QSlider::QSlider;
    ~SaferSlider() override = default;
  protected:
    bool event(QEvent *e) override;
};

#endif

