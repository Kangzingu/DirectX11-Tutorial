#pragma once
#include "MouseEvent.h"
#include <queue>

class Mouse {
private:
	int m_x = 0;
	int m_y = 0;

	bool m_isLeftDown = false;
	bool m_isRightDown = false;
	bool m_isWheelButtonDown = false;

	float m_sensitivity = 0.0005f;

	std::queue<MouseEvent> m_eventBuffer;

public:
	int GetPositionX();
	int GetPositionY();
	MousePoint GetPosition();

	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);
	void OnWheelUp(int x, int y);
	void OnWheelDown(int x, int y);
	void OnWheelButtonPressed(int x, int y);
	void OnWheelButtonReleased(int x, int y);
	
	void OnMouseMove(int x, int y);
	void OnMouseMoveRaw(int x, int y);
	
	MouseEvent ReadEvent();
	bool IsEventBufferEmpty();

	bool IsLeftDown();
	bool IsWheelButtonDown();
	bool IsRightDown();

	float GetSensitivity();
	void SetSensitivity(float sensitivity);

};