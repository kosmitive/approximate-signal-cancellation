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
#include <iostream>
#include <string>
#include <utility>
#include "Code/UANC/util/tools/Path.h"
#include "Code/libs/aquila/source/WaveFile.h"
#include "Code/UANC/amv/anc/algorithm/InverseFFTAlgorithm.h"
#include "Code/UANC/amv/anc/ANCAlgorithmRegister.h"
#include "Code/UANC/util/GlobalSettings.h"
#include "Code/UANC/gui/MainWidget.h"
#include "GlobalSettingsBar.h"

namespace uanc {
namespace gui {

/** \brief This is the main widget constructor.
 *
 * This basically initializes the gui and does some other
 * initialization stuff.
 */
MainWidget::MainWidget() {
  this->setupGUI();
}

/** \brief This method will setup the gui appropriately
 *
 * It creates basically a main widget and a menu inside of the main window.
 */
void MainWidget::setupGUI() {
  // create the layout. Therefore create the apply button
  // and the corresponding combobox displaying the algorithms
  QVBoxLayout *layout = new QVBoxLayout;
  this->_buttonApply = new QPushButton("Apply");
  this->_cmbAlgorithm = new QComboBox();
  this->_progressIndicator = new QProgressIndicator();

  // connect the handler to the button
  connect(this->_buttonApply, SIGNAL(clicked()), this, SLOT(applyClicked()));

  // register algorithms and add them to the combobox
  this->_algorithmList = uanc::amv::anc::ANCAlgorithmRegister::getAlgorithms();
  this->showAvailableAlgorithms();

  // basically create a hbox layout
  QWidget *hbar = new QWidget;
  QHBoxLayout *hlayout = new QHBoxLayout;

  // add subwidgets and set the correct size policies
  this->_buttonApply->setFixedWidth(80);
  this->_progressIndicator->setFixedWidth(80);
  hlayout->addWidget(this->_cmbAlgorithm);
  hlayout->addWidget(this->_buttonApply);
  hlayout->addWidget(this->_progressIndicator);
  this->_progressIndicator->hide();

  // set the correct layout options
  hbar->setLayout(hlayout);
  hbar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);

  // construct the complete layout
  this->_tabWidget = new QTabWidget();
  this->_tabWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  this->_tabWidget->setTabsClosable(true);
  connect(this->_tabWidget,
          SIGNAL(tabCloseRequested(int)), this, SLOT(waveClosed(int)));

  // construct the complete layout
  this->_detailTabWidget = new QTabWidget();
  this->_detailTabWidget->setSizePolicy(
      QSizePolicy::Minimum, QSizePolicy::Minimum);
  this->_detailTabWidget->setTabsClosable(true);
  connect(this->_detailTabWidget,
          SIGNAL(tabCloseRequested(int)),
          this,
          SLOT(algorithmClosed(int)));

  // add new algorithm for a chosen detailTabWidget
  connect(this->_tabWidget,
          SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));

  layout->addWidget(this->_tabWidget);
  layout->addWidget(hbar);
  layout->addWidget(this->_detailTabWidget);

  this->setLayout(layout);
  this->show();
}

/** \brief This method gets used to show the algorithms inside of the combobox
 *
 * This method displays the algorithms inside of the combobox.
 */
void MainWidget::showAvailableAlgorithms() {
  // get size of algorithm list
  auto size = this->_algorithmList->size();

  // now basically iterate over the elements
  for (size_t i = -1; ++i < size;) {
    this->_cmbAlgorithm->addItem(
        QString::fromStdString(
            this->_algorithmList->at(i)->getName()));
  }
}

/** \brief This gets fired, when the direct inverse button is clicked
 *
 * Gets fired, whenever a user clicks on the direct inverse button.
 */
void MainWidget::applyClicked() {
  // get the current index and using that get the correct algorithm
  // After that simply apply the algorithm
  auto currentIndex = _cmbAlgorithm->currentIndex();
  _algorithm = this->_algorithmList->at(currentIndex)->clone();

  this->applyAlgorithm(_algorithm);
}

