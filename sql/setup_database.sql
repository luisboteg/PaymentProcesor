-- Create the user paymentdb if it does not exist
DO
$$
BEGIN
    IF NOT EXISTS (SELECT FROM pg_catalog.pg_roles WHERE rolname = 'paymentdb') THEN
        CREATE USER paymentdb WITH PASSWORD 'yourpassword';
    END IF;
END
$$;

-- Create the database paymentdb if it does not exist
CREATE DATABASE paymentdb;

-- Grant privileges to paymentdb user
GRANT ALL PRIVILEGES ON DATABASE paymentdb TO paymentdb;

-- Connect to the newly created database
\c paymentdb

-- Create the Account table
CREATE TABLE Account (
    account_id SERIAL PRIMARY KEY,
    account_limit DECIMAL(10, 2) NOT NULL
);

-- Create the Card table
CREATE TABLE Card (
    card_id SERIAL PRIMARY KEY,
    account_id INTEGER REFERENCES Account(account_id) ON DELETE CASCADE,
    card_number VARCHAR(16) UNIQUE NOT NULL,
    cardholder_name VARCHAR(100) NOT NULL,
    expiry_date DATE NOT NULL,
    card_type VARCHAR(50) NOT NULL
);

-- Create the Transaction table
CREATE TABLE Transaction (
    transaction_id SERIAL PRIMARY KEY,
    card_id INTEGER REFERENCES Card(card_id) ON DELETE CASCADE,
    merchant_type VARCHAR(50) NOT NULL,
    transaction_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    amount DECIMAL(10, 2) NOT NULL
);
