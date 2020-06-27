#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
   
        std::unique_lock<std::mutex> uLock(_mutex);
        _cond.wait(uLock, [this] { return !_messages.empty(); }); // pass unique lock to condition variable

        // remove last vector element from queue
        T msg = std::move(_messages.back());
        _messages.pop_back();

        return msg; // will not be copied due to return value optimization (RVO) in C++
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    
      
        // perform vector modification under the lock
        std::lock_guard<std::mutex> uLock(_mutex);

        // add vector to queue
        _messages.push_back(std::move(msg));
        _cond.notify_one(); // notify client after pushing new Vehicle into vector
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    
      int cycleDuration = 1; // duration of a single simulation cycle in ms

  std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // init stop watch
  lastUpdate = std::chrono::system_clock::now();
  while (true)
     {
    // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // compute time difference to stop watch
        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
     if( timeSinceLastUpdate >= cycleDuration)
     {  
       if (_Queue.receive()==TrafficLightPhase::green)
    	{
    	return ;
	 	 }
     }
    
  }
  
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
   
  threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this)) ;
  
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
   
 	 std::random_device dev;  // https://stackoverflow.com/a/13445752
    std::mt19937 rng(dev()); // Random number generator
    std::uniform_int_distribution<std::mt19937::result_type> distribution(4000, 6000); // dist betweeen 4000ms and 6000ms
    int lastCycleDuration = distribution(rng); // duration of a single simulation cycle in ms
  std::chrono::time_point<std::chrono::system_clock> lastUpdate;

    // init stop watch
  lastUpdate = std::chrono::system_clock::now();
  while(true)
  {
    // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        


     	long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >=lastCycleDuration)
        {
        _currentPhase=  _currentPhase ==TrafficLightPhase::red ?  TrafficLightPhase::green : TrafficLightPhase::red ;
         _Queue.send(std::move(_currentPhase));
          
          lastUpdate = std::chrono::system_clock::now();
          lastCycleDuration = distribution(rng); // Randomly update cycle duration

        }   
  }
}

