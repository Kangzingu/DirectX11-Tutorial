#include "Keyboard.h"

Keyboard::Keyboard()
{
	for (int i = 0; i < 256; i++)
		m_keyStates[i] = false;
}
void Keyboard::OnPressed(const unsigned char key)
{
	m_keyStates[key] = true;
	m_keyBuffer.push(KeyboardEvent(KeyboardEvent::Type::Press, key));
}
void Keyboard::OnReleased(const unsigned char key)
{
	m_keyStates[key] = false;
	m_keyBuffer.push(KeyboardEvent(KeyboardEvent::Type::Release, key));
}
void Keyboard::OnCharInput(const unsigned char key)
{
	m_charBuffer.push(key);
}
KeyboardEvent Keyboard::ReadKey()
{
	if (m_keyBuffer.empty())
	{
		return KeyboardEvent();
	}
	else
	{
		KeyboardEvent e = m_keyBuffer.front();
		m_keyBuffer.pop();
		return e;
	}
}
unsigned char Keyboard::ReadChar()
{
	if (m_charBuffer.empty())
	{
		return 0u;
	}
	else
	{
		unsigned char e = m_charBuffer.front();
		m_charBuffer.pop();
		return e;
	}
}
bool Keyboard::IsPressed(const unsigned char keycode) { return m_keyStates[keycode]; }
bool Keyboard::IsKeyBufferEmpty() { return m_keyBuffer.empty(); }
bool Keyboard::IsCharBufferEmpty() { return m_charBuffer.empty(); }
bool Keyboard::IsKeysAutoRepeat() { return m_isKeyAutoRepeat; }
bool Keyboard::IsCharsAutoRepeat() { return m_isCharAutoRepeat; }