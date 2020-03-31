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

#include "SaferSlider.hpp"

bool SaferSlider::event(QEvent *e) {
  if (e->type() == QEvent::Leave) {
    /* Clear the keyboard focus, when the mousepointer leaves the slider. */
    e->accept();
    blockSignals(true);
    clearFocus();
    blockSignals(false);
    return true;
  }
  if (e->type() == QEvent::Wheel) {
    /* Only allow the slider to move on wheel events when it has focus */
    e->ignore();
    if (hasFocus()) {
      return QSlider::event(e);
    }
    return true;
  }
  /* Not our event */
  return QSlider::event(e);
}

