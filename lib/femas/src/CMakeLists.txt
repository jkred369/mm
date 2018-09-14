# target
file(GLOB_RECURSE Femas_SOURCES "*.cpp")

add_library(Femas SHARED 
	${Femas_SOURCES}
	${Common_HEADERS}
)

target_include_directories(Femas PUBLIC
	${Common_INCLUDES}
)