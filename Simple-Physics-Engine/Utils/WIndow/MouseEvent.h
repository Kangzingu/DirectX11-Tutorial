#pragma once
struct MousePoint
{
	int m_x;
	int m_y;
};

class MouseEvent
{
public:
	enum Type
	{
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
	int m_x;
	int m_y;
	Type m_type;

public:
	MouseEvent();
	MouseEvent(const Type type, const int x, const int y);
	bool IsValid() const;
	Type GetType() const;
	MousePoint GetPosition() const;
	int GetPositionX() const;
	int GetPositionY() const;
};