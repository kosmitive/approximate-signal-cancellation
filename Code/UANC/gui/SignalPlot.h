/* Simplified ANC Model, only targets inversion, but can be extended. University project.
 *  Copyright (C) 2017 Danielle Ceballos, Janne Wulf, Markus Semmler, Roman Rempel, Vladimir Roskin.

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UANC_SIGNALPLOT_H
#define UANC_SIGNALPLOT_H

#include <memory>
#include <Code/libs/qplot/qcustomplot.h>

#include "PlotWidget.h"

namespace uanc {
namespace gui {

class PlotWidget;

class SignalPlot : public QCustomPlot {

 Q_OBJECT

 public:
    /** \brief The default Constructor
   */
  SignalPlot(PlotWidget* parent);

    /** \brief sets the data to plot the signal
     *
     * @param data this is the data that has to be ploted
     */
  void setData(QCPGraphDataContainer *data);

    /** \brief sets the data to plot the signal
     *
     * @param error this is the eroor that has to be ploted
     */
  void setError(QCPGraphDataContainer *error);

    /** \brief sets the range of the plot
     *
     * @param lower the lower border
     * @param upper the upper border
     */
  void setRange(double lower, double upper);

 public slots:
    /** \brief simple set error to visible or not
   *
   * @param hide boolean to determine wether the error is shown or not
   */
  void hideError(bool hide);

    /** \brief simple set the YAxis to centered or not
   *
   * @param hide boolean to determine wether the YAxis is centered or not
   */
  void setCenteredYAxis(bool b) {_centeredYAxis = b;};

 private slots:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

 private:
  /** \brief Holds reference to the parent widget
   *
   */
  PlotWidget* _parent;

  /** \brief Holds the color for the plot
   *
   * Holds the QColor for the plot.
   */
  QColor _graphColor;

  QCPItemStraightLine *_zoomLineOrigin, *_zoomLineTarget;

  int _pressedX = -1, _pressedY = -1;

  const double _ZOOMFACTOR = 1.5;
  void zoom(double center);
  void unzoom(double center);
  void zoomRange(double press, double release);
  QCPRange scaleRange(double factor, double center);

  void setZoomLinePos(QCPItemStraightLine*, double xCoord);

  bool _centeredYAxis = true;
  /** \brief Rescales the y-axis depending on the max/min value (visible)
 *
 * Rescales the y-axis depending on the max/min value (visible)
 */
  void rescaleYAxis();

  std::shared_ptr<QCPGraphDataContainer> _MapExtremeValues;
  void setExtremeValues();

  const int _SIGNAL = 0;
  const int _ERROR = 1;

};

}
}
#endif //UANC_SIGNALPLOT_H
