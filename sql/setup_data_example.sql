-- Insert account with different account limits
INSERT INTO Account (account_limit) VALUES (500.00);
INSERT INTO Account (account_limit) VALUES (2000.00);
INSERT INTO Account (account_limit) VALUES (1500.00);
INSERT INTO Account (account_limit) VALUES (10000.00);

-- Insert cards for different account IDs with expiry dates
INSERT INTO Card (account_id, card_number, cardholder_name, expiry_date, card_type)
VALUES (1, '1234567890123456', 'nameExample1', '2025-12-31', 'Visa');
INSERT INTO Card (account_id, card_number, cardholder_name, expiry_date, card_type)
VALUES (2, '2345678901234567', 'nameExample2', '2026-05-15', 'MasterCard');
INSERT INTO Card (account_id, card_number, cardholder_name, expiry_date, card_type)
VALUES (3, '3456789012345678', 'nameExample3', '2027-08-21', 'AMEX');
INSERT INTO Card (account_id, card_number, cardholder_name, expiry_date, card_type)
VALUES (4, '4567890123456789', 'nameExample14', '2025-11-30', 'Visa');

-- Insert transactions for different cards and merchants
INSERT INTO Transaction (card_id, merchant_type, transaction_time, amount)
VALUES (1, 'Supermarket', NOW(), 200.00);
INSERT INTO Transaction (card_id, merchant_type, transaction_time, amount)
VALUES (1, 'Restaurant', NOW(), 50.00);
INSERT INTO Transaction (card_id, merchant_type, transaction_time, amount)
VALUES (2, 'Leisure', NOW(), 100.00);
INSERT INTO Transaction (card_id, merchant_type, transaction_time, amount)
VALUES (3, 'Supermarket', NOW(), 150.00);
INSERT INTO Transaction (card_id, merchant_type, transaction_time, amount)
VALUES (4, 'Restaurant', NOW(), 80.00);

