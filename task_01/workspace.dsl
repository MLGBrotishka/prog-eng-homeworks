workspace {
    name "Система заказа такси"
    description "Сервис для заказа и управления поездками на такси (аналог Uber)"

    !identifiers hierarchical

    model {

        passenger = person "Пассажир" "Пользователь, заказывающий поездку"
        driver = person "Водитель" "Водитель, выполняющий заказы на поездки"

        mapService = softwareSystem "Картографический сервис" {
            description "Внешний сервис маршрутизации и геоданных (Yandex Maps, Google Maps)"
            tags "ExternalSystem"
        }
        paymentSystem = softwareSystem "Платежная система" {
            description "Внешняя система обработки платежей по банковским картам"
            tags "ExternalSystem"
        }
        pushService = softwareSystem "Push-сервис" {
            description "Сервис push-уведомлений (FCM / APNs)"
            tags "ExternalSystem"
        }

        taxiSystem = softwareSystem "Taxi System" "Система заказа и управления поездками" {

            # Клиентские приложения
            group "Клиентские приложения" {
                passengerApp = container "Мобильное приложение пассажира" {
                    description "Приложение для заказа поездок и отслеживания водителя"
                    technology "React Native"
                    tags "MobileApp"
                }
                driverApp = container "Мобильное приложение водителя" {
                    description "Приложение для принятия заказов и навигации"
                    technology "React Native"
                    tags "MobileApp"
                }
            }


            # API Gateway Group
            group "API Gateway" {
                apiGateway = container "API Gateway" {
                    description "Единая точка входа. Выполняет маршрутизацию, аутентификацию и Rate Limiting."
                    technology "C++, Userver"
                }
                apiGatewayRedis = container "API Gateway Redis" {
                    description "Кеширование данных, Rate Limiting профилей"
                    technology "Redis"
                    tags "Database"
                }
                apiGateway -> apiGatewayRedis "Проверка Rate Limiting профилей" "TCP :6379"
            }

            passengerApp -> apiGateway "REST запросы пассажира" "HTTPS/JSON"
            driverApp -> apiGateway "REST запросы водителя" "HTTPS/JSON"
            apiGateway -> passengerApp "Отправка GQL/WS уведомлений" "WSS/JSON"
            apiGateway -> driverApp "Отправка GQL/WS уведомлений" "WSS/JSON"

            # Микросервис пользователей
            group "User Microservice" {
                userService = container "User Service" {
                    description "Управление пользователями: регистрация, аутентификация, профиль"
                    technology "C++, Userver"
                }
                userDb = container "User Database" {
                    description "Реляционное хранение данных пользователей"
                    technology "PostgreSQL"
                    tags "Database"
                }
                userService -> userDb "Чтение/запись данных" "SQL/TCP :5432"
            }

            # Микросервис водителей
            group "Driver Microservice" {
                driverService = container "Driver Service" {
                    description "Регистрация водителей, управление их статусами (CQRS Read/Write)"
                    technology "C++, Userver"
                }
                driverDb = container "Driver Database" {
                    description "Реляционное хранение данных водителей"
                    technology "PostgreSQL"
                    tags "Database"
                }
                driverCache = container "Driver Cache" {
                    description "Кеширование статуса и геопозиции водителя (GEO)"
                    technology "Redis"
                    tags "Database"
                }
                driverService -> driverDb "Чтение/запись данных" "SQL/TCP :5432"
                driverService -> driverCache "Кеширование статуса и геопозиции водителя (GEO)" "TCP :6379"
            }

            # Микросервис поездок
            group "Ride Microservice" {
                rideService = container "Ride Service" {
                    description "Управление жизненным циклом поездки (Создание, принятие, завершение)"
                    technology "C++, Userver"
                }
                rideDb = container "Ride Database" {
                    description "Хранение основных данных о поездках и транзакциях"
                    technology "PostgreSQL"
                    tags "Database"
                }
                rideHistoryDb = container "Ride History Database" {
                    description "Хранение истории геопозиций, логов и сложных документов поездки"
                    technology "MongoDB"
                    tags "Database"
                }
                rideService -> rideDb "Запись заказов и стейтов" "SQL/TCP :5432"
                rideService -> rideHistoryDb "Запись истории гео-трекинга (NoSQL)" "MQL/TCP :27017"
            }

            # Микросервис уведомлений
            group "Notification Microservice" {
                notificationService = container "Notification Service" {
                    description "Подписывается на события из Message Broker и отправляет уведомления пассажирам и водителям (push, WebSocket)"
                    technology "C++, Userver"
                }
                messageBroker = container "Message Broker" {
                    description "Центральная шина Event-Driven архитектуры. Асинхронная коммуникация между микросервисами."
                    technology "RabbitMQ / Apache Kafka"
                    tags "MessageBroker"
                }
                messageBroker -> notificationService "Подписка на все бизнес-события" "AMQP"
            }

            # Маршрутизация REST через API Gateway
            apiGateway -> userService "Запросы по пользователям" "REST/HTTP"
            apiGateway -> driverService "Запросы по водителям" "REST/HTTP"
            apiGateway -> rideService "Запросы по поездкам" "REST/HTTP"

            # Прямые вызовы сервисов к внешним системам
            rideService -> mapService "Расчет маршрута и времени" "HTTPS/JSON"
            rideService -> paymentSystem "Холдирование и списание средств" "HTTPS/JSON"

            # Асинхронное взаимодействие
            rideService -> messageBroker "Публикует: RideCreated, RideAccepted, RideCompleted" "AMQP"
            driverService -> messageBroker "Публикует: DriverStatusChanged" "AMQP"

            # Notification Service отправляет push-уведомления через внешний Push-сервис
            notificationService -> pushService "Отправка push-уведомлений пассажирам и водителям" "HTTPS/JSON"

            # Синхронные вызовы между сервисами
            rideService -> userService "RPC запрос данных пассажира" "REST/HTTP"
            rideService -> driverService "RPC запрос данных водителя" "REST/HTTP"

            paymentSystem -> apiGateway "Подтверждение оплаты (webhook)" "HTTPS/JSON"
        }

        passenger -> taxiSystem "Заказывает поездки"
        driver -> taxiSystem "Принимает и выполняет заказы"
        taxiSystem -> mapService "Получает маршруты и геоданные"
        taxiSystem -> paymentSystem "Обработка платежей"
        taxiSystem -> pushService "Отправка push-уведомлений"
        passenger -> paymentSystem "Оплачивает поездки"

        passenger -> taxiSystem.passengerApp "Использует приложение"
        driver -> taxiSystem.driverApp "Использует приложение"
    }

    views {

        themes default

        systemContext taxiSystem "SystemContext" {
            include *
            autoLayout
        }

        container taxiSystem "Containers" {
            include *
            autoLayout
        }

        dynamic taxiSystem "UC01" "Создание заказа, бронирование и принятие поездки" {
            autoLayout lr
            # Оценка стоимости
            passenger -> taxiSystem.passengerApp "Указывает адрес"
            taxiSystem.passengerApp -> taxiSystem.apiGateway "Запрос оценки стоимости"
            taxiSystem.rideService -> mapService "Расчет маршрута и времени"
            taxiSystem.rideService -> taxiSystem.driverService "Поиск ближайших водителей"
            taxiSystem.driverService -> taxiSystem.driverCache "GEORADIUS: поиск водителей рядом"
            taxiSystem.apiGateway -> taxiSystem.passengerApp "Возврат стоимости и времени"

            # Создание заказа и холдирование
            passenger -> taxiSystem.passengerApp "Подтверждение заказа"
            taxiSystem.passengerApp -> taxiSystem.apiGateway "Создание заказа"
            taxiSystem.rideService -> paymentSystem "Запрос холдирования"
            taxiSystem.rideService -> taxiSystem.rideDb "Сохранение заказа (статус PENDING)"
            taxiSystem.rideService -> taxiSystem.rideHistoryDb "Логирование создания (MongoDB)"

            # Подтверждение оплаты
            taxiSystem.passengerApp -> taxiSystem.apiGateway "Пассажир подтверждает оплату"
            taxiSystem.rideService -> paymentSystem "Выполнение транзакции"
            paymentSystem -> taxiSystem.apiGateway "Webhook: подтверждение бронирования"
            taxiSystem.rideService -> taxiSystem.rideDb "Обновление статуса (BOOKED)"

            # Уведомление водителей
            taxiSystem.rideService -> taxiSystem.messageBroker "Событие RideCreated"
            taxiSystem.messageBroker -> taxiSystem.notificationService "Получение события RideCreated"
            taxiSystem.notificationService -> pushService "Push-уведомление водителю"

            # Принятие заказа водителем
            driver -> taxiSystem.driverApp "Принимает заказ"
            taxiSystem.driverApp -> taxiSystem.apiGateway "Принять заказ"
            taxiSystem.rideService -> taxiSystem.driverService "Получение данных водителя"
            taxiSystem.rideService -> taxiSystem.rideDb "Обновление статуса (ACCEPTED)"
            taxiSystem.rideService -> taxiSystem.rideHistoryDb "Лог принятия заказа (MongoDB)"

            # Уведомление пассажира
            taxiSystem.rideService -> taxiSystem.messageBroker "Событие RideAccepted"
            taxiSystem.messageBroker -> taxiSystem.notificationService "Получение события RideAccepted"
            taxiSystem.notificationService -> pushService "Push-уведомление пассажиру"
        }

        styles {
            element "Person" {
                color #ffffff
                fontSize 22
                shape Person
            }
            element "Software System" {
                background #1168bd
                color #ffffff
            }
            element "Container" {
                background #438dd5
                color #ffffff
            }
            element "Database" {
                shape Cylinder
            }
            element "MobileApp" {
                shape MobileDevicePortrait
            }
            element "ExternalSystem" {
                background #999999
                color #ffffff
            }
            element "MessageBroker" {
                shape Pipe
                background #ff9900
                color #ffffff
            }
        }
    }
}
