#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <algorithm>
#include <cmath> //used to calculate standard deviation

MerkelMain::MerkelMain()
{
}

void MerkelMain::init()
{
    std::cout << "Welcome, I am Advisorbot. "
                 "I am command a line program that can help you with cryptocurrency investing analysis"
              << std::endl;
    std::cout << "Please enter \"help\" to to see all available commands" << std::endl;

    currentTime = orderBook.getEarliestTime();
    std::cout << "Current Time is " << currentTime << std::endl;

    while (true)
    {
        printMenu();
        processUserOption();
    }
}

//==============PRINT MENUS AND HELP =================================================================
void MerkelMain::printMenu()
{
    // Changed these from Merkelrex to reflect the new Advisorbot commands

    // list all available help commands
    std::cout << "Help " << std::endl;
    // Output help for the specified command
    std::cout << "Help <cmd>" << std::endl;
    // List available products
    std::cout << "Prod" << std::endl;
    // Find minimum bid or ask for the product in current time step
    std::cout << "Min " << std::endl;
    // Find maximum bid or ask for product in current time step
    std::cout << "Max " << std::endl;
    // Compute Average ask or bid for the sent product over the sent number of time steps
    std::cout << "Avg" << std::endl;
    // Predict max or min ask or bid for the sent product for the next time step
    std::cout << "Predict" << std::endl;
    // Calculate standard deviation
    std::cout << "STD" << std::endl;
    // State current time in dataset, i.e. which timeframe we are looking at
    std::cout << "Time" << std::endl;
    // Move to next time step
    std::cout << "Step" << std::endl;
    std::cout << "============== " << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << " - All available commands: " << std::endl;
    std::cout << "help, help <cmd>, prod, min, max, avg, predict, STD, time, "
                 "step <parameter1>, step <parameter1> <parameter2>"
              << std::endl;
}

void MerkelMain::printHelpCmd(std::string &cmd)
{
    if (cmd == "HELP")
    {
        std::cout << cmd << " - List all available help commands" << std::endl;
        std::cout << "Format: help" << std::endl;
        std::cout << "Example: help" << std::endl;
        return;
    }

    if (cmd == "PROD")
    {
        std::cout << cmd << " - List available products" << std::endl;
        std::cout << "Format: prod" << std::endl;
        std::cout << "Example: prod" << std::endl;
        return;
    }

    if (cmd == "MIN")
    {
        std::cout << cmd << " - Find minimum bid or ask for the product in current time step" << std::endl;
        std::cout << "Format: min _product bid/ask" << std::endl;
        std::cout << "Example: min ETH/BTC ask" << std::endl;
        return;
    }

    if (cmd == "MAX")
    {
        std::cout << cmd << " - Find maximum bid or ask for product in current time step" << std::endl;
        std::cout << "Format: max _product bid/ask" << std::endl;
        std::cout << "Example: max ETH/BTC ask" << std::endl;
        return;
    }

    if (cmd == "AVG")
    {
        std::cout << cmd << " - Compute Average ask or bid for the sent product over the sent number of time steps" << std::endl;
        std::cout << "Format: avg _product bid/ask _numberOfTimeSteps" << std::endl;
        std::cout << "Example: avg ETH/BTC ask 10" << std::endl;
        return;
    }

    if (cmd == "PREDICT")
    {
        std::cout << cmd << " - Predict max or min ask or bid for the sent product for the next time step" << std::endl;
        std::cout << "Format: predict max/min _product bid/ask" << std::endl;
        std::cout << "Example: predict max ETH/BTC ask" << std::endl;
        return;
    }

    if (cmd == "TIME")
    {
        std::cout << cmd << " - State current time in dataset, i.e which timeframe we are looking at" << std::endl;
        std::cout << "Format: time" << std::endl;
        std::cout << "Example: time" << std::endl;
        return;
    }

    if (cmd == "STEP")
    {
        std::cout << "There are two versions of this command" << std::endl;
        std::cout << "This defaults for one time step" << std::endl;
        std::cout << "Format: step forth/back" << std::endl;
        std::cout << "Example: step forth" << std::endl;

        std::cout << "This allows the user to move forward ot backwards with a chunk of time steps" << std::endl;
        std::cout << "Format: step forward/back _numberOfTimeStamps" << std::endl;
        std::cout << "Example: step forward 5" << std::endl;
        return;
    }
    if (cmd == "STD")
    {
        std::cout << "This allows the user to see the standard deviation for bid/buy for the current time step" << std::endl;
        std::cout << "Format: STD product bid/ask" << std::endl;
        std::cout << "Example: STD ETH/BTC bid" << std::endl;
        return;
    }

    std::cout << "MerkelMain::printHelpCmd Unknown command" << std::endl;
}

