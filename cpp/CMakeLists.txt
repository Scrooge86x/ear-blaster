add_subdirectory(audiosystem)
target_sources(${PROJECT_NAME} PRIVATE
    main.cpp
    globalkeylistener.h globalkeylistener.cpp
    translator.h translator.cpp
)
