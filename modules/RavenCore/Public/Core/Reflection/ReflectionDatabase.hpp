#pragma once

namespace Raven
{
	namespace Meta
	{
		template<typename GlobalType, typename GetterType, typename SetterType>
		void RReflectionDatabase::AddGlobalVariable(const std::string& name, GetterType getter, SetterType setter, const RMetaData::Initializer& meta)
		{
			typedef GlobalGetter<GlobalType, true> GlobalGetterType;
			typedef GlobalSetter<GlobalType, true> GlobalSetterType;

			RVariable global{
				name,
				typeof(GlobalType),
				!getter ? nullptr : new GlobalGetterType(
					reinterpret_cast<typename GlobalGetterType::Signature>(getter)
				),
				!setter ? nullptr : new GlobalSetterType(
					reinterpret_cast<typename GlobalSetterType::Signature>(setter)
				)
			};

			global._meta = meta;

			_globals.emplace(name, global);
		}

		///////////////////////////////////////////////////////////////////////

		template<typename GlobalType, typename GetterType>
		void RReflectionDatabase::AddGlobalVariable(const std::string &name, GetterType getter, GlobalType *globalSetter, const RMetaData::Initializer &meta)
		{
			typedef GlobalGetter<GlobalType, true> GlobalGetterType;
			typedef GlobalSetter<GlobalType, false> GlobalSetterType;

			RVariable global{
				name,
				typeof(GlobalType),
				!getter ? nullptr : new GlobalGetterType(
					reinterpret_cast<typename GlobalGetterType::Signature>(getter)
				),
				!globalSetter ? nullptr : new GlobalSetterType(globalSetter)
			};

			global._meta = meta;

			_globals.emplace(name, global);
		}

		///////////////////////////////////////////////////////////////////////

		template<typename GlobalType, typename SetterType>
		void RReflectionDatabase::AddGlobalVariable(const std::string &name, GlobalType *globalGetter, SetterType setter, const RMetaData::Initializer &meta)
		{
			typedef GlobalGetter<GlobalType, false> GlobalGetterType;
			typedef GlobalSetter<GlobalType, true> GlobalSetterType;

			RVariable global{
				name,
				typeof(GlobalType),
				!globalGetter ? nullptr : new GlobalGetterType(globalGetter),
				!setter ? nullptr : new GlobalSetterType(
					reinterpret_cast<typename GlobalSetterType::Signature>(setter)
				)
			};

			global._meta = meta;

			_globals.emplace(name, global);
		}

		///////////////////////////////////////////////////////////////////////

		template<typename GlobalType>
		void RReflectionDatabase::AddGlobalVariable(const std::string &name, GlobalType *globalGetter, GlobalType *globalSetter, const RMetaData::Initializer &meta)
		{
			typedef GlobalGetter<GlobalType, false> GlobalGetterType;
			typedef GlobalSetter<GlobalType, false> GlobalSetterType;

			RVariable global{
				name,
				typeof(GlobalType),
				!globalGetter ? nullptr : new GlobalGetterType(globalGetter),
				!globalSetter ? nullptr : new GlobalSetterType(globalSetter)
			};

			global._meta = meta;

			_globals.emplace(name, global);
		}

		template<typename FunctionType>
		void RReflectionDatabase::AddGlobalFunction(
			const std::string &name,
			FunctionType globalFunction,
			const RMetaData::Initializer &meta
		)
		{
			Function function(name, globalFunction);

			function._meta = meta;

			_globalFunctions[name].emplace(
				function.GetSignature(),
				function
			);
		}
	}
}