void MerkelMain::printMarketStats()
{
    std::vector<std::string> products = orderBook.getKnownProducts(currentTime);
    std::cout << "Products found: " << std::endl;
    for (std::string const &s : products)
    {
        std::cout << "Product: " << s << std::endl;
    }
}

//============================ PRINT MIN =============================================================

void MerkelMain::printMin(const std::vector<std::string> &tokenInput)
{
    //Print the minimum amount for either ask or bid for the current time step
    std::string product = tokenInput[1];
    std::vector<OrderBookEntry> entries;
    OrderBookType orderType;

    std::vector<std::string> allProducts = orderBook.getKnownProducts(currentTime);
    // iterate through all products to find if one matches our token
    for (std::string &s : allProducts)
    {
        if (s == product)
        {
            break;
        }
        // if no product is found, and we are also at the last record then tell the user the product is not in the timeframe
        if (s != product && s == allProducts.back())
        {
            std::cout << "MerkelMain::printMin no matching product for current time" << std::endl;
            return;
        }
    }

    // now that we can see the product exists, we can assign the order type to a variable
    orderType = OrderBookEntry::stringToOrderBookType(tokenInput[2]);
    // check if the order type is one of the known types
    if (orderType == OrderBookType::unknown)
    {
        std::cout << "MerkelMain::printMin unknowns order orderType" << std::endl;
        return;
    }

    // saving all match with parameters orders
    entries = orderBook.getOrders(orderType, product, currentTime);
    if (entries.empty())
    {
        std::cout << "MerkelMain::printMin No matching orders found" << std::endl;
        return;
    }

    // printing number of matches
    if (orderType == OrderBookType::ask)
    {
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Min ask: ";
    }
    else
    {
        std::cout << "Bids seen: " << entries.size() << std::endl;
        std::cout << "Min bid: ";
    }

    // printing the lowest price
    std::cout << OrderBook::getLowPrice(entries) << std::endl;
}

//============================ PRINT MAX ==============================================================

void MerkelMain::printMax(std::vector<std::string> const &tokens)
{
    //print the max amount for a product for the current time step
    OrderBookType orderType;
    std::string product = tokens[1];
    std::vector<OrderBookEntry> entries;

    // check for existence of the product
    std::vector<std::string> listOfProducts = orderBook.getKnownProducts(currentTime);
    for (std::string &s : listOfProducts)
    {
        if (s == product)
        {
            break;
        }

        if (s != product && s == listOfProducts.back())
        {
            std::cout << "MerkelMain::printMax no matching product for current time" << std::endl;
            return;
        }
    }

    // assigning value "orderType" variable
    orderType = OrderBookEntry::stringToOrderBookType(tokens[2]);
    if (orderType == OrderBookType::unknown)
    {
        std::cout << "MerkelMain::printMax unknowns order orderType" << std::endl;
        return;
    }

    // saving all match with parameters orders
    entries = orderBook.getOrders(orderType, product, currentTime);
    if (entries.empty())
    {
        std::cout << "MerkelMain::printMax No matching orders found" << std::endl;
        return;
    }

    // printing number of matches
    if (orderType == OrderBookType::ask)
    {
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: ";
    }
    else
    {
        std::cout << "Bids seen: " << entries.size() << std::endl;
        std::cout << "Max bid: ";
    }

    // printing the highest price
    std::cout << OrderBook::getHighPrice(entries) << std::endl;
}

//============================ PRINT AVERAGE ===========================================================

