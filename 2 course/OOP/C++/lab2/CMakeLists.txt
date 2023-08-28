cmake_minimum_required(VERSION 3.1)

project(lab2)

set(SOURCES AbstractMode.cpp game.cpp
		CooperatorStrategy.cpp CopyCatStrategy.cpp DefectorStrategy.cpp 
		GrudgerStrategy.cpp SoftCopyCatStrategy.cpp RandomStrategy.cpp 
		Factory.cpp parser.cpp 
		main.cpp )



add_executable(run ${SOURCES})


