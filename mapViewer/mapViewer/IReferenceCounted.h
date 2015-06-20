#pragma once

class IReferenceCounted
{
public:

	//! Constructor.
	IReferenceCounted()
		: DebugName(0), ReferenceCounter(1)
	{
	}

	//! Destructor.
	virtual ~IReferenceCounted()
	{
	}

	void grab() const { ++ReferenceCounter; }

	bool drop() const
	{
		--ReferenceCounter;
		if (!ReferenceCounter)
		{
			delete this;
			return true;
		}

		return false;
	}

	//! Get the reference count.
	/** \return Current value of the reference counter. */
	int getReferenceCount() const
	{
		return ReferenceCounter;
	}

	//! Returns the debug name of the object.
	/** The Debugname may only be set and changed by the object
	itself. This method should only be used in Debug mode.
	\return Returns a string, previously set by setDebugName(); */
	const char* getDebugName() const
	{
		return DebugName;
	}

protected:
	//! Sets the debug name of the object.
	/** The Debugname may only be set and changed by the object
	itself. This method should only be used in Debug mode.
	\param newName: New debug name to set. */
	void setDebugName(const char* newName)
	{
		DebugName = newName;
	}

private:
	//! The debug name.
	const char* DebugName;

	//! The reference counter. Mutable to do reference counting on const objects.
	mutable int ReferenceCounter;
};
