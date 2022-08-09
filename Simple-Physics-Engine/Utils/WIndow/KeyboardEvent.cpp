#include "KeyboardEvent.h"

KeyboardEvent::KeyboardEvent() :m_type(Type::Invalid), m_key(0u) {}
KeyboardEvent::KeyboardEvent(const Type type, const unsigned char key) : m_type(type), m_key(key) {}
bool KeyboardEvent::IsPress() const { return m_type == Type::Press; }
bool KeyboardEvent::IsRelease() const { return m_type == Type::Release; }
bool KeyboardEvent::IsValid() const { return m_type != Type::Invalid; }
unsigned char KeyboardEvent::GetKeyCode() const { return m_key; }