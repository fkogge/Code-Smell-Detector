CC = g++
FLAGS = -c -Wall -Werror -pedantic -std=c++11
SRC_DIR = src
BUILD_DIR = build

EXECUTABLE = CodeSmellDetector
CODE_SMELL_DETECTOR_H = $(SRC_DIR)/CodeSmellDetector.h
CODE_SMELL_DETECTOR_CPP = $(SRC_DIR)/CodeSmellDetector.cpp
MAIN_CPP = $(SRC_DIR)/main.cpp

OBJECT_MAIN = main.o
OBJECT_CODE_SMELL_DETECTOR = CodeSmellDetector.o

$(EXECUTABLE): $(OBJECT_CODE_SMELL_DETECTOR) $(OBJECT_MAIN)
	$(CC) $(OBJECT_CODE_SMELL_DETECTOR) $(OBJECT_MAIN) -o $(EXECUTABLE)

$(OBJECT_CODE_SMELL_DETECTOR): $(CODE_SMELL_DETECTOR_CPP) $(CODE_SMELL_DETECTOR_H)
	$(CC) $(FLAGS) $(CODE_SMELL_DETECTOR_CPP)

$(OBJECT_MAIN): $(MAIN) $(CODE_SMELL_DETECTOR_H)
	$(CC) $(FLAGS) $(MAIN_CPP)