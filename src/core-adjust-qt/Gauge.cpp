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
#include <QDebug>
#include <QPainter>
#include "Gauge.hpp"

/* Safe floating point boolean == operation */
#define EQUAL(a, b) ((a) <= (b) && (a) >= (b))

/*
 *  GaugeBase implementation
 */

void GaugeBase::calculateAngles() {
  /* The gauge is drawn on a 'canvas' with a cartesian coordinate system
   * that runs from -100 to +100 on both axis, so the coordinate 0,0 is the
   * center of the canvas.
   *
   * 'max_x' is the maximum x coordinate that our gauge will occupy when given
   * the portion of the circle bewteen its 0% and 100% marks (ie. the span of
   * the gauge). 'max_x' applies to both sides of the x axis so the total
   * occupied width runs from -max_x to +max_x. */
  int max_x = 0;

  /* 'max_y' is the maximum y coordinate that our gauge will occupy when given
   * the same restraints as 'max_x'. 'max_y' only applies to the bottom halve
   * of the y axis, the top halve is allways fully occupied,
   * thus the total gauge height runs from -100 to +max_y. */
  int max_y = 0;

  /* limit the dail arc to 30...360° */
  if (span_ > 360.) span_ = 360.;
  else if (span_ < 30.) span_ = 30.;

  /* angle of 0% mark before adjustment (0% mark @ 0° and 50% mark @ +180°) */
  double offset_angle = (360. - span_) / 2.;

  /* angles of 0% and 100% mark after adjustment (0% mark @ +90° and 50% mark @ +270°) */
  min_angle_ = 90. + offset_angle;
  max_angle_ = min_angle_ + span_;

  /* Determine max_x and max_y */
  if (span_ >= 360.) {
    max_x = 100;
    max_y = 100;
  }
  else {
    if (span_ >= 180.) {
      max_x = 100;
      max_y = static_cast<int>(100 * cos(deg2rad(90. - max_angle_)));
    }
    else /* span < 180 */ {
      max_x = static_cast<int>(100 * cos(deg2rad(90. - (max_angle_ - 270.))));
      max_y = 0;
    }
  }

  /* Adjust max_y for the part of the needle that sticks out from
   * the center on the bottom side of the indicator. */
  if (max_y < 8) max_y = 8;

  /* Total width and height of the gauge as displayed in the widget */
  double w = max_x * 2;
  double h = 100 + max_y;

  gauge_width_factor_ = w / h;  /* ratio to calculate gauge width from gauge height */
  gauge_height_factor_ = h / w; /* ratio to calculate gauge height from gauge width */

  /* Factor used to calculate the size of the background image */
  if (span_ >= 180.) background_size_factor_ = 1.;
  else background_size_factor_ = 1. / cos(deg2rad(360. - max_angle_));
}

QImage* GaugeBase::generateImage(double value) {
  /* Determine the required size of the image */
  double image_size = gauge_width_ * background_size_factor_;

  /* Redraw the background image if needed and cache it */
  if (!cached_background_ || !EQUAL(cached_background_->width(), image_size)) {
    delete cached_background_;
    cached_background_ = generateBackground(image_size);
  }

  /* Draw the indicator onto a copy of the cached background image */
  auto&& image = drawIndicator(*cached_background_, value);

  /* Return the smallest rectangle that fully contains the gauge */
  return new QImage(image.copy(
      static_cast<int>((image_size - gauge_width_) / 2), 0,
      gauge_width_, gauge_height_));
}

