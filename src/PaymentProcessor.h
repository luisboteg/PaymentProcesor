#ifndef PAYMENTPROCESSOR_H
#define PAYMENTPROCESSOR_H

#include <pqxx/pqxx>
#include <string>

/**
 * @class PaymentProcessor
 * @brief Handles payment processing and transaction history retrieval.
 *
 * This class interacts with a PostgreSQL database to process payments,
 * store transactions, and retrieve transaction history.
 */
class PaymentProcessor 
{
public:
    /**
     * @brief Constructor that initializes the database connection.
     * @param dbConnectionString Connection string for the PostgreSQL database.
     */
    PaymentProcessor(const std::string& dbConnectionString);

    /**
     * @brief Processes a payment transaction.
     * 
     * @param amount The transaction amount.
     * @param cardNumber The credit card number used for payment.
     * @param merchantType The type of merchant (e.g., "Supermarket", "Restaurant", "Leisure").
     * @return true if the transaction is successful, false otherwise.
     */
    bool processPayment(double amount, const std::string& cardNumber, const std::string& merchantType);

    /**
     * @brief Displays the transaction history for the account associated with a given card number.
     *
     * @param cardNumber The credit card number.
     * @param startDate (Optional) Start date for filtering transactions. Date Format: 'YYYY/MM/DD'
     * @param endDate (Optional) End date for filtering transactions. Date Format: 'YYYY/MM/DD'
     */
    void showHistory(const std::string& cardNumber, const std::string& startDate = "", const std::string& endDate = "");

protected:
    /**
     * @brief Calculates the total amount including merchant and issuer fees.
     * 
     * @param amount The base transaction amount.
     * @param merchantType The type of merchant (determines the fee percentage).
     * @param cardType The type of card (determines the issuer fee percentage).
     * @return The total amount after applying fees.
     */
    double calculateTotalAmount(double amount, const std::string& merchantType, const std::string& cardType);

private:
    pqxx::connection conn; ///< Database connection instance.
};

#endif // PAYMENTPROCESSOR_H
