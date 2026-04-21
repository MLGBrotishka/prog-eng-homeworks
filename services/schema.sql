CREATE TABLE drivers (
    id            BIGSERIAL PRIMARY KEY,
    user_id       BIGINT NOT NULL,
    full_name     VARCHAR(255) NOT NULL,
    license_plate VARCHAR(50) NOT NULL,
    status        VARCHAR(50) NOT NULL DEFAULT 'available'
                      CHECK (status IN ('available', 'busy', 'offline')),
    created_at    TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT uq_drivers_user_id UNIQUE(user_id)
);

CREATE TABLE rides (
    id            BIGSERIAL PRIMARY KEY,
    passenger_id  BIGINT NOT NULL,
    driver_id     BIGINT REFERENCES drivers(id),
    from_address  TEXT NOT NULL,
    to_address    TEXT NOT NULL,
    status        VARCHAR(50) NOT NULL DEFAULT 'pending'
                      CHECK (status IN ('pending', 'accepted', 'completed', 'cancelled')),
    cost          DOUBLE PRECISION CHECK (cost >= 0),
    created_at    TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

-- FK lookup
CREATE INDEX idx_drivers_user_id ON drivers(user_id);

-- find available drivers
CREATE INDEX idx_drivers_status ON drivers(status);

-- user ride history (API op 8)
CREATE INDEX idx_rides_passenger_id ON rides(passenger_id);

-- driver FK lookup
CREATE INDEX idx_rides_driver_id ON rides(driver_id);

-- active rides listing (API op 6)
CREATE INDEX idx_rides_status ON rides(status);

-- ordering by time
CREATE INDEX idx_rides_created_at ON rides(created_at);
