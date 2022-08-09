#pragma once
typedef unsigned char BYTE;

class Color
{
public:
	Color();
	Color(unsigned int val);
	Color(BYTE r, BYTE g, BYTE b);
	Color(BYTE r, BYTE g, BYTE b, BYTE a);
	Color(const Color& src);

	Color& operator=(const Color& src);
	bool operator==(const Color& rhs) const;
	bool operator!=(const Color& rhs) const;

	constexpr BYTE GetR() const;
	void SetR(BYTE r);
	constexpr BYTE GetG() const;
	void SetG(BYTE g);
	constexpr BYTE GetB() const;
	void SetB(BYTE b);
	constexpr BYTE GetA() const;
	void SetA(BYTE a);

private:
	union
	{
		BYTE rgba[4];
		// 컬러값 비교할 때 매우 용이해서 int형을 넣었다고 함(아마 rgba 4개 각각이 아니라 하나의 정수로 비교하면 쉬우니깐 그런듯)
		unsigned int color;
	};
};

namespace Colors
{
	const Color UnloadedTextureColor(100, 100, 100);
	const Color UnhandledTextureColor(250, 0, 0);
}