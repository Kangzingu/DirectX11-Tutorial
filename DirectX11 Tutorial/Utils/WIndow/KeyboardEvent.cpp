#include "KeyboardEvent.h"

KeyboardEvent::KeyboardEvent() :type(Type::Invalid), key(0u) {

}

KeyboardEvent::KeyboardEvent(const Type type, const unsigned char key) : type(type), key(key) {

}

bool KeyboardEvent::IsPress() const {
	return this->type == Type::Press;
}

bool KeyboardEvent::IsRelease() const {
	return this->type == Type::Release;
}

bool KeyboardEvent::IsValid() const {
	return this->type != Type::Invalid;
}

unsigned char KeyboardEvent::GetKeyCode() const {
	return this->key;
}