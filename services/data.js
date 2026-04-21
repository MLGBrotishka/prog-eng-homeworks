// data.js — Seed data for taxi_db MongoDB database
// Run with: mongosh taxi_db data.js
// Or loaded automatically via docker-entrypoint-initdb.d

// ─────────────────────────────────────────────
// Users (passengers) — 12 documents
// ─────────────────────────────────────────────
db.users.drop();
db.users.insertMany([
  {
    _id: NumberLong(1),
    login: "ivan_petrov",
    email: "ivan.petrov@example.com",
    password_hash: "$2b$12$examplehash1",
    first_name: "Иван",
    last_name: "Петров",
    phone: "+7-900-111-0001",
    created_at: new Date("2024-01-15T10:00:00Z"),
    is_active: true,
    preferences: { language: "ru", notifications_enabled: true, preferred_car_class: "comfort" },
    rating: { average: 4.8, count: 32 }
  },
  {
    _id: NumberLong(2),
    login: "maria_smirnova",
    email: "maria.smirnova@example.com",
    password_hash: "$2b$12$examplehash2",
    first_name: "Мария",
    last_name: "Смирнова",
    phone: "+7-900-111-0002",
    created_at: new Date("2024-02-01T09:30:00Z"),
    is_active: true,
    preferences: { language: "ru", notifications_enabled: true, preferred_car_class: "economy" },
    rating: { average: 4.9, count: 57 }
  },
  {
    _id: NumberLong(3),
    login: "alex_kozlov",
    email: "alex.kozlov@example.com",
    password_hash: "$2b$12$examplehash3",
    first_name: "Александр",
    last_name: "Козлов",
    phone: "+7-900-111-0003",
    created_at: new Date("2024-02-10T14:20:00Z"),
    is_active: true,
    preferences: { language: "ru", notifications_enabled: false, preferred_car_class: "business" },
    rating: { average: 4.5, count: 18 }
  },
  {
    _id: NumberLong(4),
    login: "olga_novikova",
    email: "olga.novikova@example.com",
    password_hash: "$2b$12$examplehash4",
    first_name: "Ольга",
    last_name: "Новикова",
    phone: "+7-900-111-0004",
    created_at: new Date("2024-03-05T11:00:00Z"),
    is_active: true,
    preferences: { language: "ru", notifications_enabled: true, preferred_car_class: "economy" },
    rating: { average: 5.0, count: 4 }
  },
  {
    _id: NumberLong(5),
    login: "dmitry_volkov",
    email: "dmitry.volkov@example.com",
    password_hash: "$2b$12$examplehash5",
    first_name: "Дмитрий",
    last_name: "Волков",
    phone: "+7-900-111-0005",
    created_at: new Date("2024-03-12T08:45:00Z"),
    is_active: true,
    preferences: { language: "ru", notifications_enabled: true, preferred_car_class: "comfort" },
    rating: { average: 4.2, count: 11 }
  },
  {
    _id: NumberLong(6),
    login: "elena_morozova",
    email: "elena.morozova@example.com",
    password_hash: "$2b$12$examplehash6",
    first_name: "Елена",
    last_name: "Морозова",
    phone: "+7-900-111-0006",
    created_at: new Date("2024-03-20T16:10:00Z"),
    is_active: true,
    preferences: { language: "en", notifications_enabled: true, preferred_car_class: "comfort" },
    rating: { average: 4.7, count: 29 }
  },
  {
    _id: NumberLong(7),
    login: "sergey_fedorov",
    email: "sergey.fedorov@example.com",
    password_hash: "$2b$12$examplehash7",
    first_name: "Сергей",
    last_name: "Фёдоров",
    phone: "+7-900-111-0007",
    created_at: new Date("2024-04-01T12:00:00Z"),
    is_active: true,
    preferences: { language: "ru", notifications_enabled: false, preferred_car_class: "economy" },
    rating: { average: 3.9, count: 7 }
  },
  {
    _id: NumberLong(8),
    login: "anna_popova",
    email: "anna.popova@example.com",
    password_hash: "$2b$12$examplehash8",
    first_name: "Анна",
    last_name: "Попова",
    phone: "+7-900-111-0008",
    created_at: new Date("2024-04-15T09:00:00Z"),
    is_active: true,
    preferences: { language: "ru", notifications_enabled: true, preferred_car_class: "business" },
    rating: { average: 4.6, count: 43 }
  },
  {
    _id: NumberLong(9),
    login: "nikita_sokolov",
    email: "nikita.sokolov@example.com",
    password_hash: "$2b$12$examplehash9",
    first_name: "Никита",
    last_name: "Соколов",
    phone: "+7-900-111-0009",
    created_at: new Date("2024-04-20T13:30:00Z"),
    is_active: false,
    preferences: { language: "ru", notifications_enabled: true, preferred_car_class: "economy" },
    rating: { average: 4.1, count: 15 }
  },
  {
    _id: NumberLong(10),
    login: "kate_lebedeva",
    email: "kate.lebedeva@example.com",
    password_hash: "$2b$12$examplehash10",
    first_name: "Екатерина",
    last_name: "Лебедева",
    phone: "+7-900-111-0010",
    created_at: new Date("2024-05-01T10:15:00Z"),
    is_active: true,
    preferences: { language: "en", notifications_enabled: true, preferred_car_class: "comfort" },
    rating: { average: 4.8, count: 21 }
  },
  {
    _id: NumberLong(11),
    login: "pavel_andreev",
    email: "pavel.andreev@example.com",
    password_hash: "$2b$12$examplehash11",
    first_name: "Павел",
    last_name: "Андреев",
    phone: "+7-900-111-0011",
    created_at: new Date("2024-05-10T11:00:00Z"),
    is_active: true,
    preferences: { language: "ru", notifications_enabled: false, preferred_car_class: "economy" },
    rating: { average: 4.4, count: 9 }
  },
  {
    _id: NumberLong(12),
    login: "julia_romanova",
    email: "julia.romanova@example.com",
    password_hash: "$2b$12$examplehash12",
    first_name: "Юлия",
    last_name: "Романова",
    phone: "+7-900-111-0012",
    created_at: new Date("2024-05-20T08:00:00Z"),
    is_active: true,
    preferences: { language: "ru", notifications_enabled: true, preferred_car_class: "comfort" },
    rating: { average: 4.9, count: 38 }
  }
]);

// ─────────────────────────────────────────────
// Indexes
// ─────────────────────────────────────────────
db.users.createIndex({ login: 1 }, { unique: true });
db.users.createIndex({ email: 1 }, { unique: true });
db.users.createIndex({ first_name: "text", last_name: "text" });

print("Seed data loaded successfully.");
print("users: " + db.users.countDocuments());
