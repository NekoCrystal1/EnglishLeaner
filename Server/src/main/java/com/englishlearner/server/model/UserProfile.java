package com.englishlearner.server.model;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.Table;

@Entity
@Table(name = "user_profile")
public class UserProfile extends BaseEntity {
    @Column(name = "user_id", nullable = false, unique = true)
    private Long userId;

    @Column(name = "english_level", length = 32)
    private String englishLevel;

    @Column(name = "target_level", length = 32)
    private String targetLevel;

    @Column(name = "daily_minutes", nullable = false)
    private Integer dailyMinutes = 30;

    @Column(nullable = false, length = 64)
    private String timezone = "Asia/Shanghai";

    @Column(name = "preference_json", length = 4000)
    private String preferenceJson;

    public Long getUserId() {
        return userId;
    }

    public void setUserId(Long userId) {
        this.userId = userId;
    }

    public String getEnglishLevel() {
        return englishLevel;
    }

    public void setEnglishLevel(String englishLevel) {
        this.englishLevel = englishLevel;
    }

    public String getTargetLevel() {
        return targetLevel;
    }

    public void setTargetLevel(String targetLevel) {
        this.targetLevel = targetLevel;
    }

    public Integer getDailyMinutes() {
        return dailyMinutes;
    }

    public void setDailyMinutes(Integer dailyMinutes) {
        this.dailyMinutes = dailyMinutes;
    }

    public String getTimezone() {
        return timezone;
    }

    public void setTimezone(String timezone) {
        this.timezone = timezone;
    }

    public String getPreferenceJson() {
        return preferenceJson;
    }

    public void setPreferenceJson(String preferenceJson) {
        this.preferenceJson = preferenceJson;
    }
}
