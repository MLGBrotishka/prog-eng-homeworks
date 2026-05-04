import pytest
import uuid
import time
import jwt

JWT_SECRET_KEY = 'your-jwt-secret-key-change-it-in-production'

def make_jwt_token(user_id):
    payload = {
        'user_id': str(user_id),
        'iss': 'taxi-service',
        'iat': int(time.time()),
        'exp': int(time.time()) + 3600
    }
    return jwt.encode(payload, JWT_SECRET_KEY, algorithm='HS256')

async def create_test_user(service_client):
    login = f"user_{uuid.uuid4().hex[:8]}"
    user_data = {
        'login': login,
        'email': f"{login}@example.com",
        'first_name': 'Test',
        'last_name': 'Performance',
        'password': 'password123'
    }
    await service_client.post('/api/v1/users', json=user_data)
    return login, user_data

async def test_rate_limit_auth_login(service_client):
    login, _ = await create_test_user(service_client)
    login_data = {'login': login, 'password': 'password123'}
    headers = {'X-Real-IP': '10.0.0.1'}
    
    # The limit is 5 requests per 60 seconds per IP
    for i in range(5):
        resp = await service_client.post('/api/v1/auth/login', json=login_data, headers=headers)
        assert resp.status == 200, f"Failed on request {i+1}"
        assert 'X-RateLimit-Limit' in resp.headers
        assert 'X-RateLimit-Remaining' in resp.headers
        assert int(resp.headers['X-RateLimit-Remaining']) == 5 - (i + 1)
    
    # The 6th request should fail
    resp = await service_client.post('/api/v1/auth/login', json=login_data, headers=headers)
    assert resp.status == 429
    assert 'X-RateLimit-Remaining' in resp.headers
    assert int(resp.headers['X-RateLimit-Remaining']) == 0

async def test_rate_limit_create_ride(service_client):
    passenger_id = 1001
    token = make_jwt_token(passenger_id)
    headers = {'Authorization': f'Bearer {token}'}
    ride_data = {'from_address': 'A', 'to_address': 'B'}
    
    # The limit is 10 requests per 60 seconds per passenger
    for i in range(10):
        resp = await service_client.post('/api/v1/rides', json=ride_data, headers=headers)
        assert resp.status == 201, f"Failed on request {i+1}"
        assert 'X-RateLimit-Remaining' in resp.headers
        assert int(resp.headers['X-RateLimit-Remaining']) == 10 - (i + 1)
        
    # The 11th request should fail
    resp = await service_client.post('/api/v1/rides', json=ride_data, headers=headers)
    assert resp.status == 429
    assert int(resp.headers['X-RateLimit-Remaining']) == 0

async def test_rate_limit_list_active_rides(service_client):
    driver_id = 2001
    token = make_jwt_token(driver_id)
    headers = {'Authorization': f'Bearer {token}', 'X-Real-IP': '10.0.0.2'}
    
    # The limit is 30 requests per 60 seconds per IP
    for i in range(30):
        resp = await service_client.get('/api/v1/rides?status=active', headers=headers)
        assert resp.status == 200, f"Failed on request {i+1}"
        assert 'X-RateLimit-Remaining' in resp.headers
        assert int(resp.headers['X-RateLimit-Remaining']) == 30 - (i + 1)
        
    # The 31st request should fail
    resp = await service_client.get('/api/v1/rides?status=active', headers=headers)
    assert resp.status == 429
    assert int(resp.headers['X-RateLimit-Remaining']) == 0

async def test_cache_user_profile(service_client):
    """Test that fetching user profile works normally (hits cache behind the scenes)"""
    login, _ = await create_test_user(service_client)
    
    # First fetch (cache miss)
    resp1 = await service_client.get(f'/api/v1/users?login={login}')
    assert resp1.status == 200
    
    # Second fetch (cache hit)
    resp2 = await service_client.get(f'/api/v1/users?login={login}')
    assert resp2.status == 200
    assert resp1.json() == resp2.json()

async def test_cache_active_rides(service_client):
    driver_id = 2002
    token = make_jwt_token(driver_id)
    headers = {'Authorization': f'Bearer {token}', 'X-Real-IP': '10.0.0.3'}
    
    # Need to wait for previous tests' rate limits if they used the same IP. 
    # Miss
    resp1 = await service_client.get('/api/v1/rides?status=active', headers=headers)
    assert resp1.status == 200
    
    # Hit
    resp2 = await service_client.get('/api/v1/rides?status=active', headers=headers)
    assert resp2.status == 200
    assert resp1.json() == resp2.json()
