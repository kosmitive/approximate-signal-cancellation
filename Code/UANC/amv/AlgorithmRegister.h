//
// Created by markus on 16.01.17.
//

#ifndef UANC_ALGORITHMREGISTER_H
#define UANC_ALGORITHMREGISTER_H

#include <memory>
#include "anc/algorithm/InverseDirectAlgorithm.h"
#include "anc/algorithm/InverseFFTAlgorithm.h"

namespace uanc { namespace amv {

// include namespace of the algorithms to support readability.
using namespace anc::algorithm;

/** \brief This class is used to retain a list of all algorithms.
 *
 * This class represents the extendability point for all algorithms. If you have implemented
 * an algorithm simply register it inside of this class and it will automatically be integrated
 * in the application.
 */
class AlgorithmRegister {
 public:

  /** \brief Supply all registered algorithms.
   *
   * This method basically returns a list of all algorithms registered inside of the application.
   * To add you new implemented one, simply add it to the list at the bottom.
   *
   * @return A vector containing a prototype of all algorithms.
   */
  static std::shared_ptr<std::vector<uanc::amv::IAlgorithm*>> getAlgorithms() {

    // simply create an empty vector
    auto lst = std::shared_ptr<std::vector<uanc::amv::IAlgorithm*>>(new std::vector<uanc::amv::IAlgorithm*>());

    // --------------------------------------
    // Here you can register your algorithms:
    // --------------------------------------

    lst->push_back(new InverseDirectAlgorithm());
    lst->push_back(new InverseFFTAlgorithm());

    // --------------------------------------

    return lst;
  }
};
}}

#endif //UANC_ALGORITHMREGISTER_H
