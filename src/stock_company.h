#ifndef STOCK_COMPANY_H
#define STOCK_COMPANY_H

#include <string>
#include <unordered_map>

using namespace std;

struct Action
{
    string compName = "";
    string action = "";
    string date = "";
    float stockWorth = 0;
    float stockSpent = 0;
    int stockQty = 0;
    float lastPrice = 0;
};

struct Company
{
    string compName = "";
    string date = "";
    float stockWorth = 0;
    float stockSpent = 0;
    int stockQty = 0;
    float lastPrice = 0;
    float cNetProfit = 0;
};

#endif  // STOCK_COMPANY_H
