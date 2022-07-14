#pragma once
struct MousePoint {
	int x;
	int y;
};

class MouseEvent {
public:
	enum Type {
		LPress,
		LRelease,
		RPress,
		RRelease,
		MPress,
		MRelease,
		WheelUp,
		WheelDown,
		Move,
		RAW_MOVE,
		Invalid
	};
private:
	Type type;
	int x;
	int y;
public:
	MouseEvent();
	MouseEvent(const Type type, const int x, const int y);
	bool IsValid() const;
	Type GetType() const;
	MousePoint GetPos() const;
	int GetPosX() const;
	int GetPosY() const;
};