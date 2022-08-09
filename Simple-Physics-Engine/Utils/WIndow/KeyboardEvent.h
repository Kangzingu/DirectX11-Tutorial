#pragma once

class KeyboardEvent
{
public:
	enum Type
	{
		Press,
		Release,
		Invalid
	};

private:
	Type m_type;
	unsigned char m_key;

public:
	KeyboardEvent();
	KeyboardEvent(const Type type, const unsigned char key);

	bool IsPress() const;
	bool IsRelease() const;
	bool IsValid() const;

	unsigned char GetKeyCode() const;
};