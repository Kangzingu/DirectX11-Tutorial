#include "MouseEvent.h"

MouseEvent::MouseEvent() : m_type(Type::Invalid), m_x(0), m_y(0) {}
MouseEvent::MouseEvent(Type type, int x, int y) : m_type(type), m_x(x), m_y(y) {}
bool MouseEvent::IsValid() const { return m_type != Type::Invalid; }
MouseEvent::Type MouseEvent::GetType() const { return m_type; }
MousePoint MouseEvent::GetPosition() const { return { m_x, m_y }; }
int MouseEvent::GetPositionX() const { return m_x; }
int MouseEvent::GetPositionY() const { return m_y; }