void GaugeBase::paintEvent(QPaintEvent*) {
  /* A change in widget width or height always means we need to generate a new image */
  if (widget_width_ != width() || widget_height_ != height()) {
    widget_width_ = width();
    widget_height_ = height();

    /* Re-calculate the width and height of the gauge image using the current
     * width and height of the widget. */
    if (widget_height_ <= widget_width_ * gauge_height_factor_) {
      gauge_width_ = static_cast<int>(widget_height_ * gauge_width_factor_);
      gauge_height_ = widget_height_;
    }
    else {
      gauge_width_ = widget_width_;
      gauge_height_ = static_cast<int>(widget_width_ * gauge_height_factor_);
    }

    /* Force a re-draw of the image. */
    value_changed_ = 1;
  }

  /* Only generate a new image when the gauge 'value' has changed
   * (or when the cached image is empty). */
  if (!cached_gauge_ || value_changed_) {
    delete cached_gauge_;
    cached_gauge_ = generateImage(value_);
    value_changed_ = 0;
  }

  /* Draw the (cached) image onto the center of the widget. */
  QPainter painter(this);
  painter.drawImage(
    (width() - cached_gauge_->width()) / 2,
    (height() - cached_gauge_->height()) / 2,
    *cached_gauge_
  );
}

GaugeBase::GaugeBase(
    QWidget* parent,
    int available_width,
    int available_height,
    double span)
    : QWidget(parent) {

  /* Set the span of the gauge dail. */
  span_ = span;

  /* Calculate the angles and the scaling factors. */
  calculateAngles();

  /* Set the gauge size given the available width and height. */
  if (available_height <= available_width * gauge_height_factor_) {
    gauge_width_ = static_cast<int>(available_height * gauge_width_factor_);
    gauge_height_ = available_height;
  }
  else {
    gauge_width_ = available_width;
    gauge_height_ = static_cast<int>(available_width * gauge_height_factor_);
  }

  /* Resize the widget to the size of the gauge. */
  widget_width_ = gauge_width_;
  widget_height_ = gauge_height_;
  resize(widget_width_, widget_height_);
}

GaugeBase::~GaugeBase() {
  delete cached_gauge_;
  delete cached_background_;
}

double GaugeBase::span() const {
  return span_;
}

double GaugeBase::value() const {
  return value_;
}

int GaugeBase::heightForWidth(int width) const {
  return static_cast<int>(width * gauge_height_factor_);
}

int GaugeBase::widthForHeight(int height) const {
  return static_cast<int>(height * gauge_width_factor_);
}

QSize GaugeBase::sizeHint() const {
  return QSize(widget_width_, widget_height_);
}

void GaugeBase::setValue(double v) {
  // Only update if the value differs.
  if (!EQUAL(value_, v)) {
    value_ = v;
    value_changed_ = 1;
    update();
  }
}

/*
 * AnimatedGaugeBase implementation
 */

AnimatedGaugeBase::AnimatedGaugeBase(
    QWidget* parent, int available_width, int available_height,
    double span, bool fluent)
    : GaugeBase(parent, available_width, available_height, span),
      animation_(this, "value", this),
      fluent_(fluent) { }

bool AnimatedGaugeBase::isFluent() const {
  return fluent_;
}

void AnimatedGaugeBase::setValue(double v) {
  if (!fluent_) v = round(v);
  GaugeBase::setValue(v);
}

void AnimatedGaugeBase::setValueAnimated(double value, unsigned long duration) {
  if (!EQUAL(value_, value)) {
    animation_.stop();
    animation_.setDuration(static_cast<int>(duration));
    animation_.setStartValue(value_);
    animation_.setEndValue(value);
    animation_.start(QAbstractAnimation::KeepWhenStopped);
  }
}

void AnimatedGaugeBase::setFluent(bool f) {
  fluent_ = f;
}

/*
 * Gauge implementation
 */

