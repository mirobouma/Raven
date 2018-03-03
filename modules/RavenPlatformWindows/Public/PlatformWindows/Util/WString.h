#include <locale>
#include <codecvt>
#include <string>

namespace Raven {
	namespace Util {
		namespace String {
			inline std::wstring ConvertStringToWString(std::string str)
			{
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				std::wstring wide = converter.from_bytes(str);
				return wide;
			}

			inline std::string ConvertWStringToString(std::wstring str)
			{
				std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
				std::string narrow = converter.to_bytes(str);
				return narrow;
			}
		}
	}
}
