# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra

# Target executable
TARGET = Greedy_algo_Seam_Carving.exe

# Source files
SRCS = Greedy_algo_Seam_Carving.cpp

# QT lib
QT_DIR ?= ./dependencies/bin

# Object files
OBJS = $(SRCS:.cpp=.o)

ifeq ($(OS),Windows_NT)
    TARGET   := $(BUILD)/Greedy_algo_Seam_Carving.exe
    OBJS     := $(BUILD)/Greedy_algo_Seam_Carving.o
    INC_DIRS := -I. -Idependencies/include
    LIB_DIRS := -Ldependencies/lib -Ldependencies/bin -Ldependencies
    LIBS     := -lopencv_highgui4140 -lopencv_imgcodecs4140 -lopencv_imgproc4140 -lopencv_core4140
    RUN_CMD  := $(BUILD)\Greedy_algo_Seam_Carving.exe
else
    # Native Linux / WSL build (requires libopencv-dev installed in WSL)
    TARGET   := $(BUILD)/Greedy_algo_Seam_Carving
    OBJS     := $(BUILD)/Greedy_algo_Seam_Carving.o
    INC_DIRS := -I.
    LIB_DIRS :=
    CXXFLAGS += $(shell pkg-config --cflags opencv4)
    LIBS     := $(shell pkg-config --libs opencv4)
    RUN_CMD  := ./$(TARGET)
endif

DEPS := $(OBJS:.o=.d)

.PHONY: all run clean deploy_dlls

all: $(TARGET) deploy_dlls

# Link executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LIB_DIRS) $(LIBS) -o $@

# Compile .cpp -> .o (tracks header changes via .d files)
$(BUILD)/%.o: %.cpp
ifeq ($(OS),Windows_NT)
  ifneq (,$(findstring sh,$(notdir $(SHELL))))
	mkdir -p $(BUILD)
  else
	if not exist $(BUILD) mkdir $(BUILD)
  endif
else
	mkdir -p $(BUILD)
endif
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

# Copy OpenCV (and Qt) DLLs into build/ so the .exe can be launched directly
deploy_dlls: $(TARGET)
ifeq ($(OS),Windows_NT)
  ifneq (,$(findstring sh,$(notdir $(SHELL))))
	cp -u dependencies/bin/*.dll $(BUILD)/ 2>/dev/null || true
	cp -u dependencies/*.dll $(BUILD)/ 2>/dev/null || true
  else
	if exist dependencies\bin\*.dll xcopy /y /d /q dependencies\bin\*.dll $(BUILD)\ >nul
	if exist dependencies\*.dll xcopy /y /d /q dependencies\*.dll $(BUILD)\ >nul
  endif
  ifneq (,$(wildcard $(QT_DIR)/bin/windeployqt.exe))
	"$(QT_DIR)/bin/windeployqt.exe" --no-translations "$(BUILD)/opencv_highgui4140.dll" >nul
  else ifneq (,$(wildcard $(QT_DIR)/bin/windeployqt6.exe))
	"$(QT_DIR)/bin/windeployqt6.exe" --no-translations "$(BUILD)/opencv_highgui4140.dll" >nul
  endif
endif

run: all
	$(RUN_CMD)

clean:
ifeq ($(OS),Windows_NT)
  ifneq (,$(findstring sh,$(notdir $(SHELL))))
	rm -rf $(BUILD)
  else
	if exist $(BUILD) rmdir /s /q $(BUILD)
  endif
else
	rm -rf $(BUILD)
endif

-include $(DEPS)