void MerkelMain::printAvg(const std::vector<std::string> &tokenInput)
{
    //print the average amount for each time step before the current time step according to the
    //number of time steps specified
    OrderBookType orderType;
    std::string product = tokenInput[1];
    int numberOfTimeStamps = 0;

    // assigning value to "value" variable
    orderType = OrderBookEntry::stringToOrderBookType(tokenInput[2]);
    if (orderType == OrderBookType::unknown)
    {
        std::cout << "MerkelMain::printAvg unknowns order orderType" << std::endl;
        return;
    }

    // assigning value to "numberOfTimeStamps" variable
    try
    {
        numberOfTimeStamps = std::stoi(tokenInput[3]);
    }
    catch (const std::exception &e)
    {
        std::cout << "MerkelMain::printAvg Bad integer " << tokenInput[3] << std::endl;
        return;
    }

    std::string timeStep = currentTime;
    double totalAvg = 0;

    // moving to the prev timestamps and adding their avg to totalAvg
    for (int i = 0; i < numberOfTimeStamps; i++)
    {
        std::cout << product << std::endl;

        std::vector<OrderBookEntry> entries = orderBook.getOrders(orderType, product, timeStep);

        // If timeStamp is empty, it means the previous timeStamp was the first and no timeStamp left to check.
        if (timeStep.empty())
        {
            std::cout << "MerkelMain::printAvg reached the first timestamp" << std::endl;
            if (i == 0)
            {
                std::cout << "Impossible to return average price, because current timestamp is the first" << std::endl;
            }
            else
            {
                std::cout << "Total average price from " << i << " timestamps: " << totalAvg / i << std::endl;
            }
            return;
        }

        // print if orders are not found on current timestamp
        if (entries.empty())
        {
            std::cout << i + 1 << ") Orders aren't found at " << timeStep << " timestamp" << std::endl;
            timeStep = orderBook.getPrevTime(timeStep);
            continue;
        }

        double avg = OrderBook::getAvgPrice(entries);

        std::cout << i + 1 << ") Average price at " << timeStep << ": " << avg << std::endl;
        totalAvg += avg;
        timeStep = orderBook.getPrevTime(timeStep);
    }

    // print total average price
    std::cout << "Total average price: " << totalAvg / numberOfTimeStamps << std::endl;
}

// ===========================PRINT PREDICTION ==========================================================

void MerkelMain::printPredict(const std::vector<std::string> &tokenInput)
{
    // change name of variable to "measurement" or "unit"
    std::string calculate;
    std::string product = tokenInput[2];
    OrderBookType orderType;

    // checking if the MAX/MIN has been added and assigning it to a variable
    if (tokenInput[1] == "MAX")
    {
        calculate = "MAX";
    }
    else if (tokenInput[1] == "MIN")
    {
        calculate = "MIN";
    }
    else
    {
        std::cout << "MerkelMain::printPrediction please add MIN/MAX to the correct position" << std::endl;
        return;
    }

    // Getting the order book type from the tokenized input
    orderType = OrderBookEntry::stringToOrderBookType(tokenInput[3]);
    if (orderType == OrderBookType::unknown)
    {
        std::cout << "MerkelMain::printAvg unknown orderType (please use BID or ASK)" << std::endl;
        return;
    }

    std::string timeStamp = currentTime;
    std::vector<OrderBookEntry> entries;
    double prevPrice = -1;
    double curPrice = -1;
    int i = 0;
    // finding previous and current price of the product
    while (i < 1000)
    {
        // If timeStamp is empty, it means the previous timeStamp was the first and no timeStamp left to check.
        // Also, if while loop was not interrupt then, prevPrice or curPrice are not assigned and not enough information
        // collected to make prediction.
        if (timeStamp.empty())
        {
            std::cout << "MerkelMain::printPrediction reached the first timestamp" << std::endl;
            std::cout << "No prediction can be made because "
                         "there is not enough information about this product on the previous timestamps"
                      << std::endl;
            return;
        }

        entries = orderBook.getOrders(orderType, product, timeStamp);
        // skipping current timeStamp if entries is empty.
        if (entries.empty())
        {
            timeStamp = orderBook.getPrevTime(timeStamp);
            i++;
            continue;
        }

        // assigning values to curPrice and prevPrice variables
        if (curPrice == -1)
        {
            if (calculate == "MAX")
            {
                curPrice = OrderBook::getHighPrice(entries);
            }
            else if (calculate == "MIN")
            {
                curPrice = OrderBook::getLowPrice(entries);
            }
            i = 0;
        }
        else if (prevPrice == -1)
        {
            if (calculate == "MAX")
            {
                prevPrice = OrderBook::getHighPrice(entries);
            }
            else if (calculate == "MIN")
            {
                prevPrice = OrderBook::getLowPrice(entries);
            }
            break;
        }

        timeStamp = orderBook.getPrevTime(timeStamp);
        i++;
    }

    // if no previous price was found, print exception for user
    if (prevPrice == -1)
    {
        std::cout << "MerkelMain::printPrediction Sorry, but advisorBot couldn't make prediction" << std::endl;
        std::cout << "The incorrect product could have been entered, or there are no entries" << tokenInput[3];
        std::cout << " in the csv file, for product you entered" << std::endl;
        return;
    }

    // print answer
    std::cout << "Previous price: " << prevPrice << " "
              << "Current price " << curPrice << std::endl;
    std::cout << "Prediction: new " << calculate + " " << product + " " << tokenInput[3] + " "
              << "price might be ";
    std::cout << ((curPrice - prevPrice) * -2 / 10) + curPrice << std::endl;
}

