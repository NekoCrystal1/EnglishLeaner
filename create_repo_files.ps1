# Create required src directories and repository files
$base = Get-Location
$dirs = @("src\main\java\com\example\model","src\main\java\com\example\repository","src\main\resources","src\main\resources\db\migration")
foreach ($d in $dirs) {
  $p = Join-Path $base $d
  if (-not (Test-Path $p)) { New-Item -ItemType Directory -Path $p -Force | Out-Null }
}

# Write User.java
$userJava = @'
package com.example.model;

import java.time.LocalDateTime;

public class User {
    private Integer id;
    private String username;
    private String passwordHash;
    private Integer roleId;
    private LocalDateTime createdAt;

    public User() {}

    public User(Integer id, String username, String passwordHash, Integer roleId, LocalDateTime createdAt) {
        this.id = id;
        this.username = username;
        this.passwordHash = passwordHash;
        this.roleId = roleId;
        this.createdAt = createdAt;
    }

    public Integer getId() { return id; }
    public void setId(Integer id) { this.id = id; }

    public String getUsername() { return username; }
    public void setUsername(String username) { this.username = username; }

    public String getPasswordHash() { return passwordHash; }
    public void setPasswordHash(String passwordHash) { this.passwordHash = passwordHash; }

    public Integer getRoleId() { return roleId; }
    public void setRoleId(Integer roleId) { this.roleId = roleId; }

    public LocalDateTime getCreatedAt() { return createdAt; }
    public void setCreatedAt(LocalDateTime createdAt) { this.createdAt = createdAt; }
}
'@
Set-Content -Path (Join-Path $base 'src\main\java\com\example\model\User.java') -Value $userJava -Encoding UTF8

# Write UserRepository.java
$repoInterface = @'
package com.example.repository;

import com.example.model.User;

import java.util.List;
import java.util.Optional;

public interface UserRepository {
    Optional<User> findById(int id);
    Optional<User> findByUsername(String username);
    List<User> findAll();
    int save(User user); // returns generated id
    boolean update(User user);
    boolean deleteById(int id);
}
'@
Set-Content -Path (Join-Path $base 'src\main\java\com\example\repository\UserRepository.java') -Value $repoInterface -Encoding UTF8

# Write JdbcUserRepository.java
$repoImpl = @'
package com.example.repository;

import com.example.model.User;

import javax.sql.DataSource;
import java.sql.*;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

public class JdbcUserRepository implements UserRepository {
    private final DataSource ds;

    public JdbcUserRepository(DataSource ds) {
        this.ds = ds;
    }

    private User mapRow(ResultSet rs) throws SQLException {
        User u = new User();
        u.setId(rs.getInt("id"));
        u.setUsername(rs.getString("username"));
        u.setPasswordHash(rs.getString("password_hash"));
        int role = rs.getInt("role_id");
        if (rs.wasNull()) role = 0;
        u.setRoleId(role == 0 ? null : role);
        Timestamp ts = rs.getTimestamp("created_at");
        u.setCreatedAt(ts != null ? ts.toLocalDateTime() : null);
        return u;
    }

    @Override
    public Optional<User> findById(int id) {
        String sql = "SELECT * FROM users WHERE id = ?";
        try (Connection c = ds.getConnection(); PreparedStatement ps = c.prepareStatement(sql)) {
            ps.setInt(1, id);
            try (ResultSet rs = ps.executeQuery()) {
                if (rs.next()) return Optional.of(mapRow(rs));
            }
        } catch (SQLException e) {
            throw new RuntimeException(e);
        }
        return Optional.empty();
    }

    @Override
    public Optional<User> findByUsername(String username) {
        String sql = "SELECT * FROM users WHERE username = ?";
        try (Connection c = ds.getConnection(); PreparedStatement ps = c.prepareStatement(sql)) {
            ps.setString(1, username);
            try (ResultSet rs = ps.executeQuery()) {
                if (rs.next()) return Optional.of(mapRow(rs));
            }
        } catch (SQLException e) {
            throw new RuntimeException(e);
        }
        return Optional.empty();
    }

    @Override
    public List<User> findAll() {
        String sql = "SELECT * FROM users";
        List<User> out = new ArrayList<>();
        try (Connection c = ds.getConnection(); PreparedStatement ps = c.prepareStatement(sql); ResultSet rs = ps.executeQuery()) {
            while (rs.next()) out.add(mapRow(rs));
        } catch (SQLException e) {
            throw new RuntimeException(e);
        }
        return out;
    }

    @Override
    public int save(User user) {
        String sql = "INSERT INTO users(username, password_hash, role_id) VALUES (?, ?, ?)";
        try (Connection c = ds.getConnection(); PreparedStatement ps = c.prepareStatement(sql, Statement.RETURN_GENERATED_KEYS)) {
            ps.setString(1, user.getUsername());
            ps.setString(2, user.getPasswordHash());
            if (user.getRoleId() == null) ps.setNull(3, Types.INTEGER); else ps.setInt(3, user.getRoleId());
            ps.executeUpdate();
            try (ResultSet keys = ps.getGeneratedKeys()) {
                if (keys.next()) {
                    int id = keys.getInt(1);
                    user.setId(id);
                    return id;
                }
            }
        } catch (SQLException e) {
            throw new RuntimeException(e);
        }
        return -1;
    }

    @Override
    public boolean update(User user) {
        String sql = "UPDATE users SET username = ?, password_hash = ?, role_id = ? WHERE id = ?";
        try (Connection c = ds.getConnection(); PreparedStatement ps = c.prepareStatement(sql)) {
            ps.setString(1, user.getUsername());
            ps.setString(2, user.getPasswordHash());
            if (user.getRoleId() == null) ps.setNull(3, Types.INTEGER); else ps.setInt(3, user.getRoleId());
            ps.setInt(4, user.getId());
            return ps.executeUpdate() > 0;
        } catch (SQLException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public boolean deleteById(int id) {
        String sql = "DELETE FROM users WHERE id = ?";
        try (Connection c = ds.getConnection(); PreparedStatement ps = c.prepareStatement(sql)) {
            ps.setInt(1, id);
            return ps.executeUpdate() > 0;
        } catch (SQLException e) {
            throw new RuntimeException(e);
        }
    }
}
'@
Set-Content -Path (Join-Path $base 'src\main\java\com\example\repository\JdbcUserRepository.java') -Value $repoImpl -Encoding UTF8

# Write application.properties
$appProps = @'
# Database configuration (override via environment variables in CI)
DB_URL=jdbc:postgresql://localhost:5432/yourdb
DB_USER=user
DB_PASS=pass

# Flyway locations are configured in build.gradle
'@
Set-Content -Path (Join-Path $base 'src\main\resources\application.properties') -Value $appProps -Encoding UTF8

Write-Output "Created repository skeleton and resources."
