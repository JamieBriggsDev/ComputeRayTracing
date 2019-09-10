#include "Timer.h"

// Include glfw for times
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

Timer* Timer::m_instance = nullptr;

Timer::Timer()
{
	// Set time to 0.0f
	glfwSetTime(0.0);
	// Get the current time.
	m_lastTime = glfwGetTime();
	// Set total frames to negative delay
	//  recording won't start until total frames reaches
	//  zero.
	m_totalFrames = -m_frameDelay;
	// Set finished flag to false
	m_finished = false;
}

Timer::~Timer()
{
	delete m_instance;
}

void Timer::TimeFrame()
{
	// Check if already finished
	if (m_finished)
		return;
	// increase the frame timer by 1
	m_totalFrames += 1;
	// Check if ready to start recording
	if (m_totalFrames == 0)
	{
		m_lastTime = glfwGetTime();
		m_frameTimes.resize(0);
	}
	// Check if frame delay has passed
	if (m_totalFrames > 0)
	{
		float currentTime = glfwGetTime();
		float frameTime = float(currentTime - m_lastTime);
		m_frameTimes.push_back(frameTime);
		m_lastTime = currentTime;
	}
	// Check if interval has finished
	if (m_totalFrames % m_frameInterval == 0 &&
		m_totalFrames > 0)
	{
		// Init average time
		float average = 0.0f;
		// Get every frame time in interval
		for (auto &time : m_frameTimes)
		{
			average += time;
		}
		// Find average
		average = average / m_frameInterval;
		// Empty times
		m_frameTimes.resize(0);
		Record(average);
		
	}
	// Check if finished
	if (m_totalFrames > m_totalRecordedFrames)
	{
		m_finished = true;
		// Record
		SaveToCSV();
		// Return function
		return;
	}

}

void Timer::Record(float _time)
{
	// Save recorded time
	m_recordedTimes.push_back(_time);
	std::cout << "Frames: " << m_totalFrames << "\tAvg Time: " << _time << std::endl;
}

void Timer::SaveToCSV()
{
	// Open file "Test.csv"
	std::ofstream myFile;
	myFile.open("Test.csv");
	myFile << "FrameInterval,Time\n";
	int interval = 0;
	// Place times
	for (auto &time : m_recordedTimes)
	{
		interval += m_frameInterval;
		myFile << interval << "," << time << ",\n";
	}
	// Close file
	myFile.close();
}

Timer * Timer::Instance()
{
	// Check if instance exists yet
	if (m_instance == nullptr)
		m_instance = new Timer();
	return m_instance;
}
