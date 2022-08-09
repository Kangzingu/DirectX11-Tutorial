#pragma once
#include "KeyboardEvent.h"
#include <queue>

class Keyboard
{
private:
	bool m_keyStates[256];

	bool m_isKeyAutoRepeat = false;
	bool m_isCharAutoRepeat = false;

	std::queue<KeyboardEvent> m_keyBuffer;
	std::queue<unsigned char> m_charBuffer;

public:
	Keyboard();

	bool IsPressed(const unsigned char keycode);
	bool IsKeyBufferEmpty();
	bool IsCharBufferEmpty();

	KeyboardEvent ReadKey();
	unsigned char ReadChar();

	void OnPressed(const unsigned char key);
	void OnReleased(const unsigned char key);
	void OnCharInput(const unsigned char key);

	bool IsKeysAutoRepeat();
	bool IsCharsAutoRepeat();
};