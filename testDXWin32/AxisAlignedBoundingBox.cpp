#include "pch.h"
#include "AxisAlignedBoundingBox.h"
#include "IEntity.h"


AxisAlignedBoundingBox::AxisAlignedBoundingBox()
{

}

AxisAlignedBoundingBox::AxisAlignedBoundingBox(IEntity * e) : ICollider(e)
{
	auto vertices = m_pEntity->getMesh()->getVertices();
	int vertexCount = m_pEntity->getMesh()->getVertexCount();
	m_maxPoint = { -FLT_MAX,-FLT_MAX };
	m_minPoint = { FLT_MAX,FLT_MAX };

	XMVECTOR temp;
	for (int i = 0; i < vertexCount; i++) {
		temp = XMLoadFloat3(&vertices[i].pos);
		if (XMVectorGetX(temp) < XMVectorGetX(m_minPoint)) m_minPoint = XMVectorSetX(m_minPoint, XMVectorGetX(temp));
		if (XMVectorGetX(temp) > XMVectorGetX(m_maxPoint)) m_maxPoint = XMVectorSetX(m_maxPoint, XMVectorGetX(temp));
		if (XMVectorGetY(temp) < XMVectorGetY(m_minPoint)) m_minPoint = XMVectorSetY(m_minPoint, XMVectorGetY(temp));
		if (XMVectorGetY(temp) > XMVectorGetY(m_maxPoint)) m_maxPoint = XMVectorSetY(m_maxPoint, XMVectorGetY(temp));
		if (XMVectorGetZ(temp) < XMVectorGetZ(m_minPoint)) m_minPoint = XMVectorSetZ(m_minPoint, XMVectorGetZ(temp));
		if (XMVectorGetZ(temp) > XMVectorGetZ(m_maxPoint)) m_maxPoint = XMVectorSetZ(m_maxPoint, XMVectorGetZ(temp));
	}
}


AxisAlignedBoundingBox::~AxisAlignedBoundingBox()
{
}

void AxisAlignedBoundingBox::update()
{
	auto worldMatrix = XMMatrixRotationRollPitchYawFromVector(m_pEntity->getRotation());
	worldMatrix *= XMMatrixTranslationFromVector(m_pEntity->getPosition());
	m_maxPoint = { -FLT_MAX,-FLT_MAX };
	m_minPoint = { FLT_MAX,FLT_MAX };

	XMVECTOR temp;
	for (int i = 0; i < 8; i++) {
		temp = m_boundingBox[i];
		temp = XMVector3Transform(temp, worldMatrix);
		if (XMVectorGetX(temp) < XMVectorGetX(m_minPoint)) m_minPoint = XMVectorSetX(m_minPoint, XMVectorGetX(temp));
		if (XMVectorGetX(temp) > XMVectorGetX(m_maxPoint)) m_maxPoint = XMVectorSetX(m_maxPoint, XMVectorGetX(temp));
		if (XMVectorGetY(temp) < XMVectorGetY(m_minPoint)) m_minPoint = XMVectorSetY(m_minPoint, XMVectorGetY(temp));
		if (XMVectorGetY(temp) > XMVectorGetY(m_maxPoint)) m_maxPoint = XMVectorSetY(m_maxPoint, XMVectorGetY(temp));
		if (XMVectorGetZ(temp) < XMVectorGetZ(m_minPoint)) m_minPoint = XMVectorSetZ(m_minPoint, XMVectorGetZ(temp));
		if (XMVectorGetZ(temp) > XMVectorGetZ(m_maxPoint)) m_maxPoint = XMVectorSetZ(m_maxPoint, XMVectorGetZ(temp));
	}
}

bool AxisAlignedBoundingBox::Collide(ICollider * rhs)
{
	AxisAlignedBoundingBox* _rhs = (AxisAlignedBoundingBox*)rhs;

	//Is obj1's max X greater than obj2's min X? If not, obj1 is to the LEFT of obj2
	if (XMVectorGetX(this->m_maxPoint) > XMVectorGetX(_rhs->m_minPoint))

		//Is obj1's min X less than obj2's max X? If not, obj1 is to the RIGHT of obj2
		if (XMVectorGetX(this->m_minPoint) < XMVectorGetX(_rhs->m_maxPoint))

			//Is obj1's max Y greater than obj2's min Y? If not, obj1 is UNDER obj2
			if (XMVectorGetY(this->m_maxPoint) > XMVectorGetY(_rhs->m_minPoint))

				//Is obj1's min Y less than obj2's max Y? If not, obj1 is ABOVE obj2
				if (XMVectorGetY(this->m_minPoint) < XMVectorGetY(_rhs->m_maxPoint))

					//Is obj1's max Z greater than obj2's min Z? If not, obj1 is IN FRONT OF obj2
					if (XMVectorGetZ(this->m_maxPoint) > XMVectorGetZ(_rhs->m_minPoint))

						//Is obj1's min Z less than obj2's max Z? If not, obj1 is BEHIND obj2
						if (XMVectorGetZ(this->m_minPoint) < XMVectorGetZ(_rhs->m_maxPoint))

							//If we've made it this far, then the two bounding boxes are colliding
							return true;

	//If the two bounding boxes are not colliding, then return false
	return false;
}
