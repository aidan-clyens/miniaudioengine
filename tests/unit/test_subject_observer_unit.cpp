#include <gtest/gtest.h>
#include <iostream>
#include <vector>

#include "subject.h" // for Subject
#include "observer.h" // for Observer

using namespace MinimalAudioEngine;

/** @class SubjectObserverTest
 *  @brief SubjectObserverTest fixture for Subject and Observer unit tests.
 *  This test suite verifies the functionality of the Subject and Observer classes
 *  including attaching, detaching, and notifying observers. 
 */
class SubjectObserverTest : public ::testing::Test
{
public:

  /** @class TestSubject
   *  @brief A test implementation of the Subject class for testing purposes.
   */
  class TestSubject : public Subject<int>
  {
  public:
    void trigger_event(int data)
    {
      notify(data);
    }
  };

  /** @class TestObserver
   *  @brief A test implementation of the Observer class for testing purposes.
   */
  class TestObserver : public Observer<int>
  {
    public:
      TestObserver() = default;
      explicit TestObserver(unsigned int id) : m_id(id) {}

      void update(const int& data) override
      {
        m_received_data.push_back(data);
      }

      void clear()
      {
        m_received_data.clear();
      }

      const std::vector<int>& get_received_data() const
      {
        return m_received_data;
      }

      const unsigned int id() const
      {
        return m_id;
      }

    private:
      std::vector<int> m_received_data;

      unsigned int m_id{0};
  };

  typedef std::vector<std::shared_ptr<TestObserver>> ObserverList;

  TestSubject &subject()
  {
    return m_subject;
  }

  ObserverList &observers()
  {
    return m_observers;
  }

protected:
  void SetUp() override
  {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  void TearDown() override
  {
    for (auto &observer : m_observers)
    {
      subject().detach(observer);
    }
    m_observers.clear();
    EXPECT_EQ(m_subject.get_observer_count(), 0) << "Subject should have 0 attached observers after teardown.";
  }

private:
  TestSubject m_subject;
  ObserverList m_observers;
};

/** @test Attach Observer to Subject
 *  This test verifies that an observer can be successfully attached to a subject.
 *  Steps:
  * 1. Initialize a list of observers.
  * 2. Attach each observer to the subject.
  * 3. Verify that the subject's observer count matches the number of attached observers.
 */
TEST_F(SubjectObserverTest, AttachObserver)
{
  const int TEST_VALUE = 42;
  const size_t OBSERVER_COUNT = 3;

  ObserverList &observers = this->observers();
  TestSubject &subject = this->subject();

  // Create and attach observers
  for (size_t i = 0; i < OBSERVER_COUNT; ++i)
  {
    auto observer = std::make_shared<TestObserver>(static_cast<unsigned int>(i));
    observers.push_back(observer);
    subject.attach(observer);
  }

  EXPECT_EQ(subject.get_observer_count(), OBSERVER_COUNT) << "Subject should have " << OBSERVER_COUNT << " attached observers.";
}

/** @test Notify Observers attached to a Subject
 *  This test verifies that all attached observers are notified correctly when the subject triggers an event. 
 *  Steps:
 *  1. Attach multiple observers to the subject.
 *  2. Trigger an event on the subject with test data.
 *  3. Verify that each observer received the correct notification data.
 */
TEST_F(SubjectObserverTest, NotifyObservers)
{
  const size_t OBSERVER_COUNT = 3;
  const int TEST_VALUE = 99;

  ObserverList &observers = this->observers();
  TestSubject &subject = this->subject();

  // Create and attach observers
  for (size_t i = 0; i < OBSERVER_COUNT; ++i)
  {
    auto observer = std::make_shared<TestObserver>(static_cast<unsigned int>(i));
    observers.push_back(observer);
    subject.attach(observer);
  }

  // Trigger event to notify observers
  subject.trigger_event(TEST_VALUE);

  // Verify each observer received the notification
  for (const auto &observer : observers)
  {
    const auto &received_data = observer->get_received_data();
    ASSERT_EQ(received_data.size(), 1) << "Observer " << observer->id() << " should have received 1 notification.";
    EXPECT_EQ(received_data[0], TEST_VALUE) << "Observer " << observer->id() << " should have received data: " << TEST_VALUE;
  }
}
