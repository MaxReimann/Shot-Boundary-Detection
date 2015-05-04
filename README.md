# Shot-Boundary-Detection

STYLE GUIDE

-use forward declarations & (reference) pointers to reduce compile time

-member variables start with m_

-includes in specifies order: (classname.h), opencv, ownHeaders

-do not "inherit" includes, always include all necessary headers

-use reference counting whereever possible (cv::Ptr<>)

-use "#pragma once" include guard

use namespaces for modules

Naming

file names are all lowercase
classes start with capital letter
