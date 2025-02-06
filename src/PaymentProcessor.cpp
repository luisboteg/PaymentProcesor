#include "PaymentProcessor.h"
#include <iostream>

// Fee percentages
constexpr  double SUPERMARKET_FEE = 0.01;
constexpr double RESTAURANT_FEE = 0.05;
constexpr double LEISURE_FEE = 0.03;
constexpr double VISA_FEE = 0.03;
constexpr double MASTERCARD_FEE = 0.02;
constexpr double AMEX_FEE = 0.05;


PaymentProcessor::PaymentProcessor(const std::string& dbConnectionString) : conn(dbConnectionString) {}

double PaymentProcessor::calculateTotalAmount(double amount, const std::string& merchantType, const std::string& cardType) 
{
    // Define merchant fees based on merchant type
    double merchantFee = 0.0;
    if (merchantType == "Supermarket") merchantFee = SUPERMARKET_FEE;
    else if (merchantType == "Restaurant") merchantFee = RESTAURANT_FEE;
    else if (merchantType == "Leisure") merchantFee = LEISURE_FEE;

    // Define issuer fees based on card type
    double issuerFee = 0.0;
    if (cardType == "Visa") issuerFee = VISA_FEE;
    else if (cardType == "MasterCard") issuerFee = MASTERCARD_FEE;
    else if (cardType == "AMEX") issuerFee = AMEX_FEE;

    // Calculate total amount after applying fees
    return amount + (amount * merchantFee) + (amount * issuerFee);
}

bool PaymentProcessor::processPayment(double amount, const std::string& cardNumber, const std::string& merchantType) {
    try {
        pqxx::work txn(conn);

        // Retrieve card details from the database
        auto cardResult = txn.exec("SELECT card_id, account_id, card_type, expiry_date FROM Card WHERE card_number = " + txn.quote(cardNumber));
        if (cardResult.empty()) {
            std::cerr << "Error: Card not found." << std::endl;
            return false;
        }
        auto card = cardResult[0];

        // Check if the card is expired
        std::string expiryDate = card["expiry_date"].c_str();
        std::tm tm = {};
        std::istringstream ss(expiryDate);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        std::time_t expiryTime = std::mktime(&tm);
        std::time_t currentTime = std::time(0);  // Get the current time
        if (expiryTime < currentTime) {
            std::cerr << "Error: Card has expired." << std::endl;
            return false;
        }


        // Retrieve account limit
        auto accountResult = txn.exec("SELECT account_limit FROM Account WHERE account_id = " + txn.quote(card["account_id"].c_str()));
        if (accountResult.empty()) {
            std::cerr << "Error: Account not found." << std::endl;
            return false;
        }
        auto account = accountResult[0];

        // Calculate the total amount with applicable fees
        double totalAmount = calculateTotalAmount(amount, merchantType, card["card_type"].c_str());

        // Get the current spent amount for the card
        auto spentAmountResult = txn.exec("SELECT COALESCE(SUM(amount), 0) FROM Transaction WHERE card_id = " + txn.quote(card["card_id"].c_str()));
        double spentAmount = spentAmountResult[0][0].as<double>();

        // Check if the transaction exceeds the account limit
        if (spentAmount + totalAmount > account[0].as<double>()) {
            std::cout << "Transaction declined: Account limit exceeded." << std::endl;
            return false;
        }

        // Insert new transaction into the database
        txn.exec("INSERT INTO Transaction (card_id, merchant_type, transaction_time, amount) VALUES (" +
                txn.quote(card["card_id"].c_str()) + ", " + txn.quote(merchantType) + ", NOW(), " + std::to_string(totalAmount) + ")");

        txn.commit();
        std::cout << "Transaction approved. Total amount: " << totalAmount << std::endl;
        return true;
    } 
    catch (const std::exception& e) {
        std::cerr << "Error processing payment: " << e.what() << std::endl;
        return false;
    }
}

void PaymentProcessor::showHistory(const std::string& cardNumber, const std::string& startDate, const std::string& endDate) 
{
    try {
        pqxx::work txn(conn);

        // First, retrieve the account ID linked to the given card number
        auto accountQuery = txn.exec("SELECT account_id FROM Card WHERE card_number = " + txn.quote(cardNumber));
        if (accountQuery.empty()) {
            std::cerr << "Error: Card not found." << std::endl;
            return;
        }
        std::string accountId = accountQuery[0]["account_id"].c_str();

        // Build the query to retrieve transaction history for the entire account
        std::string query = "SELECT t.transaction_time, t.amount, t.merchant_type, c.card_number "
                            "FROM Transaction t "
                            "JOIN Card c ON t.card_id = c.card_id "
                            "WHERE c.account_id = " + txn.quote(accountId);

        // Apply date filtering if provided
        if (!startDate.empty()) {
            query += " AND t.transaction_time >= " + txn.quote(startDate);
        }
        if (!endDate.empty()) {
            query += " AND t.transaction_time <= " + txn.quote(endDate);
        }

        // Execute query and print transaction history
        auto result = txn.exec(query);
        if (result.empty()) {
            std::cout << "No transactions found for this account." << std::endl;
            return;
        }

        std::cout << "Transaction history for account associated with card: " << cardNumber << std::endl;
        for (const auto& row : result) {
            std::cout << "Date: " << row["transaction_time"].c_str()
                      << ", Amount: " << row["amount"].as<double>()
                      << ", Merchant: " << row["merchant_type"].c_str()
                      << ", Card Number: " << row["card_number"].c_str()
                      << std::endl;
        }
    } 
    catch (const std::exception& e) {
        std::cerr << "Error retrieving transaction history: " << e.what() << std::endl;
    }
}