/** Gets called when the algorithm is finished. */
void MainWidget::algorithmFinished() {
  auto cIndex = this->_tabWidget->currentIndex();

  // get the mapping list and push back the algorithm save afterwards
  auto vec = this->_waveAlgorithMapping.at(this->_algorithmTabIndex);
  vec->push_back(std::shared_ptr<uanc::amv::IAlgorithm>(_algorithm));
  this->_waveAlgorithMapping.insert(
      std::make_pair(this->_algorithmTabIndex, vec));

  // we want to simply derive a anc a view and combine them
  if (cIndex == this->_algorithmTabIndex) {
    this->_detailTabWidget->addTab(
        _algorithm->getView()->getWidget(),
        QString::fromStdString(_algorithm->getName()));
  } else {
    this->_tabWidget->setCurrentIndex(this->_algorithmTabIndex);
  }
  _algorithm->fillView();
  delete algoThread;
  _algorithm = nullptr;

  // show loading badge
  this->_progressIndicator->hide();
  // this->_progressIndicator->stopAnimation();
  this->_buttonApply->show();
}

bool tabInRun = false;

/** \brief Simple signal for a differenct selected tab */
void MainWidget::tabSelected() {
  if (tabInRun) return;

  // remove all tabs from the detail widget
  for (int i = this->_detailTabWidget->count() - 1; i >= 0; --i) {
    this->_detailTabWidget->removeTab(i);
  }

  // get the list of algorithms
  auto index = this->_tabWidget->currentIndex();
  uanc::util::GlobalSettings::get()->currentIndex = index;

  // get the mapping list and push back the algorithm save afterwards
  auto vec = this->_waveAlgorithMapping.at(index);

  // iterate over vector and fill new plots in
  for (auto it = vec->begin(); it != vec->end(); ++it) {
    auto algo = (*it).get();
    this->_detailTabWidget->addTab(algo->getView()->getWidget(),
                                   QString::fromStdString(algo->getName()));
    this->_detailTabWidget->update();
  }
}

/** \brief loads the signal source.
 *
 * This method loads a signal source in the top tab view.
 * @param signalSource the signal source to load.
 */
void MainWidget::loadSignalSource(std::shared_ptr<InvertedModel> signalSource) {
  // Simply add the tab and block the rest
  tabInRun = true;
  std::string text = "Standard";
  auto castedObj = std::dynamic_pointer_cast<
      Aquila::WaveFile>(signalSource->left_channel);
  if (castedObj.get() != nullptr) {
    text = uanc::util::Path::getFileName(castedObj->getFilename());
  }

  auto vBoxLayout = new QVBoxLayout();
  auto widget = new QWidget();
  widget->setLayout(vBoxLayout);
  uanc::util::GlobalSettings::get()->currentIndex =
      this->_tabWidget->addTab(widget, QString::fromStdString(text));
  this->_tabWidget->setCurrentIndex(uanc::util::GlobalSettings::get()->currentIndex);

  // add GlobalSettingsBar
  auto globalSettingsBar =
      new GlobalSettingsBar(
          signalSource->right_channel->length() > 0);
  vBoxLayout->addWidget(globalSettingsBar);
  globalSettingsBar->setSizePolicy(
      QSizePolicy::Minimum, QSizePolicy::Maximum);

  // create new widget with appropriate bar.
  auto signalWidget = new SignalViewWidget();
  signalWidget->setSignalModel(signalSource);
  vBoxLayout->addWidget(signalWidget);

  // set tabinRun to false
  tabInRun = false;


  auto vec = std::make_shared<
      std::vector<
          std::shared_ptr<uanc::amv::IAlgorithm>>>();
  this->_waveAlgorithMapping.insert(
      std::make_pair(uanc::util::GlobalSettings::get()->currentIndex, vec));

  tabSelected();
}

