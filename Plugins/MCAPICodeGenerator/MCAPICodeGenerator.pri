# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Add-in.
# ------------------------------------------------------

# This is a reminder that you are using a generated .pro file.
# Remove it when you are finished editing this file.
message("You are running qmake on a generated .pro file. This may not work!")


HEADERS += ./CSourceWriter.h \
    ./MCAPIHeaderGenerator.h \
    ./MCAPIParser.h \
    ./MCAPICodeGenerator.h \
    ./MCAPICodeGeneratorPlugin.h \
    ../common/NameGenerationPolicy.h
SOURCES += ./CSourceWriter.cpp \
    ./MCAPICodeGenerator.cpp \
    ./MCAPICodeGeneratorPlugin.cpp \
    ./MCAPIHeaderGenerator.cpp \
    ./MCAPIParser.cpp \
    ../common/NameGenerationPolicy.cpp
RESOURCES += MCAPICodeGenerator.qrc
