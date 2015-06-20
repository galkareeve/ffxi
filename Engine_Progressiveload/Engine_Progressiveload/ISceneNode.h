#pragma once

#include <string>
#include <list>
#include <glm/glm.hpp>

#include "IReferenceCounted.h"
#include "ISceneNodeAnimator.h"

class IMesh;
class IDriver;
class CSceneManager;
class ISceneNodeAnimator;
class ISceneNode : public IReferenceCounted
{
public:
	ISceneNode(ISceneNode *parent) : m_Parent(parent), m_RelativeTranslation(glm::fvec3(0,0,0)), m_RelativeRotation(glm::fvec3(0,0,0))
	{
		if (parent)
			parent->addChild(this);

		updateAbsolutePosition();
	};
	virtual ~ISceneNode(void){};

	virtual bool animate(int frame)=0;
	virtual void draw(IDriver *d, glm::mat4 &Pmat, glm::mat4 &Vmat )=0;
	virtual void addMesh(IMesh *in)=0;
	virtual IMesh* getMesh()=0;
	virtual void onAnimate(unsigned int timeMs) {
		if (m_IsVisible) {
			// animate this node with all animators
			auto ait = m_Animators.begin();
			while (ait != m_Animators.end()) {
				// continue to the next node before calling animateNode()
				// so that the animator may remove itself from the scene
				// node without the iterator becoming invalid
				ISceneNodeAnimator* anim = *ait;
				++ait;
				anim->animateNode(this, timeMs);
			}

			// update absolute position
			updateAbsolutePosition();
		}
	}
	
	//! Updates the absolute position based on the relative and the parents position
	/** Note: This does not recursively update the parents absolute positions, so if you have a deeper
		hierarchy you might want to update the parents first.*/
	virtual void updateAbsolutePosition()
	{
		if (m_Parent) {
			m_AbsoluteTransformation = m_Parent->getAbsoluteTransformation() * getRelativeTransformation();
		}
		else
			m_AbsoluteTransformation = getRelativeTransformation();
	}

	//! Get the absolute transformation of the node. Is recalculated every OnAnimate()-call.
	/** NOTE: For speed reasons the absolute transformation is not
	automatically recalculated on each change of the relative
	transformation or by a transformation change of an parent. Instead the
	update usually happens once per frame in OnAnimate. You can enforce
	an update with updateAbsolutePosition().
	\return The absolute transformation matrix. */
	virtual const glm::mat4x4& getAbsoluteTransformation() const
	{
		return m_AbsoluteTransformation;
	}

	//! Returns the relative transformation of the scene node.
	/** The relative transformation is stored internally as 3
	vectors: translation, rotation and scale. To get the relative
	transformation matrix, it is calculated from these values.
	\return The relative transformation matrix. */
	virtual glm::mat4x4 getRelativeTransformation() const
	{
		glm::mat4x4 mat;
//		mat.setRotationDegrees(RelativeRotation);
//		mat.setTranslation(RelativeTranslation);
		
		if (m_RelativeScale != glm::fvec3(1.f,1.f,1.f))
		{
			glm::mat4x4 smat;
	//		smat.setScale(m_RelativeScale);
			mat *= smat;
		}

		return mat;
	}

	void addChild(ISceneNode *child) {
		m_Children.push_back(child);
	}

	void setName(std::string &name) { m_Name=name; }
	std::string getName() { return m_Name; }

	void setRotation(glm::fvec3 &rot) { m_RelativeRotation=rot; }
	glm::fvec3& getRotation() { return m_RelativeRotation; }

	void setPosition(glm::fvec3 &tr) { m_RelativeTranslation=tr; }
	glm::fvec3& getPosition() { return m_RelativeTranslation; }

	void setVisible(bool isVisible) { m_IsVisible=isVisible; }
	bool isVisible() { return m_IsVisible; }

protected:
		//! Name of the scene node.
		std::string m_Name;

		//! Absolute transformation of the node. wrt to parent
		glm::mat4x4 m_AbsoluteTransformation;

		//! Relative translation of the scene node.
		glm::fvec3 m_RelativeTranslation;
		
		//! Relative rotation of the scene node.
		glm::fvec3 m_RelativeRotation;

		float m_direction;		//angle in radian
		//! Relative scale of the scene node.
		glm::fvec3 m_RelativeScale;

		//! Pointer to the parent
		ISceneNode* m_Parent;

		//! List of all children of this node
		std::list<ISceneNode*> m_Children;

		//! List of all animator nodes
		std::list<ISceneNodeAnimator*> m_Animators;

		//! Pointer to the scene manager
		CSceneManager* m_SceneManager;

		//! ID of the node.
		int m_ID;

		//! Is the node visible?
		bool m_IsVisible;
};

