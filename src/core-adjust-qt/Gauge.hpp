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
  * @file src/core-adjust-qt/Gauge.hpp
  * @brief Class for a 'gauge' widget with a round dail.
  *
  * @file src/core-adjust-qt/Gauge.cpp
  * @brief Class for a 'gauge' widget with a round dail (implementation).
  */
#ifndef CoreAdjust_Gauge
#define CoreAdjust_Gauge

#include <QImage>
#include <QObject>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QSize>
#include <QStyle>
#include <QWidget>

/*
   Requirements for class Gauge:

   - Drawn using a cartesian coordinate system with a range of -100 to 100 on both axis.
     (See below. The center is the origin of the indicator/needle, the '*' characters
     represent the span of the dail.)

   - The 'dail' shall span 360° or less and this is divided over a range of 0% to 100%,
     the 0% and 100% marks are indicated by m0 and m1 respectively.

   - When the span is less then 360°, draw a (virtual) rectangle around
     the smallest possible region that contains the gauge arc and crop the image
     displayed on the widget to that rectangle. 

   - The (gauge) image shall scale together with the widget when it is resized.

   - The widget can be created given three parameters:
     a) the span of the gauge arc
     b) the available widget width (or a default value)
     c) the available widget height (or a default value)

                                Y
                               270°

            II               *  |  *                I
                        *       |       *
                                |
                    *           |           *
                                |
                 *              |             *
                                |
               *                |               *
                                |
       180° ---*----------------+---------------*--- 0° X
                            b` /|\  b
               *              / | \             *
                             /a`| a\
                 *          /   |   \         *
                           /    |    \
                    *     /     |     \     *
                         /      |      \ 
                        *       |       *
            III        m0       |        m1        IV

                               90°

  When the span of the dail is less then 360° we can cut a piece of the image
  because it doesn't display anything and we would prefer as much gauge as
  possible in the widget;

  The height can be made smaller when the span is less the 360° and the width
  can also be made smaller is the span is less then 180°.

  In calculateAngles() the variable max_x and max_y are used to store the
  X and Y position that contains the cropped image (as measured in the
  -100..100 point coordinate sytem).

  Drawing the gauge on the widget is done by first rendering the gauge to a
  temporary image that is dimensioned to fit the entire coordinate system.
  After rendering we cut the desired rectangle from that image.

  (This 'full' image is cached and re-used as long as the size of the
  widget does not change. The cropped image is also cached an not
  re-rendered as long as value_ and widget size do not change.)

  To calculate the Y position of m1 at a given span (when span > 180°):
  Simplify the problem into a right triangle (in kwardrant IV).

        φ
        +
        |\           φ = 90° - max_angle_
        | \          H = 100, A and O are unknown
        |  \
      A |   \ H      cos φ = A / H
        |    \       A = H × cos φ
        |     \      A = 100 × cos φ
        |      \
        +-------+    max_y = 100 × cos φ
            O

  To calculate the X position of m1 at a given span (when span < 180°):
  If we rotate this problem to kwardrant I and then move φ to the opposite corner
  (to flip the axis), this becomes solveable using the same equation as above, ie:

                                        φ
      +-------+                         +                            +
      |      /                         /|                           /|
      |     /                         / |                          / |
      |    /                         /  |                         /  |
      |   /      == rotate ==>      /   |     == move φ ==>   H  /   | O
      |  /           180°          /    |                       /    |
      | /                         /     |                      /     |
      |/                         /      |                     /      |
      +                         +-------+                    +-------+
      φ                                                     φ    A

   φ = 90° - (max_angle_ - 270°)
   H = 100
   A = 100 × cos φ
   max_x = 100 × cos φ

  The region we need to cut from the 360° circle is the rectangle:
    x, y, w, h = -max_x_, -100, 2 * max_x, 100 + max_y_

  When creating a new Gauge instance we only know the span of the dail
  and the available width and height of the widget.

  To fill as much of the available space as possible (and for the
  antialiasing to function properly) the cut-out region is set to the
  widget size at every paint-event.

  However, the temporary image that is generated must be able to hold
  the entire -100..100 coordinate system.

  So we need the scaling factor to convert the width of the widget to
  the size of the temporary image.

  This problem can be visualised as (for example a span of 90°):

                            270°
                          .  .  .
   II               .        |        .                 I      m0 = angle of 0% mark (min_angle_)
               .             |             .                   m1 = angle of 100% mark (max_angle_)
      m0   .                 |                 .    m1         A  = gauge_width_ / 2 = max_x_
         .                   |                    .            φ  = 360° - max_angle_
         |\                  |                  / |
         |  \                |                /   |
         |    \              |              /     |
         |      \            |            /       |
         |        \          |       H  /         |
         |          \        |        /           |O
         |            \      |      /             |
         |              \    |    /               |
         |                \  |  /                 |
         |                  \|/  φ                |
   ------+-------------------+--------------------+-------- 0°
                             0         A         
   III                                                  IV

   We want to know the length of the hypotenuse (H).
   The width/height of the required temporary image is: hypotenuse * 2

   We know that:

    cos φ == adjacent side / hypotenuse

   So:

    H = (gauge_width_ / 2) / cos(360° - max_angle_)
    background_size == H * 2

   Which can be simplified to:

    background_size == gauge_width_ / cos(360° - max_angle_)

   or:

    background_size_factor_ == 1 / cos(360° - max_angle_)
    background_size == gauge_width_ * background_size_factor_
*/

