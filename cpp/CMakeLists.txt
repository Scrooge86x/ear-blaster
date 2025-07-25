target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    globalkeylistener.h globalkeylistener.cpp
    translator.h translator.cpp
)

add_subdirectory(audiosystem)
target_link_libraries(${PROJECT_NAME} PRIVATE AudioSystem)
