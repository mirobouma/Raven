
/*!
\def RASSERT(expr)
\brief Assertion macro for debugging. On release builds this will be compiled to (void)0
*/
#ifndef NDEBUG
#define RASSERT(expr) \
    if (!(expr)) \
        Raven::RAssert::HandleAssert(__FILE__, __LINE__, #expr);
#else
#define RASSERT(expr) (void)0
#endif

namespace Raven {
	/*! 
	\class RAssert
	\brief Assertion helper class.
	*/
	class RAssert {
	public:
		/*! Static assertion handler function for when an assertion is thrown. */
		static void HandleAssert(const char* file, int line, const char* expression);
	};
}