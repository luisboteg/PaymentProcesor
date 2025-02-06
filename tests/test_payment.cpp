#include "../src/PaymentProcessor.h"
#include <gtest/gtest.h>

// Testable class that exposes calculateTotalAmount() from PaymentProcessor
class TestablePaymentProcessor : public PaymentProcessor 
{
public:
    using PaymentProcessor::PaymentProcessor;
    using PaymentProcessor::calculateTotalAmount;  // Expose the protected function for testing purposes
};

// Test fixture for PaymentProcessor with database integration
class PaymentProcessorTest : public ::testing::Test 
{
protected:
    // Set up the database connection and prepare the environment for tests
    void SetUp() override 
    {
        // Establish a database connection
        conn = std::make_unique<pqxx::connection>("dbname=paymentdb user=postgres password=admin");

        // Clean the database before each test
        clearDatabase();

        // Create a test account with card and initial transaction data
        createTestAccount("1234567890123456", "Visa", 1000.0, 0.0);
    }

    // Clean up the test environment after each test
    void TearDown() override 
    {
        clearDatabase();
    }

    // Helper function to clean the database by deleting all records from relevant tables
    void clearDatabase() 
    {
        pqxx::work txn(*conn);
        txn.exec("DELETE FROM Transaction");
        txn.exec("DELETE FROM Card");
        txn.exec("DELETE FROM Account");
        txn.commit();
    }

    // Helper function to create a test account with a card and a transaction
    void createTestAccount(const std::string& cardNumber, const std::string& cardType, double accountLimit, double spentAmount) 
    {
        pqxx::work txn(*conn);

        // Insert a new account into the database
        txn.exec("INSERT INTO Account (account_id, account_limit) VALUES (1, " + std::to_string(accountLimit) + ")");

        // Insert a new card associated with the created account
        txn.exec("INSERT INTO Card (card_id, account_id, card_number, cardholder_name,  card_type, expiry_date) VALUES (1, 1, " + 
                txn.quote(cardNumber) + ", 'NameExample', " + txn.quote(cardType) + ", '2028-01-01')");

        // Insert simulated transaction data to match the account's spent amount
        txn.exec("INSERT INTO Transaction (card_id, merchant_type, transaction_time, amount) VALUES (1, 'Supermarket', NOW(), " +
                 std::to_string(spentAmount) + ")");

        txn.commit();
    }

    // Database connection
    std::unique_ptr<pqxx::connection> conn;
};

// Test case: Successful payment processing
TEST_F(PaymentProcessorTest, PaymentSuccess) 
{
    TestablePaymentProcessor processor("dbname=paymentdb user=postgres password=admin");
    EXPECT_TRUE(processor.processPayment(50.0, "1234567890123456", "Supermarket"));
}

// Test case: Payment exceeds account limit
TEST_F(PaymentProcessorTest, PaymentExceedsLimit) 
{
    TestablePaymentProcessor processor("dbname=paymentdb user=postgres password=admin");
    EXPECT_FALSE(processor.processPayment(100000.0, "1234567890123456", "Restaurant"));
}

// Test case: Invalid card number
TEST_F(PaymentProcessorTest, InvalidCard) 
{
    TestablePaymentProcessor processor("dbname=paymentdb user=postgres password=admin");
    EXPECT_FALSE(processor.processPayment(50.0, "9999999999999999", "Supermarket"));
}

// Test case: Transaction history retrieval (only checks if it runs without errors)
TEST_F(PaymentProcessorTest, TransactionHistory) 
{
    TestablePaymentProcessor processor("dbname=paymentdb user=postgres password=admin");
    processor.showHistory("1234567890123456");
    // TODO: Verify std::cout output
    // Expected behavior: Default test transaction should be shown.
}

// Test case: Transaction history for an entire account (across multiple cards)
TEST_F(PaymentProcessorTest, TransactionHistoryForEntireAccount) 
{
    pqxx::work txn(*conn);
    
    // Add a second card associated with the same account
    txn.exec("INSERT INTO Card (card_id, account_id, card_number, cardholder_name, card_type, expiry_date) VALUES (2, 1, '6543210987654321', 'NameExample', 'MasterCard', '2028-01-01')");
    
    // Add a transaction for the second card
    txn.exec("INSERT INTO Transaction (card_id, merchant_type, transaction_time, amount) VALUES (2, 'Leisure', NOW(), 30.0)");
    
    txn.commit();
    
    TestablePaymentProcessor processor("dbname=paymentdb user=postgres password=admin");
    processor.showHistory("1234567890123456"); // Should display transactions from both cards
    // TODO: Verify std::cout output
    // Expected behavior: Default test transactionand new one should be shown.
}