/** @brief Base class for a gauge widget with a round dail. */
class GaugeBase : public QWidget {
  Q_OBJECT
  Q_PROPERTY(double value READ value WRITE setValue)

  private:
    /** @brief Converts degrees to radians, used by calculateAngles()
      * @note rad = deg * π / 180° */
    inline double deg2rad(double x) { return x * 3.14159265 / 180.; }

  protected:
    /* Widget Properties: */

    /** @brief The gauge dail spans this portion of a full circle (30...360°) */
    double span_ { 0. };
    /** @brief Position of the gauge indicator (ie. the needle) (0...100%) */
    double value_ { 0. };
    /** @brief Text to overlay onto the gauge dial. */
    QString label_;

    /* Protected Variables: */

    /** Nonzero when value_ has changed.
      * Set by setValue(), reset upon every paint event. */
    int value_changed_ { 0 };

    /* Set by calculateAngles() : */
    /** @brief The angle of the 0% mark (m0 == a` + a + b == a` + 90°) */
    double min_angle_ { 0. };
    /** @brief The angle of the 100% mark (m1 == m0 + span_). */
    double max_angle_ { 0. };
    /** @brief Ratio to calculate gauge_width_  from gauge_height_ */
    double gauge_width_factor_ { 1. };
    /** @brief Ratio to calculate gauge_height_ from gauge_width_ */
    double gauge_height_factor_ { 1. };
    /** @brief Ratio to calculate the size of the background image from gauge_width_ */
    double background_size_factor_ { 1. };

    /** @brief Current width of the widget, set on every paint event. */
    int widget_width_ { 0 };
    /** @brief Current height of the widget, set on every paint event. */
    int widget_height_ { 0 };

    /** @brief Width of the gauge as displayed on the widget.
      * This is determined by the widget width on every paint event. */
    int gauge_width_ { 0 };
    /** @brief Height of the gauge as displayed on the widget.
      * This is determined by the widget height on every paint event. */
    int gauge_height_ { 0 };

    /** @brief Used by generateImage() to cache the (intermediate) background
      * image, freed by the destructor. */
    QImage* cached_background_ { nullptr };

    /** @brief Used by paintEvent() to cache the (final) image returned
      * by generateImage(), freed by the destructor. */
    QImage* cached_gauge_ { nullptr };

    /* Protected Functions: */

    /** @brief Calculates the angles...
      *
      * ...of the 0% and 100% marks so that the 50% mark is
      * always at 270° (ie. pointing straight up).<br/>
      * Also determines the multiplication factors required to calculate the:<br/>
      * - largest gauge width that fits inside the widget when given the widget height.<br/>
      * - largest gauge height that fits inside the widget when given the widget width.<br/>
      * - size of the background image needed to draw the full circle of the<br/>
      *   gauge-dail when given the gauge's width as it is (finally) displayed<br/>
      *   on the widget (ie. the smallest rectangle in the background<br/>
      *   image that fully contains the gauge).
      */
    virtual void calculateAngles();
    /** @brief Must return a square image with the gauge background
      * (ie. the dail) drawn onto it.
      * @param image_size
      * The size of the Gauge if its span would be 360°.
      * @returns Pointer to the new QImage. */
    virtual QImage* generateBackground(double image_size) = 0;
    /** @brief Must return a copy of 'background' with the gauge
      * indicator (needle) drawn onto it.
      * @param background Reference to the background image.
      * @param value The value to display (in percent).
      * @returns The new QImage.
      */
    virtual QImage drawIndicator(const QImage& background, double value) = 0;
    /** @brief Generate a complete gauge image.
      * @note Calls generateBackground() and drawIndicator(), called by paintEvent(). */
    virtual QImage* generateImage(double value);

