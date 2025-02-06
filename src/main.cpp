#include "PaymentProcessor.h"
#include <iostream>

int main() 
{
    // Initialize the payment processor with the database connection details.
    PaymentProcessor processor("dbname=paymentdb user=postgres password=admin");

    std::string command;
    while (true) 
    {
        // Prompt user to enter a command to either make a payment, view transaction history, or exit the program
        std::cout << "Enter command 'payment', 'history' or 'exit': ";
        std::cin >> command;

        // If the user enters "payment", proceed to process a payment
        if (command == "payment") 
        {
            double amount;
            std::string cardNumber, merchantType;

            // Prompt the user for the payment details
            std::cout << "Enter amount: ";
            std::cin >> amount;
            std::cout << "Enter card number: ";
            std::cin >> cardNumber;
            std::cout << "Enter merchant type ('Supermarket', 'Restaurant' or 'Leisure'): ";
            std::cin >> merchantType;

            // Process the payment using the provided details
            // The PaymentProcessor's processPayment method will handle fee calculations and check if the payment is valid
            processor.processPayment(amount, cardNumber, merchantType);
        } 
        // If the user enters "history", show the transaction history for a card
        else if (command == "history") 
        {
            std::string cardNumber, startDate, endDate;

            // Prompt the user for the card number
            std::cout << "Enter card number: ";
            std::cin >> cardNumber;

            // Clear input buffer before using getline
            std::cin.ignore();

            // Prompt for optional start date and end date
            std::cout << "Enter start date (or press Enter to skip): ";
            std::getline(std::cin, startDate);
            // If no start date is entered, treat it as an empty string
            if (startDate.empty()) startDate = "";

            std::cout << "Enter end date (or press Enter to skip): ";
            std::getline(std::cin, endDate);
            // If no end date is entered, treat it as an empty string
            if (endDate.empty()) endDate = "";

            // Show transaction history based on the provided card number and optional date range
            processor.showHistory(cardNumber, startDate, endDate);
        } 
        // If the user enters "exit", break the loop and exit the program
        else if (command == "exit") 
        {
            break;
        } 
        // If the entered command is not recognized, inform the user
        else 
        {
            std::cout << "Unknown command. Please enter 'payment', 'history', or 'exit'." << std::endl;
        }
    }

    return 0;
}
