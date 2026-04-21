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
      required: ["_id", "login", "email", "password_hash", "first_name", "last_name", "phone", "created_at", "is_active"],
      additionalProperties: true,
      properties: {
        _id: {
          bsonType: "long",
          description: "user_id must be a 64-bit integer (BIGINT equivalent)"
        },
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
// 2. Test validation — inserting invalid documents should fail
// ─────────────────────────────────────────────
print("\n--- Testing validation ---");

// Test 1: user with invalid email (missing @)
try {
  db.users.insertOne({
    _id: NumberLong(999),
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

print("\nValidation tests complete.");