QImage* Gauge::generateBackground(double image_size)
{
  static const auto& bgColor   = QColor(0,0,0,1);//palette().color(QPalette::Window);
  const auto& textColor        = palette().color(QPalette::WindowText);
  const auto& tenPercentColor  = palette().color(QPalette::WindowText);
  const auto& fivePercentColor = palette().color(QPalette::WindowText);
  const auto& onePercentColor  = palette().color(QPalette::WindowText);

  auto* img = new QImage(
      static_cast<int>(image_size),
      static_cast<int>(image_size),
      QImage::Format_ARGB32_Premultiplied);

  img->fill(bgColor);

  /* Draw % text using the widget's font @ ??px size */
  QFont f(font());
  if (span_ > 90.) {
    f.setPixelSize(8);
  }
  else {
    if (span_ > 45.) {
      f.setPixelSize(6);
    }
    else {
      f.setPixelSize(4);
    }
  }

  /* Draw the gauge onto the image */
  QPainter painter(img);
  painter.setRenderHint(QPainter::Antialiasing);

  /* Map the image width/height to a -100..100 points
   * coordinate system on both axis. */
  painter.translate(image_size / 2, image_size / 2);
  painter.scale(image_size / 200, image_size / 200);

  /* Draw the text-label */
  if (!label_.isEmpty()) {
    QFont f(font());
    f.setPixelSize(12);
    painter.setFont(f);
    painter.setPen(textColor);
    painter.drawText(0 - painter.fontMetrics().width(label_) / 2, -25, label_);
  }

  painter.setFont(f);
  auto&& fm = painter.fontMetrics();

  /* rotate (the coordinate system) to the 0% mark. */
  painter.rotate(min_angle_);

  /* - draw a 10% mark @ every range/10 degrees */
  painter.save();
  if (span_ >= 90.) {
    /* (horizontal text if span >= 120) */
    painter.rotate(90.);
    const int y = -(85 - f.pixelSize());
    for (int j = 0; j < ((span_ >= 360.0) ? 10 : 11); ++j) {
      /* draw tick mark */
      painter.setPen(tenPercentColor);
      painter.drawLine(0, -85, 0, -96);
      /* draw percentage text */
      auto&& s = QString::number(j * 10);
      int x = static_cast<int>(-fm.width(s) / 2.);
      painter.setPen(textColor);
      painter.drawText(x, y, s);
      /* rotate the coordinate-system to the next mark */
      painter.rotate(span_ / 10);
    }
  }
  else {
    /* (vertical text if span < 120) */
    const int y = static_cast<int>((f.pixelSize() / 2.) - 1);
    for (int j = 0; j < ((span_ >= 360.0) ? 10 : 11); ++j) {
      /* draw tick mark */
      painter.setPen(tenPercentColor);
      painter.drawLine(85, 0, 96, 0);
      /* draw percentage text */
      auto&& s = QString::number(j * 10);
      int x = 85 - fm.width(s) - 2;
      painter.setPen(textColor);
      painter.drawText(x, y, s);
      /* rotate the coordinate-system to the next mark */
      painter.rotate(span_ / 10);
    }
  }
  painter.restore();

  /* - Draw a 5% mark @ every range/20 degrees,
   *   but skip every 10% mark (allready drawn). */
  painter.save();
  painter.setPen(fivePercentColor);
  for (int j = 0; j < 20; ++j) {
    if ((j % 2) != 0) painter.drawLine(90, 0, 96, 0);
    painter.rotate(span_ / 20);
  }
  painter.restore();

  /* - Draw a 1% mark @ every range/100 degrees (if span_ >= 90°),
   *   but skip every 5% mark (allready drawn) */
  if (span_ >= 90.) {
    painter.save();
    painter.setPen(onePercentColor);
    for (int j = 0; j < 100; ++j) {
      if ((j % 5) != 0) {
        painter.drawLine(94, 0, 96, 0);
      }
      painter.rotate(span_ / 100);
    }
    painter.restore();
  }

  painter.end();
  return img;
}

QImage Gauge::drawIndicator(const QImage& background, double value) {
  static constexpr const QPoint needle[] = {
    QPoint(2, 8),
    QPoint(1, -92),
    QPoint(-1, -92),
    QPoint(-2, 8)
  };
  const double image_size = background.width();
  QImage img(background);
  QPainter painter(&img);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.translate(image_size / 2, image_size / 2);
  painter.scale(image_size / 200, image_size / 200);
  painter.rotate(min_angle_ + 90.0 + ((span_ / 100) * value));
  painter.setPen(Qt::NoPen);
  painter.setBrush(palette().color(QPalette::WindowText));
  painter.drawConvexPolygon(needle, sizeof(needle) / sizeof(QPoint));
  return img;
}

