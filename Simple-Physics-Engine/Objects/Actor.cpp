#include "Actor.h"

void Actor::Draw(Matrix4x4& viewProjectionMatrix)
{
	m_model.Draw(this->m_transform.GetWorldMatrix(), viewProjectionMatrix);
}
