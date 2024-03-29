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

#ifndef UANC_SIGNALMANAGER_H
#define UANC_SIGNALMANAGER_H

#include "Code/libs/aquila/source/SignalSource.h"

#include <list>
#include <memory>
#include <map>
#include <Code/UANC/amv/InvertedModel.h>

namespace uanc {
namespace util {

using namespace util;
using namespace std;
using namespace uanc::amv;

class SignalManager {
 private:
  /** \brief Map used to store the signals.
   *
   * This field gets used to save the signals inside of a map.
   */
  std::map<int, std::shared_ptr<InvertedModel>> signalsMap;

 public:

  /** \brief Shared pointer of the one and only instance of MainWindow.
   *
   * This field wraps a MainWindow inside of a shared_ptr. The main goal is that
   * there are no dangling pointer referring to MainWindow.
   */
  static std::shared_ptr<SignalManager> _instance;

  /** \brief Private constructor to deny creation outside of the singleton pattern.
    *
    * This constructor takes a QApplication and saves it internally as it context.
    */
  SignalManager();

  /** \brief Adds a signal to the internal map.
   *
   * Simply takes a string and a signal source and add them insidie of
   * the map.
   *
   * @param signalSource the signal source to adnamed.
   */
  int addSignal(const InvertedModel &signalSource);

  /** Simpel signal counter */
  int signalCounter = -1;

  /** \brief Adds a signal to the internal map.
   *
   * Simply takes a string and a signal source and add them insidie of
   * the map.
   *
   * @param identifier the identifier of the signal.
   * @param signalSource the signal source to adnamed.
   */
  int addSignal(std::shared_ptr<InvertedModel> signalSource);

  /** \brief This method returns a signal as a weak ptr.
   *
   * Return a weak pointer, pointing to the desired signal inside
   * if the manager
   *
   * @param name The name of the signal.
   *
   * @return It returns a weak pointer of aquilla sinal osource with
   * the speiciged name.
   */
  std::shared_ptr<InvertedModel> getSignal(int name);

  /** \brief Obtain a reference to the main window.
 *
 * Uses a classical singleton pattern to give back exactly the same copy of the main window.
 * In addition it uses a shared pointer.
 *
 * @return The shared pointer containing the MainWindow
 */
  static std::shared_ptr<SignalManager> get();

  /** \brief This method deletes a signal
 *
 * delete a signal at a given index
 *
 * @param index The index of the signal, wich will be deleted.
 *
 */
  void eraseSignal(int index);
};

}
}

#endif //UANC_SIGNALMANAGER_H
