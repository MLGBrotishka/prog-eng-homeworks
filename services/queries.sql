-- 1. Create new user
INSERT INTO users (login, email, first_name, last_name, password_hash)
VALUES ($1, $2, $3, $4, $5)
RETURNING id, login, email, first_name, last_name, password_hash, created_at;

-- 2. Find user by login
SELECT id, login, email, first_name, last_name, password_hash, created_at
FROM users WHERE login = $1;

-- 3. Search users by first_name/last_name mask (case-insensitive)
SELECT id, login, email, first_name, last_name, password_hash, created_at
FROM users WHERE first_name || ' ' || last_name ILIKE $1;

-- 4. Register driver
INSERT INTO drivers (user_id, full_name, license_plate)
VALUES ($1, $2, $3)
RETURNING id, user_id, full_name, license_plate, status, created_at;

-- 5. Create ride order
INSERT INTO rides (passenger_id, from_address, to_address, cost)
VALUES ($1, $2, $3, $4)
RETURNING id, passenger_id, driver_id, from_address, to_address, status, cost, created_at;

-- 6. Get active rides (not completed)
SELECT id, passenger_id, driver_id, from_address, to_address, status, cost, created_at
FROM rides WHERE status != 'completed'
ORDER BY created_at DESC;

-- 7. Accept ride by driver
UPDATE rides SET driver_id = $1, status = 'accepted'
WHERE id = $2 AND status = 'pending'
RETURNING id, passenger_id, driver_id, from_address, to_address, status, cost, created_at;

-- 8. Get user ride history
SELECT id, passenger_id, driver_id, from_address, to_address, status, cost, created_at
FROM rides WHERE passenger_id = $1
ORDER BY created_at DESC;

-- 9. Complete ride
UPDATE rides SET status = 'completed'
WHERE id = $1 AND status = 'accepted'
RETURNING id, passenger_id, driver_id, from_address, to_address, status, cost, created_at;

-- Auxiliary: Find user by id
SELECT id, login, email, first_name, last_name, password_hash, created_at
FROM users WHERE id = $1;

-- Auxiliary: Get driver by user_id
SELECT id, user_id, full_name, license_plate, status, created_at
FROM drivers WHERE user_id = $1;

-- Auxiliary: Get driver by id
SELECT id, user_id, full_name, license_plate, status, created_at
FROM drivers WHERE id = $1;

-- Auxiliary: Get ride by id
SELECT id, passenger_id, driver_id, from_address, to_address, status, cost, created_at
FROM rides WHERE id = $1;
