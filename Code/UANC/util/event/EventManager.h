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

#ifndef CODE_UANC_UTIL_EVENT_EVENTMANAGER_H_
#define CODE_UANC_UTIL_EVENT_EVENTMANAGER_H_

#include <memory>
#include <utility>
#include <vector>
#include <unordered_map>
#include "EventToken.h"
#include "Events.h"
#include "EventObserver.h"

struct EventHash {
 public:
  template <typename T>
  std::size_t operator()(const T &x) const {
    return static_cast<std::size_t>(x);
  }
};

class EventEqual {
 public:
  template <typename T>
  bool operator() (T const& t1, T const& t2) const {
    return t1 == t2;
  }
};

struct EventIDHash {
 public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const {
    return std::hash<T>()(x.first) ^ static_cast<std::size_t>(x.second);
  }
};

namespace uanc {
namespace util {
namespace event {

class EventToken;
class EventObserver;

using Cache = std::unordered_map<std::pair<int, Events>,
                                EventContainer, EventIDHash>;

/** \brief Simple event manager.
 *
 * This class represents the event manager and can be accessed via the singleton pattern
 * from all classes which are friends of this one.
 */
class EventManager {
  // Add the Event Publisher as friend.
  friend class EventToken;
  friend class EventObserver;

 public:
  /** \brief Basic destructor.
   *
   * This deconstructore destroys the registered instance.
   */
  ~EventManager();

  /** This method gets called to destroy the event manager. */
  static void destroy();

 private:
  /** Simple debug flag. */
  const bool DEBUG = true;

  /** \brief Basic constructor.
   *
   * This constructor fills the internal list with an initial value.
   */
  EventManager();

  /** \brief Token calls this method to trigger an event.
   *
   * This method can be called from the outside to trigger an event.
   *
   * @param publisher The publisher event token.
   * @param event The event to trigger.
   * @param data The data to supply for this trigger.
   */
  void trigger(EventToken *publisher, Events event, EventContainer data);

  /** \brief Shared pointer to the one and only instance of EventManager
   *
   * Holds a reference to the global event manager.
   */
  static EventManager* _instance;

  /**
    * Holds an unordered map over all events
    * (Event, id(=vector<int>))
    */
  std::unique_ptr<std::unordered_map<Events, std::vector<int>*,
                                     EventHash, EventEqual>> _eventMapping;

  /**
   * Holds an unordered map over all IDs
   * (ID, events(=vector<Events>))
   */
  std::unique_ptr<std::unordered_map<int, std::vector<Events>*>>
      _idEventMapping;
  /**
   * Holds an unorderd map over all IDs and EventObserver
   */
  std::unique_ptr<std::unordered_map<int, EventObserver*>> _idMapping;

  // init counter for id to zero
  int _idCounter = 0;

  /**
   * Simple cache for the last event
   */

  std::unique_ptr<Cache> cache;

  /** True, iff the event manger was already destroyed. */
  bool _destroyed = false;

  /** \brief Obtain the reference to the EventManager.
   *
   * Uses a classical singleton pattern to give back exactly the same copy of the event manager.
   * In addition it uses a shared pointer.
   *
   * @return The shared pointer containing the event manager
   */
  static EventManager* get();

  /** \brief Used to register an oberserver to an event.
   *
   * This method can be used to register an observer to register to events internally.
   *
   * @param event The event to listen to
   * @param observer The observer to listen to
   * @return The unique event publisher neccessary to publish messages.
   */
  EventToken* listen(EventObserver *observer);

  /** \brief Subsrcibe to an event.
   *
   * This method gets used to subscribe to an event. It can only be used by the corresponding token
   * because it is declared as friend.
   *
   * @param event The event to register.
   * @param token The token to register.
   */
  void subscribe(Events event, EventToken *token);


/** \brief Return the last event
  *
  * This method returns the last event for a given id
  * @param ID to check for the event
  * @param Event to check for on ID
  */
  EventContainer getLastEvent(int id, Events event);

  /** \brief Checks if there was a last event
  *
  * This method checks if there is a last event for a given ID
  * @param ID to check for the event
  * @param Event to check for on ID
  */
  bool hasLastEvent(int id, Events event);

  /** \brief Deletes the last event
   *
   * This method deletes the last event for a given ID
   * @param ID to delete the event
   * @param Event to delete from ID
   */
  void deleteLastEvent(int id, Events event);

  /** \brief Unregister token from the event manager.
   *
   * This method takes a token and degisters it from the event manager.
   * @param token The token to remove.
   */
  void unregister(int tokenid);
};

}  // namespace event
}  // namespace util
}  // namespace uanc

#endif  // CODE_UANC_UTIL_EVENT_EVENTMANAGER_H_
