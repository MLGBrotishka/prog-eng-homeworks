// validation.js — JSON Schema validation for MongoDB collections
// Run with: mongosh taxi_db validation.js

// ─────────────────────────────────────────────
// 1. users collection validation
// ─────────────────────────────────────────────
db.runCommand({
  collMod: "users",
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["login", "email", "password_hash", "first_name", "last_name", "phone", "created_at", "is_active"],
      additionalProperties: true,
      properties: {
        login: {
          bsonType: "string",
          minLength: 3,
          maxLength: 64,
          pattern: "^[a-zA-Z0-9_]+$",
          description: "login must be alphanumeric, 3-64 chars"
        },
        email: {
          bsonType: "string",
          pattern: "^[^@]+@[^@]+\\.[^@]+$",
          description: "email must be a valid email address"
        },
        password_hash: {
          bsonType: "string",
          minLength: 10,
          description: "password_hash is a bcrypt hash string"
        },
        first_name: {
          bsonType: "string",
          minLength: 1,
          maxLength: 100
        },
        last_name: {
          bsonType: "string",
          minLength: 1,
          maxLength: 100
        },
        phone: {
          bsonType: "string",
          pattern: "^\\+[0-9\\-]+$",
          description: "phone must start with + and contain only digits and dashes"
        },
        created_at: {
          bsonType: "date"
        },
        is_active: {
          bsonType: "bool"
        },
        preferences: {
          bsonType: "object",
          properties: {
            language: { bsonType: "string", enum: ["ru", "en"] },
            notifications_enabled: { bsonType: "bool" },
            preferred_car_class: { bsonType: "string", enum: ["economy", "comfort", "business"] }
          }
        },
        rating: {
          bsonType: "object",
          required: ["average", "count"],
          properties: {
            average: { bsonType: ["double", "int"], minimum: 0, maximum: 5 },
            count: { bsonType: "int", minimum: 0 }
          }
        }
      }
    }
  },
  validationLevel: "strict",
  validationAction: "error"
});
print("users validation schema applied.");

// ─────────────────────────────────────────────
// 2. drivers collection validation
// ─────────────────────────────────────────────
db.runCommand({
  collMod: "drivers",
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["login", "first_name", "last_name", "phone", "license_number", "created_at", "is_active", "status", "vehicle"],
      properties: {
        login: {
          bsonType: "string",
          minLength: 3,
          maxLength: 64,
          pattern: "^[a-zA-Z0-9_]+$"
        },
        first_name: { bsonType: "string", minLength: 1, maxLength: 100 },
        last_name: { bsonType: "string", minLength: 1, maxLength: 100 },
        phone: {
          bsonType: "string",
          pattern: "^\\+[0-9\\-]+$"
        },
        license_number: {
          bsonType: "string",
          minLength: 5
        },
        created_at: { bsonType: "date" },
        is_active: { bsonType: "bool" },
        status: {
          bsonType: "string",
          enum: ["available", "on_ride", "offline"],
          description: "status must be available, on_ride, or offline"
        },
        completed_rides: {
          bsonType: "int",
          minimum: 0
        },
        vehicle: {
          bsonType: "object",
          required: ["make", "model", "year", "plate", "car_class"],
          properties: {
            make: { bsonType: "string" },
            model: { bsonType: "string" },
            year: { bsonType: "int", minimum: 1990, maximum: 2030 },
            plate: { bsonType: "string", minLength: 3 },
            color: { bsonType: "string" },
            car_class: { bsonType: "string", enum: ["economy", "comfort", "business"] }
          }
        },
        rating: {
          bsonType: "object",
          properties: {
            average: { bsonType: ["double", "int"], minimum: 0, maximum: 5 },
            count: { bsonType: "int", minimum: 0 }
          }
        }
      }
    }
  },
  validationLevel: "strict",
  validationAction: "error"
});
print("drivers validation schema applied.");

