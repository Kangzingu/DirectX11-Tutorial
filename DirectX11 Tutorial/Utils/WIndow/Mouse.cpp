#include "Mouse.h"

void Mouse::OnLeftPressed(int x, int y) {
	this->leftIsDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::Type::LPress, x, y));
}

void Mouse::OnLeftReleased(int x, int y) {
	this->leftIsDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::Type::LRelease, x, y));
}

void Mouse::OnRightPressed(int x, int y) {
	this->rightIsDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::Type::RPress, x, y));
}

void Mouse::OnRightReleased(int x, int y) {
	this->rightIsDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::Type::RRelease, x, y));
}

void Mouse::OnMiddlePressed(int x, int y) {
	this->mbuttonDown = true;
	this->eventBuffer.push(MouseEvent(MouseEvent::Type::MPress, x, y));
}

void Mouse::OnMiddleReleased(int x, int y) {
	this->mbuttonDown = false;
	this->eventBuffer.push(MouseEvent(MouseEvent::Type::MRelease, x, y));
}

void Mouse::OnWheelUp(int x, int y) {
	this->eventBuffer.push(MouseEvent(MouseEvent::Type::WheelUp, x, y));
}

void Mouse::OnWheelDown(int x, int y) {
	this->eventBuffer.push(MouseEvent(MouseEvent::Type::WheelDown, x, y));
}

void Mouse::OnMouseMove(int x, int y) {
	this->x = x;
	this->y = y;
	this->eventBuffer.push(MouseEvent(MouseEvent::Type::Move, x, y));
}

void Mouse::OnMouseMoveRaw(int x, int y) {
	this->eventBuffer.push(MouseEvent(MouseEvent::Type::RAW_MOVE, x, y));
}

bool Mouse::IsLeftDown() {
	return this->leftIsDown;
}

bool Mouse::IsMiddleDown() {
	return this->mbuttonDown;
}

bool Mouse::IsRightDown() {
	return this->rightIsDown;
}

int Mouse::GetPosX() {
	return this->x;
}

int Mouse::GetPosY() {
	return this->y;
}

MousePoint Mouse::GetPos() {
	return { this->x, this->y };
}

bool Mouse::IsEventBufferEmpty() {
	return this->eventBuffer.empty();
}

MouseEvent Mouse::ReadEvent() {
	if (this->eventBuffer.empty()) {
		return MouseEvent();
	}
	else {
		MouseEvent e = this->eventBuffer.front();
		this->eventBuffer.pop();
		return e;
	}
}