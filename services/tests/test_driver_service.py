# Integration tests for Driver Service
import pytest
import jwt
import time
import uuid
import random

JWT_SECRET_KEY = 'your-jwt-secret-key-change-it-in-production'

def make_jwt_token(user_id):
    payload = {
        'user_id': str(user_id),
        'iss': 'taxi-service',
        'iat': int(time.time()),
        'exp': int(time.time()) + 3600
    }
    return jwt.encode(payload, JWT_SECRET_KEY, algorithm='HS256')

async def test_register_driver_success(service_client):
    # 1. Create a user first (needed for foreign key constraint in Postgres)
    login = f"user_{uuid.uuid4().hex[:8]}"
    user_data = {
        'login': login,
        'email': f"{login}@example.com",
        'first_name': 'Jane',
        'last_name': 'Driver',
        'password': 'password123'
    }
    user_resp = await service_client.post('/api/v1/users', json=user_data)
    assert user_resp.status == 201
    user_id = user_resp.json()['id']

    # 2. Register as driver
    token = make_jwt_token(user_id)
    headers = {'Authorization': f'Bearer {token}'}
    
    driver_data = {
        'full_name': 'Jane Driver',
        'license_plate': 'TAXI-001'
    }
    
    response = await service_client.post('/api/v1/drivers', json=driver_data, headers=headers)
    assert response.status == 201
    
    json_response = response.json()
    assert json_response['full_name'] == 'Jane Driver'
    assert json_response['license_plate'] == 'TAXI-001'
    assert json_response['status'] == 'available'
