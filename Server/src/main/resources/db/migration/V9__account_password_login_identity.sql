INSERT INTO user_auth_identity (
    user_id,
    provider,
    provider_user_id,
    credential_hash,
    bound_at,
    created_at,
    updated_at,
    version,
    is_deleted
)
SELECT
    u.id,
    'PASSWORD',
    u.username,
    u.password_hash,
    CURRENT_TIMESTAMP,
    CURRENT_TIMESTAMP,
    CURRENT_TIMESTAMP,
    0,
    0
FROM users u
WHERE u.is_deleted = 0
  AND NOT EXISTS (
      SELECT 1
      FROM user_auth_identity i
      WHERE i.user_id = u.id
        AND i.provider = 'PASSWORD'
        AND i.is_deleted = 0
  );
