#include "OrderBook.h"
#include "CSVReader.h"
#include <map>
#include <algorithm>
#include <iostream>
#include <unordered_map>

/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);
}

/** return vector of all know products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts(std::string const &currentTime)
{
    // changes: use unordered_map instead of map and made function to find product only from current time step
    std::vector<std::string> products;
    std::unordered_map<std::string, bool> prodMap;

    for (OrderBookEntry &e : orders)
    {
        if (e.timestamp == currentTime)
        {
            prodMap[e.product] = true;
        }
        else if (e.timestamp > currentTime)
        {
            break;
        }
    }

    for (auto const &e : prodMap)
    {
        products.push_back(e.first);
    }

    return products;
}
/** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type,
                                                 std::string product,
                                                 std::string timestamp)
{
    std::cout << product << std::endl;
    std::cout << timestamp << std::endl;
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry &e : orders)
    {
        if (e.timestamp < timestamp)
        {
            continue;
        }
        if (e.timestamp > timestamp)
        {
            break;
        }
        if (e.orderType == type && e.product == product && e.timestamp == timestamp)
        {
            orders_sub.push_back(e);
        }

    }
    return orders_sub;
    
}


//=========================== GET HIGH, Low AND AVG Price ==============================
double OrderBook::getHighPrice(std::vector<OrderBookEntry> &orders)
{
    //This has been changed to start at -1
    double max = -1;
    for(OrderBookEntry& e : orders) {
        if(e.price > max)
            max = e.price;
    }
    return max;
}

double OrderBook::getLowPrice(std::vector<OrderBookEntry> &orders)
{
    double min = orders[0].price;
    for (OrderBookEntry &e : orders)
    {
        if (e.price < min)
            min = e.price;
    }
    return min;
}

double OrderBook::getAvgPrice(std::vector<OrderBookEntry> &orders)
{
    double totalOfPrices = 0;
    for (OrderBookEntry &e : orders)
    {
        totalOfPrices += e.price;
    }

    return totalOfPrices / orders.size();
}

//===============================CHANGE TIME STEPS=========================================
std::string OrderBook::getEarliestTime()
{
    if(orders.size() != 0) {
        return orders[0].timestamp;
    }
    return "";
}

std::string OrderBook::getNextTime(std::string timestamp)
{
    //It does not loop over dataset if reaches the final timestamp
    std::string next_timestamp = "";
    for(OrderBookEntry& e : orders) {
        if(e.timestamp > timestamp) {
            next_timestamp = e.timestamp;
            break;
        }
    }

    return next_timestamp;
}

std::string OrderBook::getPrevTime(std::string timestamp)
{
    //this allows the user to go back timeframes
    std::string next_timestamp = "";
    for (OrderBookEntry &e : orders)
    {
        if (e.timestamp >= timestamp)
        {
            break;
        }
        else
        {
            next_timestamp = e.timestamp;
        }
    }
    return next_timestamp;
}

//============================= MATCH ASKS AND BIDS ======================================
std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string timestamp)
{
    // This has been changed to only take in the timestamp as an input
    /* This has been optimized to find matches for every product in the time step instead of only one *.
     */
    // it also finds all the sales for the current time step
    std::vector<OrderBookEntry> sales;

    // loop over all products
    std::vector<std::string> products = getKnownProducts(timestamp);
    for (std::string &product : products)
    {
        // create a vector for all the asks in current time step
        std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, product, timestamp);
        // create a vector for al bids in current time step
        std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, product, timestamp);

        // sorts the asks and bids so that iteration is effective
        std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
        std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);

        // loop over all asks and bids to find matches
        for (OrderBookEntry &ask : asks)
        {
            if (ask.amount == 0)
            {
                continue;
            }
            for (OrderBookEntry &bid : bids)
            {
                if (bid.price >= ask.price)
                {
                    if (bid.amount == 0)
                    {
                        continue;
                    }
                    //creating the sale of a product because the sale criteria is met
                    OrderBookEntry sale{ask.price, 0, timestamp, product, OrderBookType::asksale};

                    //adjusting the sale amount according to the actual amount of the product
                    sale.amount = std::min(bid.amount, ask.amount);
                    ask.amount -= sale.amount;
                    bid.amount -= sale.amount;

                    //Adding this to the database
                    sales.push_back(sale);
                }
            }
        }
    }

    return sales;
}

//==========================GET ENTRIES SIZE ==============================================
int OrderBook::getEntriesSize()
{
    return orders.size();
}

//========================== SET CURRENT TIME =============================================
std::string OrderBook::setCurrentTime(int a)
{
    if (a < 1 || a > orders.size())
    {
        std::cout << "OrderBook::setCurrentTime Bad input" << std::endl;
        return "";
    }

    std::string currentTimeStamp = orders[0].timestamp;
    int cnt = 1;
    for (OrderBookEntry &e : orders)
    {
        if (currentTimeStamp != e.timestamp)
        {
            currentTimeStamp = e.timestamp;
            cnt++;
        }

        if (a == cnt)
        {
            break;
        }
    }

    return currentTimeStamp;
}



