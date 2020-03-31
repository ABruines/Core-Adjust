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

#ifndef CoreAdjust_SaferCombo
#define CoreAdjust_SaferCombo

#include <QComboBox>
#include <QEvent>

/**
  * @file src/core-adjust-qt/SaferCombo.hpp
  * @brief A QComboBox class that does not allow scrollwheel events to select items.
  *
  * @file src/core-adjust-qt/SaferCombo.cpp
  * @brief A QComboBox class that does not allow scrollwheel events to select items (implementation).
  *
  * @class SaferCombo
  * @brief A QComboBox class that does not allow scrollwheel events to select items.
  */
class SaferCombo : public QComboBox {
  Q_OBJECT
  public:
    using QComboBox::QComboBox;
    ~SaferCombo() override = default;
  protected:
    bool event(QEvent *e) override;
};

#endif

