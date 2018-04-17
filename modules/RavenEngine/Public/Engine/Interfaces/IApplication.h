#pragma once

/*! Application interface. */
class IApplication 
{
	/*! Initialization code. */
	virtual void Initialize() = 0;

	/*! Update code.*/
	virtual void Update() = 0;

	/*! Destroy application.*/
	virtual void Destroy() = 0;
};