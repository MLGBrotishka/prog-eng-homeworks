# Integration tests for User Service
import pytest
import jwt
import time
import uuid

JWT_SECRET_KEY = 'your-jwt-secret-key-change-it-in-production'

def make_jwt_token(user_id):
    """Create a JWT token for testing"""
    payload = {
        'user_id': str(user_id),
        'iss': 'taxi-service',
        'iat': int(time.time()),
        'exp': int(time.time()) + 3600
    }
    return jwt.encode(payload, JWT_SECRET_KEY, algorithm='HS256')

async def test_ping(service_client):
    """Test that the service responds to ping"""
    response = await service_client.get('/ping')
    assert response.status == 200
    assert response.text == 'pong\n'

async def test_create_user_success(service_client):
    """Test successful user creation"""
    login = f"user_{uuid.uuid4().hex[:8]}"
    user_data = {
        'login': login,
        'email': f"{login}@example.com",
        'first_name': 'Taxi',
        'last_name': 'Passenger',
        'password': 'password123'
    }
    
    response = await service_client.post('/api/v1/users', json=user_data)
    assert response.status == 201
    
    json_response = response.json()
    assert 'id' in json_response
    assert json_response['login'] == user_data['login']
    assert json_response['first_name'] == 'Taxi'

async def test_login_success(service_client):
    """Test successful user login"""
    login = f"user_{uuid.uuid4().hex[:8]}"
    user_data = {
        'login': login,
        'email': f"{login}@example.com",
        'first_name': 'Login',
        'last_name': 'User',
        'password': 'testpassword'
    }
    await service_client.post('/api/v1/users', json=user_data)
    
    login_data = {'login': login, 'password': 'testpassword'}
    response = await service_client.post('/api/v1/auth/login', json=login_data)
    assert response.status == 200
    
    json_response = response.json()
    assert 'token' in json_response
    assert 'user_id' in json_response

async def test_search_users(service_client):
    """Test searching users"""
    login = f"user_{uuid.uuid4().hex[:8]}"
    user_data = {
        'login': login,
        'email': f"{login}@example.com",
        'first_name': 'SearchMe',
        'last_name': 'User',
        'password': 'password123'
    }
    await service_client.post('/api/v1/users', json=user_data)
    
    # Search by login
    response = await service_client.get(f'/api/v1/users?login={login}')
    assert response.status == 200
    assert response.json()['login'] == login

    # Search by name mask
    response = await service_client.get('/api/v1/users/search?mask=SearchMe')
    assert response.status == 200
    assert isinstance(response.json(), list)
    assert len(response.json()) >= 1
