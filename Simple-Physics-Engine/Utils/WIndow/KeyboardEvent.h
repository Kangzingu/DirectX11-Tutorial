#pragma once

class KeyboardEvent {
public:
	enum Type {
		Press,
		Release,
		Invalid
	};
	KeyboardEvent();
	KeyboardEvent(const Type type, const unsigned char key);
	bool IsPress() const;
	bool IsRelease() const;
	bool IsValid() const;
	unsigned char GetKeyCode() const;

private:
	Type type;
	unsigned char key;
};