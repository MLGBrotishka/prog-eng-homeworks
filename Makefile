.PHONY: up down build logs test swagger clean help

# Default target
help:
	@echo "Taxi Service Management Commands:"
	@echo "  make build   - Build and start services in Docker containers"
	@echo "  make up      - Start services in Docker containers"
	@echo "  make down    - Stop and remove containers"
	@echo "  make logs    - Show real-time logs from services"
	@echo "  make test    - Run integration tests (requires Python)"
	@echo "  make swagger - Open Swagger UI in your browser"
	@echo "  make clean   - Remove containers, volumes and build artifacts"

# Start the services in containers
up:
	cd services && docker-compose up -d

# Stop and remove containers
down:
	cd services && docker-compose down

# Build and start services
build:
	cd services && docker-compose up --build -d

# Show logs from all services
logs:
	cd services && docker-compose logs -f

# Run integration tests
test:
	@echo "Running tests..."
	cd services/tests && pip install -q -r requirements.txt pytest-asyncio && SERVICE_URL=http://localhost:8080 pytest

# Open Swagger UI in the default browser
swagger:
	@echo "Opening Swagger UI at http://localhost:8080/swagger/index.html"
	@if command -v xdg-open > /dev/null; then \
		xdg-open http://localhost:8080/swagger/index.html; \
	elif command -v open > /dev/null; then \
		open http://localhost:8080/swagger/index.html; \
	else \
		echo "Please open http://localhost:8080/swagger/index.html in your browser"; \
	fi

# Clean up build artifacts and database
clean:
	cd services && docker-compose down -v
	rm -rf services/build
	@echo "Cleaned up successfully."
