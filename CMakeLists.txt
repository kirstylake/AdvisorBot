cmake_minimum_required(VERSION 3.22)
project(AdvisorBot_Midterm)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

FILE(COPY 20200601.csv DESTINATION "${CMAKE_BINARY_DIR}")

add_executable(AdvisorBot_Midterm
        CSVReader.cpp
        CSVReader.h
        main.cpp
        MerkelMain.cpp
        MerkelMain.h
        OrderBookEntry.cpp
        OrderBookEntry.h
        OrderBook.cpp
        OrderBook.h
        )