// Test case: Transaction history with date range filter, including a transaction outside the range
TEST_F(PaymentProcessorTest, TransactionHistoryWithDateRangeFilter) 
{
    pqxx::work txn(*conn);
    
    // Add two transactions: one inside the date range and one outside
    txn.exec("INSERT INTO Transaction (card_id, merchant_type, transaction_time, amount) VALUES (1, 'Supermarket', '2023-06-01', 50.0)");
    txn.exec("INSERT INTO Transaction (card_id, merchant_type, transaction_time, amount) VALUES (1, 'Restaurant', '2024-01-01', 100.0)");
    
    txn.commit();

    // Test with date range from '2023-01-01' to '2023-12-31'
    TestablePaymentProcessor processor("dbname=paymentdb user=postgres password=admin");
    processor.showHistory("1234567890123456", "2023-01-01", "2023-12-31");

    // TODO: Verify std::cout output
    // Expected behavior: only the 'Supermarket' transaction from '2023-06-01' should be shown.
}

// Test case: Correct fee calculation for Supermarket with Visa card
TEST_F(PaymentProcessorTest, CalculateFeesSupermarketVisa) 
{
    TestablePaymentProcessor processor("dbname=paymentdb user=postgres password=admin");
    double totalAmount = processor.calculateTotalAmount(100.0, "Supermarket", "Visa");
    EXPECT_DOUBLE_EQ(totalAmount, 104.0);  // 100 + (100 * 0.01) + (100 * 0.03)
}

// Test case: Correct fee calculation for Restaurant with MasterCard
TEST_F(PaymentProcessorTest, CalculateFeesRestaurantMasterCard) 
{
    TestablePaymentProcessor processor("dbname=paymentdb user=postgres password=admin");
    double totalAmount = processor.calculateTotalAmount(100.0, "Restaurant", "MasterCard");
    EXPECT_DOUBLE_EQ(totalAmount, 107.0);  // 100 + (100 * 0.05) + (100 * 0.02)
}

// Test case: Correct fee calculation for Leisure with AMEX card
TEST_F(PaymentProcessorTest, CalculateFeesLeisureAMEX) 
{
    TestablePaymentProcessor processor("dbname=paymentdb user=postgres password=admin");
    double totalAmount = processor.calculateTotalAmount(100.0, "Leisure", "AMEX");
    EXPECT_DOUBLE_EQ(totalAmount, 108.0);  // 100 + (100 * 0.03) + (100 * 0.05)
}

// Test case: Database connection error handling
TEST_F(PaymentProcessorTest, DatabaseConnectionError) 
{
    EXPECT_THROW({
        TestablePaymentProcessor processor("dbname=invalid user=invalid password=invalid");
        processor.processPayment(50.0, "1234567890123456", "Supermarket");
    }, std::runtime_error);
}

// Test case: Valid card but invalid account
TEST_F(PaymentProcessorTest, ValidCardInvalidAccount) 
{
    TestablePaymentProcessor processor("dbname=paymentdb user=postgres password=admin");
    EXPECT_FALSE(processor.processPayment(50.0, "11111111111111", "Supermarket"));
}

// Test case: Transaction history with date range filter
TEST_F(PaymentProcessorTest, TransactionHistoryWithDates) 
{
    TestablePaymentProcessor processor("dbname=paymentdb user=postgres password=admin");
    processor.showHistory("1234567890123456", "2023-01-01", "2023-12-31");
}

// Test case: Expired Card
TEST_F(PaymentProcessorTest, ExpiredCard) 
{
    pqxx::work txn(*conn);
    
    // Add a second card associated with the same account and a expired Card
    txn.exec("INSERT INTO Card (card_id, account_id, card_number, cardholder_name, card_type, expiry_date) VALUES (2, 1, '6543210987654321', 'NameExample', 'MasterCard', '2020-01-01')");
    txn.commit();

    TestablePaymentProcessor processor("dbname=paymentdb user=postgres password=admin");
    EXPECT_FALSE(processor.processPayment(50.0, "6543210987654321", "Supermarket")); // This card is expired
}

// Main function to run all tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
