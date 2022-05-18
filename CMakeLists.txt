cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(LMN-3-Emulator VERSION 0.1.0)

add_subdirectory(JUCE)            

juce_add_gui_app(LMN-3-Emulator
    # VERSION ...                       # Set this if the app version is different to the project version
    # ICON_BIG ...                      # ICON_* arguments specify a path to an image file to use as an icon
    # ICON_SMALL ...
    # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
    # COMPANY_NAME ...                  # Specify the name of the app's author
    PRODUCT_NAME "LMN-3-Emulator")      # The name of the final executable, which can differ from the target name


target_compile_features(LMN-3-Emulator  PRIVATE cxx_std_17)

target_sources(LMN-3-Emulator
    PRIVATE
        Source/Main.cpp
        Source/MainComponent.cpp
        Source/Encoder/Encoder.cpp
        Source/ButtonKeyboard/ButtonKeyboard.cpp
        Source/LookAndFeel/EmulatorLookAndFeel.cpp
)

target_include_directories(LMN-3-Emulator
    PUBLIC
        Source/
        Source/Encoder
        Source/ButtonKeyboard
        Source/LookAndFeel
)

target_compile_definitions(LMN-3-Emulator
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:LMN-3-Emulator,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:LMN-3-Emulator,JUCE_VERSION>")


juce_add_binary_data(FontData HEADER_NAME FontData.h NAMESPACE FontData SOURCES
    FontAwesome/FontAwesome6BrandsRegular400.otf
    FontAwesome/FontAwesome6FreeSolid900.otf
    FontAwesome/FontAwesome6FreeRegular400.otf
)


target_link_libraries(LMN-3-Emulator
    PRIVATE
        FontData
        juce::juce_gui_extra
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)