    /* Overridden functions from QWidget: */

    /** @brief Calls generateImage() and displays that image on the widget. */
    void paintEvent(QPaintEvent*) override;

  public:

    explicit GaugeBase(
        QWidget* parent = nullptr,
        int available_width = 200,
        int available_height = 200,
        double span = 360.
    );

    explicit GaugeBase(double span, QWidget* parent = nullptr)
      : GaugeBase(parent, 200, 200, span) {}

    virtual ~GaugeBase();

    /** @brief Get the span of the gauge dail.
      * @returns The span of the gauge dail (0..360°) */
    double span() const;
    /** @brief Get the value displayed on the gauge..
      * @returns The indicator (needle) position (0..100%) */
    double value() const;
    /** @brief Get the text-label displayed on the gauge..
      * @returns The text. */
    const QString& label() const { return label_; }

    int heightForWidth(int width) const override;
    virtual int widthForHeight(int height) const;

    /* Overridden functions from QWidget: */
    QSize sizeHint() const override;

  public slots:
    /** @brief Set the value of the gauge and que a paint event.
      * @param v The new value in percent (0..100). */
    virtual void setValue(double v);
    /** @brief Set the text-label displayed on the gauge.
      * @param str The text to use as label. */
    virtual void setLabel(QString str) { label_ = std::move(str); }
};

/** @brief Animation for a GaugeBase indicator (needle). */
class AnimatedGaugeBase : public GaugeBase {
  Q_OBJECT
  Q_PROPERTY(double animatedValue READ value WRITE setValueAnimated)
  Q_PROPERTY(bool fluent READ isFluent WRITE setFluent)
  protected:

    QPropertyAnimation animation_;

    /** @brief Use fluent animation of the Gauge indicator (needle).
      *
      * Set to \c true for smooth animation (more cpu power required),
      * or \c false to animate in whole percent values (less cpu power required). */
    bool fluent_;

  protected slots:

    /** @brief Set the value of the gauge and que a paint event.
      * @param v The new value in percent (0..100).
      * @note The value is rounded to a whole number if fluent_ is false.*/
    void setValue(double v) override;

  public:

    explicit AnimatedGaugeBase(
        QWidget* parent = nullptr,
        int available_width = 200,
        int available_height = 200,
        double span = 360.,
        bool fluent = false
    );

    explicit AnimatedGaugeBase(double span, QWidget* parent = nullptr)
      : AnimatedGaugeBase(parent, 200, 200, span, false) {}

    explicit AnimatedGaugeBase(double span, bool fluent, QWidget* parent = nullptr)
      : AnimatedGaugeBase(parent, 200, 200, span, fluent) {}

    virtual ~AnimatedGaugeBase() {}

    /** @brief Returns true if fluent animation rendering is enabled, false if disabled. */
    virtual bool isFluent() const;

  public slots:
    /** @brief Set the value of the gauge using animation.
      * @param value The new value in percent (0..100).
      * @param duration The amount of time (milliseconds) used to animate the indicator (needle).
      * @note The value is rounded to a whole number if fluent_ is false.*/
    virtual void setValueAnimated(double value, unsigned long duration = 200);
    /** @brief Fluent animation rendering on/off.
      * @param f True to enable, false to disabled fluent animation. */
    virtual void setFluent(bool f);
};

/** @brief A widget that displays a percentage on a gauge with a round dail. */
class Gauge : public AnimatedGaugeBase {
  Q_OBJECT
  protected:
    /* Implement pure virtuals from the base class. */
    QImage* generateBackground(double image_size) override;
    QImage drawIndicator(const QImage& background, double value) override;
  public:
    /* Inherit constructors from base class. */
    using AnimatedGaugeBase::AnimatedGaugeBase;
    /* Provide a default destructor. */
    virtual ~Gauge() = default;
};

#endif

