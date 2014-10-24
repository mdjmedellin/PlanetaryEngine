#pragma once
#ifndef NAMED_PROPERTY_BASE_H
#define NAMED_PROPERTY_BASE_H

namespace gh
{
	class NamedPropertyBase
	{
		virtual NamedPropertyBase* clone() const = 0;
	};
}

#endif