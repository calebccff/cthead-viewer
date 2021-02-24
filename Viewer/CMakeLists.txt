add_library (Viewer 
  File.cc
  View.cc
  View_renderFuncs.cc)

install (TARGETS Viewer DESTINATION bin)