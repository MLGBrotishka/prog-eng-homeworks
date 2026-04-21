// queries.js — MongoDB CRUD queries and aggregation pipeline
// Illustrates all operators required by the assignment.
// Run with: mongosh taxi_db queries.js

print("=== MongoDB Queries Demo ===\n");

// ─────────────────────────────────────────────
// CREATE — вставка документов
// ─────────────────────────────────────────────
print("--- CREATE ---");

// C1. Создание нового пользователя
var newUser = db.users.insertOne({
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
print("C1. Inserted user _id: " + newUser.insertedId);

// C2. Регистрация водителя
var newDriver = db.drivers.insertOne({
  login: "demo_driver",
  first_name: "Демо",
  last_name: "Водитель",
  phone: "+7-900-999-0002",
  license_number: "LIC-DEMO-001",
  created_at: new Date(),
  is_active: true,
  status: "available",
  vehicle: { make: "Toyota", model: "Camry", year: 2022, plate: "Д999ДД77", color: "Белый", car_class: "comfort" },
  rating: { average: 0.0, count: 0 },
  completed_rides: 0,
  location: { type: "Point", coordinates: [37.6173, 55.7558] }
});
print("C2. Inserted driver _id: " + newDriver.insertedId);

// C3. Создание заказа поездки
var demoUserId = newUser.insertedId;
var rideResult = db.rides.insertOne({
  user_id: demoUserId,
  driver_id: null,
  status: "pending",
  created_at: new Date(),
  accepted_at: null,
  completed_at: null,
  origin: { address: "Пушкинская площадь", coordinates: { type: "Point", coordinates: [37.6059, 55.7653] } },
  destination: { address: "Красная площадь", coordinates: { type: "Point", coordinates: [37.6208, 55.7540] } },
  price: { estimated: 350.0, final: null, currency: "RUB" },
  car_class: "comfort",
  duration_minutes: null,
  distance_km: null,
  rating: { user_to_driver: null, driver_to_user: null },
  events: [
    { status: "pending", timestamp: new Date(), note: null }
  ]
});
print("C3. Inserted ride _id: " + rideResult.insertedId);

// ─────────────────────────────────────────────
// READ — поиск документов
// ─────────────────────────────────────────────
print("\n--- READ ---");

// R1. Поиск пользователя по логину ($eq implicitly)
var userByLogin = db.users.findOne({ login: "ivan_petrov" });
print("R1. User by login: " + (userByLogin ? userByLogin.email : "not found"));

// R2. Поиск пользователя по маске имя+фамилия (text index)
var byName = db.users.find({ $text: { $search: "Иван Петров" } }).toArray();
print("R2. Users matching text 'Иван Петров': " + byName.length);

// R3. Получение активных заказов ($in, $eq)
var activeRides = db.rides.find({
  status: { $in: ["pending", "accepted", "in_progress"] }
}).sort({ created_at: -1 }).toArray();
print("R3. Active rides count: " + activeRides.length);

// R4. Поиск водителей с рейтингом выше 4.5 ($gt)
var highRatedDrivers = db.drivers.find({
  "rating.average": { $gt: 4.5 },
  is_active: true
}).toArray();
print("R4. Drivers with rating > 4.5: " + highRatedDrivers.length);

// R5. История поездок пользователя — завершённые ($eq + user_id ref)
var userId = db.users.findOne({ login: "ivan_petrov" })._id;
var rideHistory = db.rides.find({
  user_id: userId,
  status: "completed"
}).sort({ created_at: -1 }).toArray();
print("R5. Completed rides for ivan_petrov: " + rideHistory.length);

// R6. Водители — доступные И класс economy ($and, $eq)
var availableEconomy = db.drivers.find({
  $and: [
    { status: "available" },
    { "vehicle.car_class": "economy" }
  ]
}).toArray();
print("R6. Available economy drivers: " + availableEconomy.length);

// R7. Поездки по нескольким статусам ($in) с ценой > 1000 ($gt)
var expensiveRides = db.rides.find({
  $and: [
    { status: { $in: ["completed", "in_progress"] } },
    { "price.estimated": { $gt: 1000 } }
  ]
}).toArray();
print("R7. Completed/active rides with price > 1000: " + expensiveRides.length);

// R8. Водители НЕ offline ($ne)
var onlineDrivers = db.drivers.find({ status: { $ne: "offline" } }).toArray();
print("R8. Drivers not offline: " + onlineDrivers.length);

// R9. Пользователи с рейтингом в диапазоне ($gte, $lte)
var midRatingUsers = db.users.find({
  "rating.average": { $gte: 4.0, $lte: 4.7 }
}).toArray();
print("R9. Users with rating 4.0–4.7: " + midRatingUsers.length);

// R10. Поиск поездок $or: статус pending ИЛИ цена < 300
var cheapOrPending = db.rides.find({
  $or: [
    { status: "pending" },
    { "price.estimated": { $lt: 300 } }
  ]
}).toArray();
print("R10. Pending OR cheap (< 300) rides: " + cheapOrPending.length);

// ─────────────────────────────────────────────
// UPDATE — обновление документов
// ─────────────────────────────────────────────
print("\n--- UPDATE ---");

var demoDriverId = newDriver.insertedId;
var demoRideId = rideResult.insertedId;

// U1. Принятие заказа водителем — обновление driver_id + status + events ($push)
db.rides.updateOne(
  { _id: demoRideId, status: "pending" },
  {
    $set: {
      driver_id: demoDriverId,
      status: "accepted",
      accepted_at: new Date()
    },
    $push: {
      events: { status: "accepted", timestamp: new Date(), note: null }
    }
  }
);
print("U1. Ride accepted, status updated");

// U2. Обновление статуса водителя на on_ride ($set)
db.drivers.updateOne(
  { _id: demoDriverId },
  { $set: { status: "on_ride" } }
);
print("U2. Driver status -> on_ride");

// U3. Завершение поездки ($set final price, status, duration)
db.rides.updateOne(
  { _id: demoRideId, status: "accepted" },
  {
    $set: {
      status: "completed",
      completed_at: new Date(),
      "price.final": 340.0,
      duration_minutes: 28,
      distance_km: 3.5
    },
    $push: {
      events: { status: "completed", timestamp: new Date(), note: null }
    }
  }
);
print("U3. Ride completed");

// U4. Обновление рейтинга водителя ($inc completed_rides, $set rating)
db.drivers.updateOne(
  { _id: demoDriverId },
  {
    $inc: { completed_rides: 1 },
    $set: { status: "available", "rating.average": 4.5, "rating.count": 1 }
  }
);
print("U4. Driver rating updated, status -> available");

// U5. Добавление нового события в массив поездки ($push)
db.rides.updateOne(
  { _id: demoRideId },
  {
    $push: {
      events: { status: "rated", timestamp: new Date(), note: "Пассажир поставил оценку 5" }
    }
  }
);
print("U5. Rating event pushed to ride.events");

// U6. $addToSet — добавить тег к водителю (idempotent)
db.drivers.updateOne(
  { _id: demoDriverId },
  { $addToSet: { tags: "verified" } }
);
print("U6. Tag 'verified' added to driver (idempotent)");

// U7. Массовое обновление: пометить всех offline-неактивных водителей ($updateMany)
var bulkResult = db.drivers.updateMany(
  { is_active: false, status: "offline" },
  { $set: { "status": "offline" } }  // no-op but illustrates updateMany
);
print("U7. Inactive drivers touched: " + bulkResult.modifiedCount);

// ─────────────────────────────────────────────
// DELETE — удаление документов
// ─────────────────────────────────────────────
print("\n--- DELETE ---");

// D1. Удаление пользователя по логину
var delUser = db.users.deleteOne({ login: "demo_user" });
print("D1. Deleted demo_user: " + delUser.deletedCount);

// D2. Удаление водителя по логину
var delDriver = db.drivers.deleteOne({ login: "demo_driver" });
print("D2. Deleted demo_driver: " + delDriver.deletedCount);

// D3. Удаление конкретной поездки
var delRide = db.rides.deleteOne({ _id: demoRideId });
print("D3. Deleted demo ride: " + delRide.deletedCount);

// D4. Удаление всех отменённых поездок ($eq)
var delCancelled = db.rides.deleteMany({ status: "cancelled" });
print("D4. Deleted cancelled rides: " + delCancelled.deletedCount);

// ─────────────────────────────────────────────
// AGGREGATION — сводная аналитика по поездкам
// ─────────────────────────────────────────────
print("\n--- AGGREGATION ---");

// Aggregation 1: Статистика по классам автомобилей для завершённых поездок
// $match → $group → $project → $sort
var carClassStats = db.rides.aggregate([
  { $match: { status: "completed" } },                    // только завершённые
  { $group: {
      _id: "$car_class",
      total_rides: { $sum: 1 },
      avg_price: { $avg: "$price.final" },
      max_price: { $max: "$price.final" },
      min_price: { $min: "$price.final" },
      total_revenue: { $sum: "$price.final" }
  }},
  { $project: {
      car_class: "$_id",
      _id: 0,
      total_rides: 1,
      avg_price: { $round: ["$avg_price", 2] },
      max_price: 1,
      min_price: 1,
      total_revenue: { $round: ["$total_revenue", 2] }
  }},
  { $sort: { total_revenue: -1 } }
]).toArray();
print("Aggregation 1 — Revenue by car class:");
carClassStats.forEach(r => print("  " + r.car_class + ": " + r.total_rides + " rides, avg=" + r.avg_price + " RUB, total=" + r.total_revenue + " RUB"));

// Aggregation 2: Топ-3 водителя по количеству завершённых поездок
// $match → $group → $lookup → $project → $sort → $limit
var topDrivers = db.rides.aggregate([
  { $match: { status: "completed", driver_id: { $ne: null } } },
  { $group: {
      _id: "$driver_id",
      rides_count: { $sum: 1 },
      total_earned: { $sum: "$price.final" },
      avg_rating: { $avg: "$rating.user_to_driver" }
  }},
  { $lookup: {
      from: "drivers",
      localField: "_id",
      foreignField: "_id",
      as: "driver"
  }},
  { $unwind: "$driver" },
  { $project: {
      _id: 0,
      driver_name: { $concat: ["$driver.first_name", " ", "$driver.last_name"] },
      car_class: "$driver.vehicle.car_class",
      rides_count: 1,
      total_earned: { $round: ["$total_earned", 2] },
      avg_rating: { $round: ["$avg_rating", 2] }
  }},
  { $sort: { rides_count: -1 } },
  { $limit: 3 }
]).toArray();
print("\nAggregation 2 — Top-3 drivers:");
topDrivers.forEach(d => print("  " + d.driver_name + " (" + d.car_class + "): " + d.rides_count + " rides, earned=" + d.total_earned + " RUB, avg_rating=" + d.avg_rating));

// Aggregation 3: Количество поездок и суммарная выручка по датам (группировка по дню)
var dailyStats = db.rides.aggregate([
  { $match: { status: "completed" } },
  { $group: {
      _id: { $dateToString: { format: "%Y-%m-%d", date: "$completed_at" } },
      rides: { $sum: 1 },
      revenue: { $sum: "$price.final" }
  }},
  { $sort: { _id: 1 } }
]).toArray();
print("\nAggregation 3 — Daily stats:");
dailyStats.forEach(d => print("  " + d._id + ": " + d.rides + " rides, revenue=" + d.revenue + " RUB"));

print("\n=== Queries demo complete ===");
