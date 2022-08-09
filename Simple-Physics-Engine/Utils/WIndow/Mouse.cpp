#include "Mouse.h"

void Mouse::OnLeftPressed(int x, int y)
{
	m_isLeftDown = true;
	m_eventBuffer.push(MouseEvent(MouseEvent::Type::LPress, x, y));
}
void Mouse::OnLeftReleased(int x, int y)
{
	m_isLeftDown = false;
	m_eventBuffer.push(MouseEvent(MouseEvent::Type::LRelease, x, y));
}
void Mouse::OnRightPressed(int x, int y)
{
	m_isRightDown = true;
	m_eventBuffer.push(MouseEvent(MouseEvent::Type::RPress, x, y));
}
void Mouse::OnRightReleased(int x, int y)
{
	m_isRightDown = false;
	m_eventBuffer.push(MouseEvent(MouseEvent::Type::RRelease, x, y));
}
void Mouse::OnWheelButtonPressed(int x, int y)
{
	m_isWheelButtonDown = true;
	m_eventBuffer.push(MouseEvent(MouseEvent::Type::MPress, x, y));
}
void Mouse::OnWheelButtonReleased(int x, int y)
{
	m_isWheelButtonDown = false;
	m_eventBuffer.push(MouseEvent(MouseEvent::Type::MRelease, x, y));
}
void Mouse::OnWheelUp(int x, int y)
{
	m_eventBuffer.push(MouseEvent(MouseEvent::Type::WheelUp, x, y));
}
void Mouse::OnWheelDown(int x, int y)
{
	m_eventBuffer.push(MouseEvent(MouseEvent::Type::WheelDown, x, y));
}
void Mouse::OnMouseMove(int x, int y)
{
	m_x = x;
	m_y = y;
	m_eventBuffer.push(MouseEvent(MouseEvent::Type::Move, x, y));
}
void Mouse::OnMouseMoveRaw(int x, int y)
{
	m_eventBuffer.push(MouseEvent(MouseEvent::Type::RAW_MOVE, x, y));
}
MouseEvent Mouse::ReadEvent()
{
	if (m_eventBuffer.empty())
	{
		return MouseEvent();
	}
	else
	{
		MouseEvent e = m_eventBuffer.front();
		m_eventBuffer.pop();
		return e;
	}
}
bool Mouse::IsEventBufferEmpty() { return m_eventBuffer.empty(); }
bool Mouse::IsLeftDown() { return m_isLeftDown; }
bool Mouse::IsWheelButtonDown() { return m_isWheelButtonDown; }
bool Mouse::IsRightDown() { return m_isRightDown; }
int Mouse::GetPositionX() { return m_x; }
int Mouse::GetPositionY() { return m_y; }
MousePoint Mouse::GetPosition() { return { m_x, m_y }; }