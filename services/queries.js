// queries.js — MongoDB CRUD queries demo for Users
// Run with: mongosh taxi_db queries.js

print("=== MongoDB Queries Demo (Users Only) ===\n");

// ─────────────────────────────────────────────
// CREATE — вставка документов
// ─────────────────────────────────────────────
print("--- CREATE ---");

// C1. Создание нового пользователя (используем явный Int64 ID для совместимости с C++)
var newUser = db.users.insertOne({
  _id: NumberLong(999),
  login: "demo_user",
  email: "demo@example.com",
  password_hash: "$2b$12$demopasswordhashXXX",
  first_name: "Демо",
  last_name: "Пользователь",
  phone: "+7-900-999-0001",
  created_at: new Date(),
  is_active: true,
  preferences: { language: "ru", notifications_enabled: true, preferred_car_class: "comfort" },
  rating: { average: 0.0, count: 0 }
});
print("C1. Inserted user _id: " + newUser.insertedId + " (Type: Int64)");

// ─────────────────────────────────────────────
// READ — поиск документов
// ─────────────────────────────────────────────
print("\n--- READ ---");

// R1. Поиск пользователя по логину ($eq implicitly)
var userByLogin = db.users.findOne({ login: "ivan_petrov" });
print("R1. User by login: " + (userByLogin ? userByLogin.email : "not found"));

// R1.1 Поиск пользователя по числовому ID (Int64)
var userById = db.users.findOne({ _id: NumberLong(1) });
print("R1.1 User by numeric _id (1): " + (userById ? userById.login : "not found"));

// R2. Поиск пользователя по маске имя+фамилия (text index)
var byName = db.users.find({ $text: { $search: "Иван Петров" } }).toArray();
print("R2. Users matching text 'Иван Петров': " + byName.length);

// R9. Пользователи с рейтингом в диапазоне ($gte, $lte)
var midRatingUsers = db.users.find({
  "rating.average": { $gte: 4.0, $lte: 4.7 }
}).toArray();
print("R9. Users with rating 4.0–4.7: " + midRatingUsers.length);

// ─────────────────────────────────────────────
// UPDATE — обновление документов
// ─────────────────────────────────────────────
print("\n--- UPDATE ---");

// U1. Обновление предпочтений пользователя
db.users.updateOne(
  { login: "demo_user" },
  { $set: { "preferences.language": "en" } }
);
print("U1. Demo user preferences updated");

// ─────────────────────────────────────────────
// DELETE — удаление документов
// ─────────────────────────────────────────────
print("\n--- DELETE ---");

// D1. Удаление пользователя по логину
var delUser = db.users.deleteOne({ login: "demo_user" });
print("D1. Deleted demo_user: " + delUser.deletedCount);
