cmake_minimum_required(VERSION 3.15)

set(APP_NAME "LMN_3_MIDI_EMULATOR")

project(${APP_NAME} VERSION 0.0.1)

add_subdirectory(JUCE)            

juce_add_gui_app(${APP_NAME}
    # VERSION ...                       # Set this if the app version is different to the project version
    # ICON_BIG ...                      # ICON_* arguments specify a path to an image file to use as an icon
    # ICON_SMALL ...
    # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
    # COMPANY_NAME ...                  # Specify the name of the app's author
    PRODUCT_NAME "LMN-3 MIDI Emulator")     # The name of the final executable, which can differ from the target name

target_sources(${APP_NAME}
    PRIVATE
        Source/Main.cpp
        Source/MainComponent.cpp
        Source/Encoder/Encoder.cpp
        Source/ButtonKeyboard/ButtonKeyboard.cpp
        Source/LookAndFeel/EmulatorLookAndFeel.cpp
)

target_include_directories(${APP_NAME}
    PUBLIC
        Source/
        Source/Encoder
        Source/ButtonKeyboard
        Source/LookAndFeel
)

target_compile_definitions(${APP_NAME}
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${APP_NAME},JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${APP_NAME},JUCE_VERSION>")


juce_add_binary_data(FontData HEADER_NAME FontData.h NAMESPACE FontData SOURCES
    FontAwesome/FontAwesome6BrandsRegular400.otf
    FontAwesome/FontAwesome6FreeSolid900.otf
    FontAwesome/FontAwesome6FreeRegular400.otf
)


target_link_libraries(${APP_NAME}
    PRIVATE
        FontData
        juce::juce_gui_extra
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)