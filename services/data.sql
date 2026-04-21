-- Test data for taxi ordering system
-- Password hash is SHA256 of "password"

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