//========================= PRINT Standard Deviation ====================================================

void MerkelMain::printSTD(const std::vector<std::string> &tokenInput) {

    OrderBookType orderType;
    std::string product = tokenInput[1];

    orderType = OrderBookEntry::stringToOrderBookType(tokenInput[2]);
    if (orderType == OrderBookType::unknown) {
        std::cout << "MerkelMain::printSTD unknowns order orderType" << std::endl;
        return;
    }

        std::string timeStep = currentTime;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(orderType, product, timeStep);

        // print if orders are not found on current timestamp
        if (entries.empty()) {
            std::cout << "Orders aren't found at " << timeStep << " timestamp" << std::endl;
            return;
        }
        //Get the average amount for the current time step
        double totalAvg = OrderBook::getAvgPrice(entries);
        float mean = totalAvg;
        float standardDeviation = 0.0;

    for(int i = 0; i < entries.size(); i++)
    {
        standardDeviation += pow(entries[i].amount - mean, 2);
    }

    std::cout << "The standard deviation is " << sqrt(standardDeviation / entries.size()) << " for the current time step" << std::endl;
    }

//========================== Go To Next Timeframe =======================================================
void MerkelMain::gotoNextTimeframe(int timeStep)
{
    /*
     * Since I have changed OrderBook::matchAsksToBids function, so it returns sales of all products and also changed
     * how MerkelMain::gotoNextTimeframe prints them
     */
    /*
     * And to make it more clear for the user, I changed OrderBook::getNextTime so MerkelMain::gotoNextTimeframe loop
     * to the first timestamp if it reaches the final timestamp
     */
    std::cout << "Going to the next time frame. " << std::endl;

    std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(currentTime);
    std::cout << "Sales: " << sales.size() << std::endl;

    std::string currentProduct = "";
    for (OrderBookEntry &e : sales)
    {
        if (currentProduct != e.product)
        {
            currentProduct = e.product;
            std::cout << currentProduct << std::endl;
        }

        std::cout << "Sale price: " << e.price << " amount: " << e.amount << std::endl;
    }

    for(int i = 0; i < timeStep; i++ )
    {
        currentTime = orderBook.getNextTime(currentTime);
    }

    if (currentTime.empty())
    {
        std::cout << "MerkelMain::gotoNextTimeframe reached the last timestamp" << std::endl;
        std::cout << "Current time will be set to the first timestamp" << std::endl;
        currentTime = orderBook.setCurrentTime(1);
    }
    std::cout << "new timestamp: " << currentTime << std::endl;
}

