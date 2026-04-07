-- Test data for taxi ordering system
-- Password hash is SHA256 of "password"

-- Users (12 total: users 1-2 are passengers only, users 3-12 are also drivers)
INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES ('john_doe', 'john@example.com', 'John', 'Doe', '4dbd5e49147b5102ee2731ac03dd0db7decc3b8715c3df3c1f3ddc62dcbcf86d');
INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES ('jane_smith', 'jane@example.com', 'Jane', 'Smith', '4dbd5e49147b5102ee2731ac03dd0db7decc3b8715c3df3c1f3ddc62dcbcf86d');
INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES ('bob_wilson', 'bob@example.com', 'Bob', 'Wilson', '4dbd5e49147b5102ee2731ac03dd0db7decc3b8715c3df3c1f3ddc62dcbcf86d');
INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES ('alice_brown', 'alice@example.com', 'Alice', 'Brown', '4dbd5e49147b5102ee2731ac03dd0db7decc3b8715c3df3c1f3ddc62dcbcf86d');
INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES ('charlie_davis', 'charlie@example.com', 'Charlie', 'Davis', '4dbd5e49147b5102ee2731ac03dd0db7decc3b8715c3df3c1f3ddc62dcbcf86d');
INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES ('diana_evans', 'diana@example.com', 'Diana', 'Evans', '4dbd5e49147b5102ee2731ac03dd0db7decc3b8715c3df3c1f3ddc62dcbcf86d');
INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES ('frank_garcia', 'frank@example.com', 'Frank', 'Garcia', '4dbd5e49147b5102ee2731ac03dd0db7decc3b8715c3df3c1f3ddc62dcbcf86d');
INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES ('grace_harris', 'grace@example.com', 'Grace', 'Harris', '4dbd5e49147b5102ee2731ac03dd0db7decc3b8715c3df3c1f3ddc62dcbcf86d');
INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES ('henry_jackson', 'henry@example.com', 'Henry', 'Jackson', '4dbd5e49147b5102ee2731ac03dd0db7decc3b8715c3df3c1f3ddc62dcbcf86d');
INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES ('irene_king', 'irene@example.com', 'Irene', 'King', '4dbd5e49147b5102ee2731ac03dd0db7decc3b8715c3df3c1f3ddc62dcbcf86d');
INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES ('kevin_lee', 'kevin@example.com', 'Kevin', 'Lee', '4dbd5e49147b5102ee2731ac03dd0db7decc3b8715c3df3c1f3ddc62dcbcf86d');
INSERT INTO users (login, email, first_name, last_name, password_hash) VALUES ('laura_martin', 'laura@example.com', 'Laura', 'Martin', '4dbd5e49147b5102ee2731ac03dd0db7decc3b8715c3df3c1f3ddc62dcbcf86d');

-- Drivers (10 total, referencing user_ids 3-12)
INSERT INTO drivers (user_id, full_name, license_plate) VALUES (3, 'Bob Wilson', 'ABC-1234');
INSERT INTO drivers (user_id, full_name, license_plate) VALUES (4, 'Alice Brown', 'DEF-5678');
INSERT INTO drivers (user_id, full_name, license_plate) VALUES (5, 'Charlie Davis', 'GHI-9012');
INSERT INTO drivers (user_id, full_name, license_plate) VALUES (6, 'Diana Evans', 'JKL-3456');
INSERT INTO drivers (user_id, full_name, license_plate) VALUES (7, 'Frank Garcia', 'MNO-7890');
INSERT INTO drivers (user_id, full_name, license_plate) VALUES (8, 'Grace Harris', 'PQR-1357');
INSERT INTO drivers (user_id, full_name, license_plate) VALUES (9, 'Henry Jackson', 'STU-2468');
INSERT INTO drivers (user_id, full_name, license_plate) VALUES (10, 'Irene King', 'VWX-3691');
INSERT INTO drivers (user_id, full_name, license_plate) VALUES (11, 'Kevin Lee', 'YZA-4802');
INSERT INTO drivers (user_id, full_name, license_plate) VALUES (12, 'Laura Martin', 'BCD-5913');

-- Rides (15 total: 5 completed, 5 accepted, 5 pending)

-- Completed rides (driver_id set, cost set)
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (1, 1, 'Main Street 1', 'Park Avenue 10', 'completed', 25.50);
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (2, 2, 'Broadway 15', 'Elm Street 7', 'completed', 18.00);
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (3, 3, 'Oak Avenue 3', 'Pine Road 22', 'completed', 32.75);
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (4, 4, 'Cedar Lane 8', 'Maple Drive 14', 'completed', 41.00);
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (5, 5, 'Birch Street 12', 'Walnut Court 6', 'completed', 15.25);

-- Accepted rides (driver_id set, cost set)
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (1, 3, 'River Road 5', 'Lake Boulevard 19', 'accepted', 29.00);
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (2, 4, 'Hill Street 9', 'Valley Drive 11', 'accepted', 36.50);
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (3, 5, 'Spring Avenue 2', 'Autumn Lane 16', 'accepted', 22.00);
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (4, 1, 'Winter Court 7', 'Summer Place 20', 'accepted', 47.25);
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (5, 2, 'Forest Path 4', 'Garden Way 13', 'accepted', 19.75);

-- Pending rides (driver_id NULL, cost set)
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (1, NULL, 'Church Street 6', 'School Road 18', 'pending', 28.00);
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (2, NULL, 'Market Square 11', 'Station Road 25', 'pending', 35.50);
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (3, NULL, 'Harbor View 8', 'Airport Road 30', 'pending', 50.00);
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (4, NULL, 'College Avenue 14', 'Library Lane 9', 'pending', 12.00);
INSERT INTO rides (passenger_id, driver_id, from_address, to_address, status, cost) VALUES (5, NULL, 'Canal Street 3', 'Bridge Road 17', 'pending', 10.00);