/** \brief This method can be used to apply an algorithm to the inner data.
 *
 * This algorithm basically applies the algorithm to the inner data.
 *
 * @param algorithm The algorithm to use
 */
int MainWidget::applyAlgorithm(uanc::amv::IAlgorithm *algorithm) {
  // check if signal available if not present a messagebox and
  // ask the user to load a signal.
  auto signalManager = uanc::util::SignalManager::get();
  auto index = this->_tabWidget->currentIndex();
  auto signal = signalManager->getSignal(index);
  if (signal == NULL) {
    QMessageBox msgBox;
    msgBox.setText("You have to load a signal first."
                       " Pleasy use File -> Open File...");
    msgBox.setWindowTitle("No signal loaded.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    return 1;
  }

  algorithm->getView()->getWidget();

  // create new algorithm thread
  algoThread = new AlgorithmThread();
  algoThread->setAlgorithm(algorithm, signal);
  this->connect(algoThread,
                SIGNAL(algorithmFinished()), this, SLOT(algorithmFinished()));

  // show loading badge
  this->_buttonApply->hide();
  this->_progressIndicator->show();
  this->_progressIndicator->startAnimation();
  this->_algorithmTabIndex = this->_tabWidget->currentIndex();

  algoThread->start();

  return 0;
}

void MainWidget::waveClosed(const int &index) {
  if (index == -1) {
    return;
  }

  if (_algorithm != nullptr) {
    QMessageBox msgBox;
    msgBox.setText("You can't close wave tabs, "
                       "while an algorithm is executing.");
    msgBox.setWindowTitle("Algorithm is executing.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    return;
  }

  tabInRun = true;
  // Removes the tab at position index from this stack of widgets.
  // The page widget itself is not deleted.
  delete this->_tabWidget->currentWidget();
  this->_tabWidget->removeTab(index);
  tabInRun = false;

  // additionally remove the algorithm from the inner mapping
  auto vec = std::make_shared<
      std::vector<
          std::shared_ptr<
              uanc::amv::IAlgorithm>>>();

  // iterate from closed to end
  for (int i = index; i < this->_tabWidget->count(); ++i) {
    auto vec = this->_waveAlgorithMapping.at(i + 1);
    this->_waveAlgorithMapping.erase(i);
    this->_waveAlgorithMapping.insert(std::make_pair(i, vec));
  }

  // Deleate signal
  auto signalManager = uanc::util::SignalManager::get();
  signalManager->eraseSignal(index);

  // do the final erase
  this->_waveAlgorithMapping.erase(this->_tabWidget->count());

  // special case occurs, when it was the last tab
  if (this->_tabWidget->count() > 0) {
    this->tabSelected();
  } else {
    // remove all tabs from the detail widget
    for (int i = this->_detailTabWidget->count() - 1; i >= 0; --i) {
      this->_detailTabWidget->removeTab(i);
    }
  }
}

void MainWidget::algorithmClosed(const int &index) {
  if (index == -1) {
    return;
  }

  if (_algorithm != nullptr) {
    QMessageBox msgBox;
    msgBox.setText("You can't close algorithm tabs,"
                       " while another algorithm is executing.");
    msgBox.setWindowTitle("Algorithm is executing.");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    return;
  }


  tabInRun = true;
  // Removes the tab at position index from this stack of widgets.
  // The page widget itself is not deleted.
  this->_detailTabWidget->removeTab(index);
  tabInRun = false;

  // additionally remove the algorithm from the inner mapping
  auto vec = std::make_shared<
      std::vector<
          std::shared_ptr<uanc::amv::IAlgorithm>>>();

  // get the top index
  auto indexTop = this->_tabWidget->currentIndex();

  auto el = this->_waveAlgorithMapping.at(indexTop);
  el->erase(el->begin() + index);
}

}  // namespace gui
}  // namespace uanc
