************************
Raven Coding Conventions
************************

Folder Structure
------------------------
Raven is built using multiple modules, therefore it's important to follow a certain folder structure to keep include paths clean.

Modules
^^^^^^^
- Modules go in the ``modules`` directory. Each module must have the ``Raven`` prefix to identify that this is a part of the engine, and not third-party.
- Each module must follow the following structure (Replace "MyModule" with your own module name):

======================================  =================================
Path                                                Information
======================================  =================================
modules/RavenMyModule/include/MyModule  Public include headers go here.
modules/RavenMyModule/src/              Private source directory.
modules/RavenMyModule/CMakeLists.txt    CMake Module build configuration.
======================================  =================================

- Reference the CMake_ section when populating the CMakeLists.txt.
- New modules must be added to the root CMakeLists.txt using the ``project_add_module()`` macro.

Applications
^^^^^^^^^^^^
- Applications go in the ``applications`` directory.
- Each application should follow the following structure (Replace "MyApplication" with your own module name):
==================================================  =================================
Path                                                Information
==================================================  =================================
applications/RavenMyApplication/include/MyModule    Public include headers go here.
applications/RavenMyApplication/src/                Private source directory.
applications/RavenMyApplication/CMakeLists.txt      CMake Module build configuration.
==================================================  =================================

- Reference the CMake_ section when populating the CMakeLists.txt.
- New applications must be added to the root CMakeLists.txt using the ``project_add_application()`` macro.

Third-party libraries
^^^^^^^^^^^^^^^^^^^^^
- Third-party libraries go in the ``third-party`` directory.
- Reference the CMake_ section when populating the CMakeLists.txt.
- New third-party libraries must be added to the root CMakeLists.txt using the ``project_add_3rdparty()`` macro.

CMake
------------------------
Raven uses CMake to build all the libraries, executables and documentation; therefore you will have to maintain your CMakeLists.

CMake Rules
^^^^^^^^^^^
- When creating new modules, applications or adding external libraries, you will have to register these in the root CMakelists by using ``project_add_module()``, ``project_add_application()`` and ``project_add_3rdparty()`` respectively.
- The CMakeLists for modules should be writting according to the following template:
.. code-block:: cmake
	:linenos:

	#module definition
	set(MODULE_NAME "MyModule")

	set (SourceGroup_PublicIncludes
		"include/MyModule/SomeHeader.h"
	)
	source_group("Include" FILES ${SourceGroup_PublicIncludes})

	set (SourceGroup_Source
		"src/MyModule.cpp"
	)
	source_group("Source" FILES ${SourceGroup_Source})

	set (SOURCE
		${SourceGroup_Source}
		${SourceGroup_PublicIncludes}
	)

	add_library(${MODULE_NAME} STATIC ${SOURCE})
	target_link_libraries(${MODULE_NAME} PUBLIC RavenCore) #Add additional dependencies here, make sure to mark them as PRIVATE or PUBLIC dependencies.
	target_include_directories(${MODULE_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

Class naming conventions
------------------------

Classes have a prefix to identify what kind of type they are. Generally the following table is followed:

==========  ========  =============================
Type        Prefix    Example
==========  ========  =============================
Interface   ``I``     ``IPlatform, IEntity``
Structure   ``F``     ``FVector3, FArchive``
Class       ``R``     ``RPlatformWin32, RWindow``
Template    ``T``     ``TContainer<int>``
==========  ========  =============================