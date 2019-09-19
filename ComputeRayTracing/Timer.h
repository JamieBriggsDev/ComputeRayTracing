#pragma once

#include <vector>

class Timer
{
private:
	// Timer instance
	static Timer* m_instance;

	// Timer variables
	int m_totalFrames = 0;
	// Time between intervals
	float m_intervalTime = 0.0f;
	// Total Time
	float m_lastTime = 0.0f;
	// Finished flag
	bool m_finished = false;
	// Individual Frame times in intervals
	std::vector<float> m_frameTimes;
	// Recorded Times
	std::vector<float> m_recordedTimes;

	// Consts
	// How many frames till time starts
	int m_frameDelay = 100;
	// How many frames for each recording
	int m_frameInterval = 25;
	// How many frames to record
	int m_totalRecordedFrames = 500;
	// Records time
	void Record(float _time);
	// Saves to a csv file
	void SaveToCSV();

public:
	// Gets the timer instance
	static Timer* Instance();

	// Update
	void TimeFrame();
	// Returns true if the timer is finished.
	bool IsFinished() { return m_finished; }

	Timer();
	~Timer();
};