//========================== Go To Previous Timeframe ===================================================
void MerkelMain::gotoPrevTimeframe(int timeStep)
{
    //This allows users to jump to the previous time frame and skip timeframes if needed
    std::cout << "Going to the previous time frame." << std::endl;
    for(int i = 0; i < timeStep; i++ )
    {
        currentTime = orderBook.getPrevTime(currentTime);
    }
    if (currentTime.empty())
    {
        std::cout << "MerkelMain::gotoNextTimeframe reached the first timestamp" << std::endl;
        std::cout << "Current time will be set to the last timestamp" << std::endl;
        currentTime = orderBook.setCurrentTime(orderBook.getEntriesSize());
    }
    std::cout << "new timestamp: " << currentTime << std::endl;
}

//========================== Process User Option ========================================================
void MerkelMain::processUserOption()
{  // changed this to work with Advisorbot strings
    std::string userInput;
    std::getline(std::cin, userInput);
    std::vector<std::string> tokenInput = CSVReader::tokenise(userInput, ' ');

    // convert user input to uppercase
    // The std::size function is only available for C++ 17 and higher
    for (int i = 0; i < std::size(tokenInput); i++)
    {
        std::for_each(tokenInput[i].begin(), tokenInput[i].end(), [](char &c)
                      { c = ::toupper(c); });
    }

    if ((tokenInput[0] == "HELP") && tokenInput.size() == 1)
    {
        printHelp();
        return;
    }
    if ((tokenInput[0] == "HELP") && tokenInput.size() == 2)
    {
        printHelpCmd(tokenInput[1]);
        return;
    }
    if ((tokenInput[0] == "PROD") && tokenInput.size() == 1)
    {
        printMarketStats();
        return;
    }
    if ((tokenInput[0] == "MIN") && tokenInput.size() == 3)
    {
        // convert all "ask" and "bid" to lower case, so they match the database
        std::for_each(tokenInput[2].begin(), tokenInput[2].end(), [](char &c)
                      { c = ::tolower(c); });
        std::cout << tokenInput[2] << std::endl;
        printMin(tokenInput);
        return;
    }
    if ((tokenInput[0] == "MAX") && tokenInput.size() == 3)
    {
        // convert all "ask" and "bid" to lower case, so they match the database
        std::for_each(tokenInput[2].begin(), tokenInput[2].end(), [](char &c)
                      { c = ::tolower(c); });
        printMax(tokenInput);
        return;
    }
    if ((tokenInput[0] == "AVG") && tokenInput.size() == 4)
    {
        std::for_each(tokenInput[2].begin(), tokenInput[2].end(), [](char &c)
                      { c = ::tolower(c); });
        printAvg(tokenInput);
        return;
    }
    if ((tokenInput[0] == "PREDICT") && tokenInput.size() == 4)
    {
        std::for_each(tokenInput[3].begin(), tokenInput[3].end(), [](char &c)
                      { c = ::tolower(c); });
        printPredict(tokenInput);
        return;
    }
    if (tokenInput[0] == "TIME")
    {
        std::cout << currentTime << std::endl;
        return;
    }

    //If no amount of time steps is specified then just standardize this to 1
    if (tokenInput[0] == "STEP")
    {
        if (tokenInput[1] == "FORWARD")
        {
            gotoNextTimeframe(1);
        }
        else if (tokenInput[1] == "BACK")
        {
            gotoPrevTimeframe(1);
        }
        else
        {
            std::cout << "MerkelMain::processUserInput wrong first parameter for step function" << std::endl;
        }
        return;
    }

    if ((tokenInput[0] == "STEP") && tokenInput.size() == 3)
    {
        //change the time step to an integer
        int timeStep = std::stoi(tokenInput[2]);
        if (tokenInput[1] == "FORWARD")
        {
            gotoNextTimeframe(timeStep);
        }
        else if (tokenInput[1] == "BACK")
        {
            gotoPrevTimeframe(timeStep);
        }
        else
        {
            std::cout << "MerkelMain::processUserInput wrong first parameter for step function" << std::endl;
        }
        return;
    }
    if (tokenInput[0] == "STD" && tokenInput.size() == 3)
    {
        //convert bid/buy to lowercase
        std::for_each(tokenInput[2].begin(), tokenInput[2].end(), [](char &c)
        { c = ::tolower(c); });
        printSTD(tokenInput);
        return;
    }

    std::cout << "MerkelMain::processUserInput The command entered is Unknown - type help to see how to enter input" << std::endl;
}
