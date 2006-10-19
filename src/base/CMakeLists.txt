# Define the adonthell_base_SRCS variable containing all required files.
set(adonthell_base_SRCS
	base.cc
    callback.cc
	configuration.cc
	configio.cc
	diskio.cc
	diskwriter_gz.cc
	diskwriter_xml.cc
	file.cc
    flat.cc
	nls.cc
    paths.cc
    timer.cc
)


set(adonthell_base_HEADERS
	base.h
	diskio.h
	endians.h  
	hash_map.h  
	types.h
	callback.h   
	diskwriter_base.h
	file.h
	nls.h
	configio.h
	diskwriter_gz.h
	flat.h
	paths.h
	configuration.h
	diskwriter_xml.h
	gettext.h
	timer.h
)


# Add specific include directory for this library.
include_directories(${LIBXML2_INCLUDE_PATH})

# Create a shared library
add_library(adonthell_base SHARED ${adonthell_base_SRCS})


target_link_libraries(adonthell_base
	${LIBXML2_LIBRARIES} -lltdl)



#############################################
# Install Stuff
adonthell_install_lib(adonthell_base)
adonthell_install_include(base "${adonthell_base_HEADERS}") 