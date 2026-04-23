package com.englishlearner.server.security;

import io.jsonwebtoken.Claims;
import io.jsonwebtoken.Jwts;
import io.jsonwebtoken.SignatureAlgorithm;
import io.jsonwebtoken.security.Keys;
import jakarta.annotation.PostConstruct;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import javax.crypto.SecretKey;
import java.nio.charset.StandardCharsets;
import java.time.Instant;
import java.util.Date;
import java.util.Optional;

@Service
public class JwtService {
    @Value("${app.security.jwt-secret}")
    private String secret;

    @Value("${app.security.token-expire-seconds:86400}")
    private long tokenExpireSeconds;

    private SecretKey secretKey;

    @PostConstruct
    public void init() {
        byte[] keyBytes = secret.getBytes(StandardCharsets.UTF_8);
        if (keyBytes.length < 32) {
            throw new IllegalStateException("JWT 密钥长度必须 >= 32 字节");
        }
        this.secretKey = Keys.hmacShaKeyFor(keyBytes);
    }

    public String generateToken(Long userId, String username) {
        Instant now = Instant.now();
        Instant expireAt = now.plusSeconds(tokenExpireSeconds);

        return Jwts.builder()
                .setSubject(username)
                .claim("uid", userId)
                .setIssuedAt(Date.from(now))
                .setExpiration(Date.from(expireAt))
                .signWith(secretKey, SignatureAlgorithm.HS256)
                .compact();
    }

    public Optional<AuthenticatedUser> parseToken(String token) {
        try {
            Claims claims = Jwts.parserBuilder()
                    .setSigningKey(secretKey)
                    .build()
                    .parseClaimsJws(token)
                    .getBody();

            Object uidClaim = claims.get("uid");
            Long userId;
            if (uidClaim instanceof Number number) {
                userId = number.longValue();
            } else {
                userId = Long.parseLong(String.valueOf(uidClaim));
            }

            String username = claims.getSubject();
            if (username == null || username.isBlank()) {
                return Optional.empty();
            }
            return Optional.of(new AuthenticatedUser(userId, username));
        } catch (Exception ignored) {
            return Optional.empty();
        }
    }
}
