#include "meta_properties.h"
#include <algorithm>
#include <sstream>

RMetaProperties::RMetaProperties(const RCursor& cursor)
{
	ExtractProperties(cursor);
}

std::string RMetaProperties::GetProperty(const std::string& key) const
{
	auto it = _properties.find(key);

	if (it != _properties.end())
	{
		return it->second;
	}

	return "";
}

bool RMetaProperties::GetFlag(const std::string& key) const
{
	return _properties.find(key) != _properties.end();
}

std::string RMetaProperties::GetPropertyConstructor(const std::string& key) const
{
	auto it = _properties.find(key);
	if (it != _properties.end())
	{
		return it->first + "(" + it->second + ")";
	}

	return "";
}

void RMetaProperties::ExtractProperties(const RCursor& cursor)
{
	for (RCursor& child : cursor.GetChildren())
	{
		if (child.GetKind() != CXCursor_AnnotateAttr)
			continue;

		// Get the full meta data definition as a string.
		std::string allProperties = child.GetDisplayName();

		// Remove whitespace.
		allProperties.erase(std::remove_if(allProperties.begin(), allProperties.end(), isspace), allProperties.end());

		// Tokenize the properties
		TokenizedPropertyList tokens = TokenizePropertyList(allProperties);

		// Add properties 
		for (MetaPropertyPair prop : tokens)
		{
			_properties[prop.first] = prop.second;
		}
	}
}

TokenizedPropertyList RMetaProperties::TokenizePropertyList(const std::string& propertyList)
{
	// Initialize property list.
	TokenizedPropertyList out;

	if (propertyList.length() == 0)
	{
		return out;
	}

	char delimiter = ',';
	char openParenthesis = '(';
	char closeParenthesis = ')';

	uint32_t innerCount = 0;
	uint32_t start = 0;

	for (uint32_t i = 0; i < propertyList.size(); ++i)
	{
		char character = propertyList.at(i);

		if (character == delimiter && innerCount == 0)
		{
			MetaPropertyPair pair = CreatePairFromToken(propertyList.substr(start, i - start));
			if (!pair.first.empty())
			{
				out.push_back(pair);
			}

			start = i + 1;
		}

		if (character == openParenthesis)
		{
			innerCount++;
		}

		if (character == closeParenthesis)
		{
			innerCount--;
		}

		if (i == propertyList.size() - 1)
		{
			MetaPropertyPair pair = CreatePairFromToken(propertyList.substr(start, i - start + 1));
			if (!pair.first.empty())
			{
				out.push_back(pair);
			}
		}
	}

	// Return property list.
	return out;
}

MetaPropertyPair RMetaProperties::CreatePairFromToken(const std::string& token)
{
	MetaPropertyPair prop;
	
	size_t start = 0;
	size_t end = 0;

	// Find the starting parenthesis for arguments.
	if ((start = token.find_first_of('(', 0)) != std::string::npos) 
	{
		// Extract the key.
		prop.first = token.substr(0, start);

		if ((end = token.find_last_of(')')) != std::string::npos)
		{
			// if this contains actual information, use it as the value.
			if (end - start > 1)
			{
				prop.second = token.substr(start + 1, end - (start + 1));
			}
		}
	}
	else
	{
		// We didnt find an opening parenthesis therefore it should be a flag/default constructor.
		prop.first = token;
	}
	return prop;
}
