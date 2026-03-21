# Integration tests for Driver Service
import pytest
import jwt
import time
import uuid

JWT_SECRET_KEY = 'your-jwt-secret-key-change-it-in-production'

def make_jwt_token(user_id):
    payload = {
        'user_id': str(user_id),
        'iss': 'taxi-service',
        'iat': int(time.time()),
        'exp': int(time.time()) + 3600
    }
    return jwt.encode(payload, JWT_SECRET_KEY, algorithm='HS256')

import random

async def test_register_driver_success(service_client):
    user_id = random.randint(1000, 999999)
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
