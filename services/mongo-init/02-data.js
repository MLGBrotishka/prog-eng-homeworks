// data.js — Seed data for taxi_db MongoDB database
// Run with: mongosh taxi_db data.js
// Or loaded automatically via docker-entrypoint-initdb.d

// ─────────────────────────────────────────────
// Users (passengers) — 12 documents
// ─────────────────────────────────────────────
db.users.drop();
db.users.insertMany([
  {
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
// Drivers — 12 documents
// ─────────────────────────────────────────────
db.drivers.drop();
db.drivers.insertMany([
  {
    login: "driver_alexeev",
    first_name: "Алексей",
    last_name: "Алексеев",
    phone: "+7-900-222-0001",
    license_number: "LIC-001-MSK",
    created_at: new Date("2023-06-01T08:00:00Z"),
    is_active: true,
    status: "available",
    vehicle: { make: "Toyota", model: "Camry", year: 2021, plate: "А001АА77", color: "Белый", car_class: "comfort" },
    rating: { average: 4.9, count: 312 },
    completed_rides: 312,
    location: { type: "Point", coordinates: [37.6173, 55.7558] }
  },
  {
    login: "driver_borisov",
    first_name: "Борис",
    last_name: "Борисов",
    phone: "+7-900-222-0002",
    license_number: "LIC-002-MSK",
    created_at: new Date("2023-07-15T09:00:00Z"),
    is_active: true,
    status: "on_ride",
    vehicle: { make: "Kia", model: "Rio", year: 2022, plate: "Б002ББ77", color: "Серебристый", car_class: "economy" },
    rating: { average: 4.6, count: 187 },
    completed_rides: 187,
    location: { type: "Point", coordinates: [37.6234, 55.7621] }
  },
  {
    login: "driver_volkov",
    first_name: "Виктор",
    last_name: "Волков",
    phone: "+7-900-222-0003",
    license_number: "LIC-003-MSK",
    created_at: new Date("2023-08-01T10:00:00Z"),
    is_active: true,
    status: "available",
    vehicle: { make: "Mercedes-Benz", model: "E-Class", year: 2023, plate: "В003ВВ77", color: "Чёрный", car_class: "business" },
    rating: { average: 4.95, count: 521 },
    completed_rides: 521,
    location: { type: "Point", coordinates: [37.5882, 55.7409] }
  },
  {
    login: "driver_gusev",
    first_name: "Геннадий",
    last_name: "Гусев",
    phone: "+7-900-222-0004",
    license_number: "LIC-004-MSK",
    created_at: new Date("2023-09-10T11:00:00Z"),
    is_active: true,
    status: "offline",
    vehicle: { make: "Hyundai", model: "Solaris", year: 2020, plate: "Г004ГГ77", color: "Синий", car_class: "economy" },
    rating: { average: 4.3, count: 98 },
    completed_rides: 98,
    location: { type: "Point", coordinates: [37.6512, 55.7820] }
  },
  {
    login: "driver_dmitriev",
    first_name: "Денис",
    last_name: "Дмитриев",
    phone: "+7-900-222-0005",
    license_number: "LIC-005-MSK",
    created_at: new Date("2023-10-01T08:30:00Z"),
    is_active: true,
    status: "available",
    vehicle: { make: "Volkswagen", model: "Polo", year: 2021, plate: "Д005ДД77", color: "Красный", car_class: "economy" },
    rating: { average: 4.7, count: 256 },
    completed_rides: 256,
    location: { type: "Point", coordinates: [37.6089, 55.7489] }
  },
  {
    login: "driver_egorov",
    first_name: "Евгений",
    last_name: "Егоров",
    phone: "+7-900-222-0006",
    license_number: "LIC-006-MSK",
    created_at: new Date("2023-11-15T12:00:00Z"),
    is_active: true,
    status: "available",
    vehicle: { make: "BMW", model: "5 Series", year: 2022, plate: "Е006ЕЕ77", color: "Тёмно-синий", car_class: "business" },
    rating: { average: 4.85, count: 143 },
    completed_rides: 143,
    location: { type: "Point", coordinates: [37.5714, 55.7512] }
  },
  {
    login: "driver_zhukov",
    first_name: "Жан",
    last_name: "Жуков",
    phone: "+7-900-222-0007",
    license_number: "LIC-007-MSK",
    created_at: new Date("2023-12-01T09:00:00Z"),
    is_active: true,
    status: "on_ride",
    vehicle: { make: "Skoda", model: "Octavia", year: 2021, plate: "Ж007ЖЖ77", color: "Зелёный", car_class: "comfort" },
    rating: { average: 4.55, count: 89 },
    completed_rides: 89,
    location: { type: "Point", coordinates: [37.6345, 55.7700] }
  },
  {
    login: "driver_ivanov",
    first_name: "Игорь",
    last_name: "Иванов",
    phone: "+7-900-222-0008",
    license_number: "LIC-008-MSK",
    created_at: new Date("2024-01-10T10:00:00Z"),
    is_active: true,
    status: "available",
    vehicle: { make: "Renault", model: "Logan", year: 2019, plate: "И008ИИ77", color: "Жёлтый", car_class: "economy" },
    rating: { average: 4.1, count: 45 },
    completed_rides: 45,
    location: { type: "Point", coordinates: [37.5990, 55.7305] }
  },
  {
    login: "driver_kiselev",
    first_name: "Константин",
    last_name: "Киселев",
    phone: "+7-900-222-0009",
    license_number: "LIC-009-MSK",
    created_at: new Date("2024-02-01T08:00:00Z"),
    is_active: true,
    status: "offline",
    vehicle: { make: "Ford", model: "Focus", year: 2020, plate: "К009КК77", color: "Серый", car_class: "comfort" },
    rating: { average: 4.6, count: 72 },
    completed_rides: 72,
    location: { type: "Point", coordinates: [37.6210, 55.7634] }
  },
  {
    login: "driver_loginov",
    first_name: "Леонид",
    last_name: "Логинов",
    phone: "+7-900-222-0010",
    license_number: "LIC-010-MSK",
    created_at: new Date("2024-03-01T09:30:00Z"),
    is_active: true,
    status: "available",
    vehicle: { make: "Nissan", model: "Teana", year: 2022, plate: "Л010ЛЛ77", color: "Белый", car_class: "comfort" },
    rating: { average: 4.75, count: 31 },
    completed_rides: 31,
    location: { type: "Point", coordinates: [37.6050, 55.7590] }
  },
  {
    login: "driver_makarov",
    first_name: "Михаил",
    last_name: "Макаров",
    phone: "+7-900-222-0011",
    license_number: "LIC-011-MSK",
    created_at: new Date("2024-03-15T11:00:00Z"),
    is_active: true,
    status: "available",
    vehicle: { make: "Lexus", model: "ES", year: 2023, plate: "М011ММ77", color: "Чёрный", car_class: "business" },
    rating: { average: 4.9, count: 18 },
    completed_rides: 18,
    location: { type: "Point", coordinates: [37.5832, 55.7450] }
  },
  {
    login: "driver_nikitin",
    first_name: "Николай",
    last_name: "Никитин",
    phone: "+7-900-222-0012",
    license_number: "LIC-012-MSK",
    created_at: new Date("2024-04-01T08:00:00Z"),
    is_active: false,
    status: "offline",
    vehicle: { make: "Lada", model: "Vesta", year: 2021, plate: "Н012НН77", color: "Серебристый", car_class: "economy" },
    rating: { average: 3.8, count: 12 },
    completed_rides: 12,
    location: { type: "Point", coordinates: [37.6400, 55.7750] }
  }
]);

// ─────────────────────────────────────────────
// Rides — 12 documents
// ─────────────────────────────────────────────
// We use user/driver ObjectIds by login lookup after insertions
var userIds = {};
db.users.find({}, { login: 1 }).forEach(u => { userIds[u.login] = u._id; });

var driverIds = {};
db.drivers.find({}, { login: 1 }).forEach(d => { driverIds[d.login] = d._id; });

db.rides.drop();
db.rides.insertMany([
  {
    user_id: userIds["ivan_petrov"],
    driver_id: driverIds["driver_alexeev"],
    status: "completed",
    created_at: new Date("2024-05-01T08:00:00Z"),
    accepted_at: new Date("2024-05-01T08:03:00Z"),
    completed_at: new Date("2024-05-01T08:35:00Z"),
    origin: { address: "ул. Тверская, 1", coordinates: { type: "Point", coordinates: [37.6085, 55.7648] } },
    destination: { address: "Красная площадь", coordinates: { type: "Point", coordinates: [37.6208, 55.7540] } },
    price: { estimated: 450.0, final: 440.0, currency: "RUB" },
    car_class: "comfort",
    duration_minutes: 32,
    distance_km: 5.2,
    rating: { user_to_driver: 5, driver_to_user: 5 },
    events: [
      { status: "pending", timestamp: new Date("2024-05-01T08:00:00Z"), note: null },
      { status: "accepted", timestamp: new Date("2024-05-01T08:03:00Z"), note: null },
      { status: "in_progress", timestamp: new Date("2024-05-01T08:05:00Z"), note: null },
      { status: "completed", timestamp: new Date("2024-05-01T08:35:00Z"), note: null }
    ]
  },
  {
    user_id: userIds["maria_smirnova"],
    driver_id: driverIds["driver_borisov"],
    status: "completed",
    created_at: new Date("2024-05-02T09:00:00Z"),
    accepted_at: new Date("2024-05-02T09:05:00Z"),
    completed_at: new Date("2024-05-02T09:40:00Z"),
    origin: { address: "Арбат, 10", coordinates: { type: "Point", coordinates: [37.5923, 55.7506] } },
    destination: { address: "Патриаршие пруды", coordinates: { type: "Point", coordinates: [37.5959, 55.7644] } },
    price: { estimated: 320.0, final: 315.0, currency: "RUB" },
    car_class: "economy",
    duration_minutes: 35,
    distance_km: 3.1,
    rating: { user_to_driver: 4, driver_to_user: 5 },
    events: [
      { status: "pending", timestamp: new Date("2024-05-02T09:00:00Z"), note: null },
      { status: "accepted", timestamp: new Date("2024-05-02T09:05:00Z"), note: null },
      { status: "in_progress", timestamp: new Date("2024-05-02T09:08:00Z"), note: null },
      { status: "completed", timestamp: new Date("2024-05-02T09:40:00Z"), note: null }
    ]
  },
  {
    user_id: userIds["alex_kozlov"],
    driver_id: driverIds["driver_volkov"],
    status: "completed",
    created_at: new Date("2024-05-03T10:00:00Z"),
    accepted_at: new Date("2024-05-03T10:02:00Z"),
    completed_at: new Date("2024-05-03T10:55:00Z"),
    origin: { address: "Кутузовский проспект, 36", coordinates: { type: "Point", coordinates: [37.5481, 55.7446] } },
    destination: { address: "Аэропорт Внуково", coordinates: { type: "Point", coordinates: [37.2615, 55.6008] } },
    price: { estimated: 1800.0, final: 1950.0, currency: "RUB" },
    car_class: "business",
    duration_minutes: 53,
    distance_km: 28.5,
    rating: { user_to_driver: 5, driver_to_user: 4 },
    events: [
      { status: "pending", timestamp: new Date("2024-05-03T10:00:00Z"), note: null },
      { status: "accepted", timestamp: new Date("2024-05-03T10:02:00Z"), note: null },
      { status: "in_progress", timestamp: new Date("2024-05-03T10:03:00Z"), note: null },
      { status: "completed", timestamp: new Date("2024-05-03T10:55:00Z"), note: null }
    ]
  },
  {
    user_id: userIds["ivan_petrov"],
    driver_id: driverIds["driver_dmitriev"],
    status: "completed",
    created_at: new Date("2024-05-05T07:30:00Z"),
    accepted_at: new Date("2024-05-05T07:35:00Z"),
    completed_at: new Date("2024-05-05T07:58:00Z"),
    origin: { address: "Садовая-Спасская, 1", coordinates: { type: "Point", coordinates: [37.6456, 55.7700] } },
    destination: { address: "Чистые пруды", coordinates: { type: "Point", coordinates: [37.6404, 55.7672] } },
    price: { estimated: 200.0, final: 195.0, currency: "RUB" },
    car_class: "economy",
    duration_minutes: 23,
    distance_km: 1.8,
    rating: { user_to_driver: 5, driver_to_user: 5 },
    events: [
      { status: "pending", timestamp: new Date("2024-05-05T07:30:00Z"), note: null },
      { status: "accepted", timestamp: new Date("2024-05-05T07:35:00Z"), note: null },
      { status: "in_progress", timestamp: new Date("2024-05-05T07:36:00Z"), note: null },
      { status: "completed", timestamp: new Date("2024-05-05T07:58:00Z"), note: null }
    ]
  },
  {
    user_id: userIds["olga_novikova"],
    driver_id: driverIds["driver_egorov"],
    status: "completed",
    created_at: new Date("2024-05-06T12:00:00Z"),
    accepted_at: new Date("2024-05-06T12:04:00Z"),
    completed_at: new Date("2024-05-06T12:50:00Z"),
    origin: { address: "Ленинский проспект, 90", coordinates: { type: "Point", coordinates: [37.5500, 55.7000] } },
    destination: { address: "Аэропорт Шереметьево", coordinates: { type: "Point", coordinates: [37.4146, 55.9726] } },
    price: { estimated: 2200.0, final: 2350.0, currency: "RUB" },
    car_class: "business",
    duration_minutes: 46,
    distance_km: 35.1,
    rating: { user_to_driver: 5, driver_to_user: 5 },
    events: [
      { status: "pending", timestamp: new Date("2024-05-06T12:00:00Z"), note: null },
      { status: "accepted", timestamp: new Date("2024-05-06T12:04:00Z"), note: null },
      { status: "in_progress", timestamp: new Date("2024-05-06T12:06:00Z"), note: null },
      { status: "completed", timestamp: new Date("2024-05-06T12:50:00Z"), note: null }
    ]
  },
  {
    user_id: userIds["elena_morozova"],
    driver_id: driverIds["driver_zhukov"],
    status: "completed",
    created_at: new Date("2024-05-08T16:00:00Z"),
    accepted_at: new Date("2024-05-08T16:06:00Z"),
    completed_at: new Date("2024-05-08T16:42:00Z"),
    origin: { address: "Пушкинская площадь", coordinates: { type: "Point", coordinates: [37.6059, 55.7653] } },
    destination: { address: "Парк Горького", coordinates: { type: "Point", coordinates: [37.6031, 55.7304] } },
    price: { estimated: 380.0, final: 370.0, currency: "RUB" },
    car_class: "comfort",
    duration_minutes: 36,
    distance_km: 4.8,
    rating: { user_to_driver: 4, driver_to_user: 5 },
    events: [
      { status: "pending", timestamp: new Date("2024-05-08T16:00:00Z"), note: null },
      { status: "accepted", timestamp: new Date("2024-05-08T16:06:00Z"), note: null },
      { status: "in_progress", timestamp: new Date("2024-05-08T16:08:00Z"), note: null },
      { status: "completed", timestamp: new Date("2024-05-08T16:42:00Z"), note: null }
    ]
  },
  {
    user_id: userIds["dmitry_volkov"],
    driver_id: driverIds["driver_loginov"],
    status: "completed",
    created_at: new Date("2024-05-10T08:00:00Z"),
    accepted_at: new Date("2024-05-10T08:04:00Z"),
    completed_at: new Date("2024-05-10T08:29:00Z"),
    origin: { address: "Таганская площадь", coordinates: { type: "Point", coordinates: [37.6523, 55.7415] } },
    destination: { address: "Театральная площадь", coordinates: { type: "Point", coordinates: [37.6159, 55.7572] } },
    price: { estimated: 350.0, final: 345.0, currency: "RUB" },
    car_class: "comfort",
    duration_minutes: 25,
    distance_km: 3.7,
    rating: { user_to_driver: 5, driver_to_user: 4 },
    events: [
      { status: "pending", timestamp: new Date("2024-05-10T08:00:00Z"), note: null },
      { status: "accepted", timestamp: new Date("2024-05-10T08:04:00Z"), note: null },
      { status: "in_progress", timestamp: new Date("2024-05-10T08:05:00Z"), note: null },
      { status: "completed", timestamp: new Date("2024-05-10T08:29:00Z"), note: null }
    ]
  },
  {
    user_id: userIds["anna_popova"],
    driver_id: driverIds["driver_makarov"],
    status: "completed",
    created_at: new Date("2024-05-12T19:00:00Z"),
    accepted_at: new Date("2024-05-12T19:03:00Z"),
    completed_at: new Date("2024-05-12T20:05:00Z"),
    origin: { address: "Раменки, ул. Мичуринский, 5", coordinates: { type: "Point", coordinates: [37.4810, 55.7140] } },
    destination: { address: "Московский Сити", coordinates: { type: "Point", coordinates: [37.5362, 55.7499] } },
    price: { estimated: 1100.0, final: 1200.0, currency: "RUB" },
    car_class: "business",
    duration_minutes: 62,
    distance_km: 9.4,
    rating: { user_to_driver: 5, driver_to_user: 5 },
    events: [
      { status: "pending", timestamp: new Date("2024-05-12T19:00:00Z"), note: null },
      { status: "accepted", timestamp: new Date("2024-05-12T19:03:00Z"), note: null },
      { status: "in_progress", timestamp: new Date("2024-05-12T19:05:00Z"), note: null },
      { status: "completed", timestamp: new Date("2024-05-12T20:05:00Z"), note: null }
    ]
  },
  {
    user_id: userIds["kate_lebedeva"],
    driver_id: driverIds["driver_ivanov"],
    status: "cancelled",
    created_at: new Date("2024-05-14T11:00:00Z"),
    accepted_at: null,
    completed_at: null,
    origin: { address: "Добрынинская площадь", coordinates: { type: "Point", coordinates: [37.6245, 55.7296] } },
    destination: { address: "ВДНХ", coordinates: { type: "Point", coordinates: [37.6397, 55.8282] } },
    price: { estimated: 650.0, final: null, currency: "RUB" },
    car_class: "economy",
    duration_minutes: null,
    distance_km: null,
    rating: { user_to_driver: null, driver_to_user: null },
    events: [
      { status: "pending", timestamp: new Date("2024-05-14T11:00:00Z"), note: null },
      { status: "cancelled", timestamp: new Date("2024-05-14T11:12:00Z"), note: "Пассажир отменил заказ" }
    ]
  },
  {
    user_id: userIds["julia_romanova"],
    driver_id: driverIds["driver_alexeev"],
    status: "accepted",
    created_at: new Date("2024-05-20T14:00:00Z"),
    accepted_at: new Date("2024-05-20T14:02:00Z"),
    completed_at: null,
    origin: { address: "Фрунзенская набережная, 30", coordinates: { type: "Point", coordinates: [37.5841, 55.7268] } },
    destination: { address: "Новый Арбат, 25", coordinates: { type: "Point", coordinates: [37.5872, 55.7523] } },
    price: { estimated: 420.0, final: null, currency: "RUB" },
    car_class: "comfort",
    duration_minutes: null,
    distance_km: null,
    rating: { user_to_driver: null, driver_to_user: null },
    events: [
      { status: "pending", timestamp: new Date("2024-05-20T14:00:00Z"), note: null },
      { status: "accepted", timestamp: new Date("2024-05-20T14:02:00Z"), note: null }
    ]
  },
  {
    user_id: userIds["pavel_andreev"],
    driver_id: null,
    status: "pending",
    created_at: new Date("2024-05-20T14:30:00Z"),
    accepted_at: null,
    completed_at: null,
    origin: { address: "Белорусский вокзал", coordinates: { type: "Point", coordinates: [37.5812, 55.7767] } },
    destination: { address: "Киевский вокзал", coordinates: { type: "Point", coordinates: [37.5650, 55.7443] } },
    price: { estimated: 280.0, final: null, currency: "RUB" },
    car_class: "economy",
    duration_minutes: null,
    distance_km: null,
    rating: { user_to_driver: null, driver_to_user: null },
    events: [
      { status: "pending", timestamp: new Date("2024-05-20T14:30:00Z"), note: null }
    ]
  },
  {
    user_id: userIds["sergey_fedorov"],
    driver_id: null,
    status: "pending",
    created_at: new Date("2024-05-20T14:45:00Z"),
    accepted_at: null,
    completed_at: null,
    origin: { address: "Курский вокзал", coordinates: { type: "Point", coordinates: [37.6593, 55.7577] } },
    destination: { address: "Аэропорт Домодедово", coordinates: { type: "Point", coordinates: [37.9062, 55.4108] } },
    price: { estimated: 1600.0, final: null, currency: "RUB" },
    car_class: "economy",
    duration_minutes: null,
    distance_km: null,
    rating: { user_to_driver: null, driver_to_user: null },
    events: [
      { status: "pending", timestamp: new Date("2024-05-20T14:45:00Z"), note: null }
    ]
  }
]);

// ─────────────────────────────────────────────
// Indexes
// ─────────────────────────────────────────────
db.users.createIndex({ login: 1 }, { unique: true });
db.users.createIndex({ email: 1 }, { unique: true });
db.users.createIndex({ first_name: "text", last_name: "text" });

db.drivers.createIndex({ login: 1 }, { unique: true });
db.drivers.createIndex({ license_number: 1 }, { unique: true });
db.drivers.createIndex({ status: 1 });
db.drivers.createIndex({ location: "2dsphere" });

db.rides.createIndex({ user_id: 1, created_at: -1 });
db.rides.createIndex({ driver_id: 1, status: 1 });
db.rides.createIndex({ status: 1, created_at: -1 });

print("Seed data loaded successfully.");
print("users: " + db.users.countDocuments());
print("drivers: " + db.drivers.countDocuments());
print("rides: " + db.rides.countDocuments());
