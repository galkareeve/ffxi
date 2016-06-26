#pragma once

#include <string>
#include <list>

#include "ISceneNodeAnimator.h"
#include "SceneManager.h"

class IMesh;
class IDriver;
class CSceneManager;
class ISceneNodeAnimator;
class ISceneNode
{
public:
	ISceneNode(ISceneNode *parent, CSceneManager *mgr) : m_Parent(parent), m_SceneManager(mgr), m_Position(glm::fvec3(0.f,0.f,0.f)), m_Rotation(glm::fvec3(0.f, 0.f, 0.f))
	{
		if (parent)
			parent->addChild(this);
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
		}
	}
	
	void addChild(ISceneNode *child) {
		m_Children.push_back(child);
	}

	void setName(std::string &name) { m_Name=name; }
	std::string getName() { return m_Name; }

	void setPosition(glm::fvec3 &tr) { m_Position =tr; }
	glm::fvec3& getPosition() { return m_Position; }

	void setRotation(glm::fvec3 &rt) { m_Rotation = rt; }
	glm::fvec3& getRotation() { return m_Rotation;  }

	void setVisible(bool isVisible) { m_IsVisible=isVisible; }
	bool isVisible() { return m_IsVisible; }

	//bool isPtinFrustum( glm::vec3 &p ) {
	//	return m_SceneManager->isPointInFrustum(p);
	//}

protected:
		//! Name of the scene node.
		std::string m_Name;

		//! Pointer to the parent
		ISceneNode* m_Parent;

		glm::fvec3 m_Position;
		glm::fvec3 m_Rotation;

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