// ─────────────────────────────────────────────
// 3. rides collection validation
// ─────────────────────────────────────────────
db.runCommand({
  collMod: "rides",
  validator: {
    $jsonSchema: {
      bsonType: "object",
      required: ["user_id", "status", "created_at", "origin", "destination", "price", "car_class"],
      properties: {
        user_id: { bsonType: "objectId" },
        driver_id: { bsonType: ["objectId", "null"] },
        status: {
          bsonType: "string",
          enum: ["pending", "accepted", "in_progress", "completed", "cancelled"]
        },
        created_at: { bsonType: "date" },
        accepted_at: { bsonType: ["date", "null"] },
        completed_at: { bsonType: ["date", "null"] },
        origin: {
          bsonType: "object",
          required: ["address"],
          properties: {
            address: { bsonType: "string", minLength: 1 },
            coordinates: { bsonType: "object" }
          }
        },
        destination: {
          bsonType: "object",
          required: ["address"],
          properties: {
            address: { bsonType: "string", minLength: 1 },
            coordinates: { bsonType: "object" }
          }
        },
        price: {
          bsonType: "object",
          required: ["estimated", "currency"],
          properties: {
            estimated: { bsonType: ["double", "int"], minimum: 0 },
            final: { bsonType: ["double", "int", "null"], minimum: 0 },
            currency: { bsonType: "string", enum: ["RUB", "USD", "EUR"] }
          }
        },
        car_class: {
          bsonType: "string",
          enum: ["economy", "comfort", "business"]
        },
        duration_minutes: { bsonType: ["int", "null"], minimum: 0 },
        distance_km: { bsonType: ["double", "null"], minimum: 0 },
        events: {
          bsonType: "array",
          items: {
            bsonType: "object",
            required: ["status", "timestamp"],
            properties: {
              status: { bsonType: "string" },
              timestamp: { bsonType: "date" },
              note: { bsonType: ["string", "null"] }
            }
          }
        }
      }
    }
  },
  validationLevel: "strict",
  validationAction: "error"
});
print("rides validation schema applied.");

// ─────────────────────────────────────────────
// 4. Test validation — inserting invalid documents should fail
// ─────────────────────────────────────────────
print("\n--- Testing validation ---");

// Test 1: user with invalid email (missing @)
try {
  db.users.insertOne({
    login: "bad_user",
    email: "not-an-email",
    password_hash: "$2b$12$validhashXXXXX",
    first_name: "Bad",
    last_name: "User",
    phone: "+7-900-000-0001",
    created_at: new Date(),
    is_active: true
  });
  print("FAIL: Should have rejected invalid email");
} catch (e) {
  print("PASS: Rejected invalid email — " + e.message.substring(0, 80));
}

// Test 2: driver with invalid status
try {
  db.drivers.insertOne({
    login: "bad_driver",
    first_name: "Bad",
    last_name: "Driver",
    phone: "+7-900-000-0002",
    license_number: "LIC-BAD-001",
    created_at: new Date(),
    is_active: true,
    status: "sleeping",   // invalid enum value
    vehicle: { make: "Toyota", model: "Camry", year: 2020, plate: "X000XX77", car_class: "economy" }
  });
  print("FAIL: Should have rejected invalid status");
} catch (e) {
  print("PASS: Rejected invalid status — " + e.message.substring(0, 80));
}

// Test 3: driver with vehicle year out of range
try {
  db.drivers.insertOne({
    login: "old_driver",
    first_name: "Old",
    last_name: "Driver",
    phone: "+7-900-000-0003",
    license_number: "LIC-OLD-001",
    created_at: new Date(),
    is_active: true,
    status: "offline",
    vehicle: { make: "Moskvich", model: "408", year: 1975, plate: "X001XX77", car_class: "economy" }
  });
  print("FAIL: Should have rejected vehicle year < 1990");
} catch (e) {
  print("PASS: Rejected old vehicle year — " + e.message.substring(0, 80));
}

// Test 4: ride with invalid status
try {
  db.rides.insertOne({
    user_id: db.users.findOne()._id,
    status: "waiting",  // invalid
    created_at: new Date(),
    origin: { address: "Some address" },
    destination: { address: "Other address" },
    price: { estimated: 300, currency: "RUB" },
    car_class: "economy"
  });
  print("FAIL: Should have rejected invalid ride status");
} catch (e) {
  print("PASS: Rejected invalid ride status — " + e.message.substring(0, 80));
}

// Test 5: ride with missing required fields
try {
  db.rides.insertOne({ status: "pending" });
  print("FAIL: Should have rejected ride missing required fields");
} catch (e) {
  print("PASS: Rejected ride missing required fields — " + e.message.substring(0, 80));
}

print("\nValidation tests complete.");
