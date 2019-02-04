#pragma once
#include "Cursor.h"
#include <unordered_map>
#include <string>
#include <vector>

typedef std::vector<std::pair<std::string, std::string>> TokenizedPropertyList;
typedef std::pair<std::string, std::string> MetaPropertyPair;

class RMetaProperties {
public:

	RMetaProperties(const RCursor& cursor);
	~RMetaProperties() {}
	
	// Get property in meta flags.
	std::string GetProperty(const std::string &key) const;

	// Check if a property is set for this type.
	bool GetFlag(const std::string &key) const;

	// Get converts the key-pair value back to a constructor.
	std::string GetPropertyConstructor(const std::string &key) const;

protected:
	
	// Extract properties from the specified cursor
	void ExtractProperties(const RCursor& cursor);

	// Tokenize a string of meta properties and add these properties into the map.
	TokenizedPropertyList TokenizePropertyList(const std::string& propertyList);

	// Create meta property from token by extracting contents between ()
	MetaPropertyPair CreatePairFromToken(const std::string& token);

	// Map of properties.
	std::unordered_map<std::string, std::string> _properties;
};