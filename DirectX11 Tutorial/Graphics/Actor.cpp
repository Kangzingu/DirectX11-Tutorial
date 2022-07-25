#include "Actor.h"

void Actor::Draw(Matrix4x4& viewProjectionMatrix)
{
	model.Draw(this->transform.GetWorldMatrix(), viewProjectionMatrix);
}
