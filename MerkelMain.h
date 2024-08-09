#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"


class MerkelMain
{
    public:
        MerkelMain();
        /** Call this to start the sim */
        void init();
    private: 
        void printMenu();
        void printHelp();
        void printMarketStats();
        void gotoNextTimeframe(int timeStep);
        void gotoPrevTimeframe(int timeStep);
        void processUserOption();
        void printHelpCmd(std::string &cmd); //print the help command
        void printMin(std::vector<std::string> const &tokenInput);
        void printMax(std::vector<std::string> const &tokenInput);
        void printAvg(std::vector<std::string> const &tokenInput);
        void printPredict(std::vector<std::string> const &tokenInput);
        void printSTD(std::vector<std::string> const &tokenInput);
        std::string currentTime;

//        OrderBook orderBook{"20200317.csv"};
	OrderBook orderBook{"20200601.csv"};

};
