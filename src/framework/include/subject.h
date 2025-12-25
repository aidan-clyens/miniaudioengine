#ifndef __SUBJECT_H__
#define __SUBJECT_H__

#include <memory> // for std::shared_ptr, std::weak_ptr
#include <vector>
#include <mutex> // for std::mutex, std::lock_guard
#include <algorithm> // for std::remove_if

#include "observer.h"

namespace MinimalAudioEngine
{

/** @class Subject
 *  @brief The Subject class is part of the Observer design pattern.
 *  It maintains a list of observers and notifies them of changes.
 *  Observers can be attached or detached from the subject.
 *  @tparam T The type of data to be sent to observers upon notification.
 *  @note This class is thread-safe for attaching, detaching, and notifying observers.
 */
template <typename T>
class Subject
{
public:
  virtual ~Subject() = default; // Virtual destructor for proper cleanup of derived classes

  /** @brief Attaches an observer to the subject.
   *  @param observer A shared pointer to the observer to be attached.
   */
  void attach(std::shared_ptr<Observer<T>> observer)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_observers.push_back(observer);
  }

  /** @brief Detaches an observer from the subject.
   *  @param observer A shared pointer to the observer to be detached.
   */
  void detach(std::shared_ptr<Observer<T>> observer)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    // Remove the observer from the list of observers
    // using a weak pointer to avoid dangling references
    m_observers.erase(std::remove_if(m_observers.begin(), m_observers.end(),
                                      [&observer](const std::weak_ptr<Observer<T>>& weak_observer) {
                                        return weak_observer.lock() == observer;
                                      }), m_observers.end());
  }

  /** @brief Notifies all attached observers with the provided data.
   *  @param data The data to be sent to the observers.
   */
  void notify(const T& data)
  {
    cleanup_expired_observers(); // TODO - Maybe call this periodically instead?

    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& weak_observer : m_observers)
    {
      // Check if the observer is still valid
      // and call its update method if it is
      if (auto observer = weak_observer.lock())
      {
        observer->update(data);
      }
    }
  }

  /** @brief Cleans up expired observers from the list.
   *  This method removes any observers that have been destroyed
   *  and are no longer valid.
   *  @note Call this method periodically to maintain a clean list of observers.
   */
  void cleanup_expired_observers()
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_observers.erase(
      std::remove_if(m_observers.begin(), m_observers.end(),
        [](const std::weak_ptr<Observer<T>>& wp) { return wp.expired(); }),
      m_observers.end()
    );
  }

  /** @brief Get the count of attached observers.
   *  @return size_t The number of attached observers.
   */
  size_t get_observer_count() const
  {
    return m_observers.size();
  }

protected:
  Subject() = default; // Protected constructor to prevent direct instantiation

private:
	std::vector<std::weak_ptr<Observer<T>>> m_observers;
  mutable std::mutex m_mutex;
};

} // namespace MinimalAudioEngine

#endif  // __SUBJECT_H__