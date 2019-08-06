#pragma once
#include <string>
#include "type/type_id.h"
#include "type/type_data.h"
#include "metadata/metadata.h"
#include "invokable/function.h"
#include <unordered_map>
#include <memory>

namespace Raven
{
	namespace Meta
	{
		/*!\class RReflectionDatabase
		\brief Static class which holds information about registered types and allows reflection on these types.
		*/
		class RReflectionDatabase
		{
			friend class RType;
			friend class RTypeData;
		private:
			/*! Initialize the reflection database */
			RReflectionDatabase();
			~RReflectionDatabase();

			RReflectionDatabase(const RReflectionDatabase&) = delete; /*!< Delete copy constuctor since it doesn't make sense to copy the input manager instance. */
			RReflectionDatabase& operator= (const RReflectionDatabase&) = delete; /*!< Delete assignment constuctor since it doesn't make sense to overwrite the input manager. */

		public:
			/*! Get reference to the static reflection database object. */
			static RReflectionDatabase& Get() {
				static RReflectionDatabase db;
				return db;
			}

			/*! Allocate a type and register it's mappings. */
			FTypeID AllocateType(std::string typeName);

			/*! Deallocate a type (could be used for hot-reloading of modules) */
			bool DeallocType(std::string typeName);

			/*! Deallocate a type (could be used for hot-reloading of modules) */
			bool DeallocType(FTypeID typeId);

			/*! Retrieve id of a specified type. */
			FTypeID GetIdOfType(std::string typeName);

			/*! Add global variable with function getter and function setter. */
			template<typename GlobalType, typename GetterType, typename SetterType>
			void AddGlobalVariable(
				const std::string &name,
				GetterType getter,
				SetterType setter,
				const RMetaData::Initializer &meta
			);

			/*! Add global variable with function getter and raw setter. */
			template<typename GlobalType, typename GetterType>
			void AddGlobalVariable(
				const std::string &name,
				GetterType getter,
				GlobalType *globalSetter,
				const RMetaData::Initializer &meta
			);

			/*! Add global variable with raw getter and function setter. */
			template<typename GlobalType, typename SetterType>
			void AddGlobalVariable(
				const std::string &name,
				GlobalType *globalGetter,
				SetterType setter,
				const RMetaData::Initializer &meta
			);

			/*! Add global variable with raw getter and raw setter. */
			template<typename GlobalType>
			void AddGlobalVariable(
				const std::string &name,
				GlobalType *globalGetter,
				GlobalType *globalSetter,
				const RMetaData::Initializer &meta
			);

			/*! Add global function. */
			template<typename FunctionType>
			void AddGlobalFunction(
				const std::string &name,
				FunctionType globalFunction,
				const RMetaData::Initializer &meta
			);

			const RFunction &GetGlobalFunction(const std::string &name);

			const RFunction &GetGlobalFunction(
				const std::string &name,
				const RInvokableSignature &signature
			);

		protected:
			/*! Mapping of type name to type ID. */
			std::unordered_map<std::string, FTypeID> _ids;

			/*! Mapping of TypeID to type data.*/
			std::map<FTypeID, std::unique_ptr<RTypeData>> _types;

			/*! Global variables */
			std::unordered_map<std::string, RVariable> _globals;

			/*! Global functions */
			std::unordered_map<
				std::string,
				InvokableOverloadMap<RFunction>
			> _globalFunctions;

			/*! Internal typeID counter for allocation of types. */
			FTypeID _nextTypeID;
		};
	}
}

#include "reflection_database.hpp"