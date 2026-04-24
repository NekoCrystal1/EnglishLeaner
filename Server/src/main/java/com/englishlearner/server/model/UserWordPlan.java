package com.englishlearner.server.model;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.PrePersist;
import jakarta.persistence.Table;

import java.time.LocalDate;

@Entity
@Table(name = "user_word_plan")
public class UserWordPlan extends BaseEntity {
    @Column(name = "user_id", nullable = false)
    private Long userId;

    @Column(name = "book_id", nullable = false)
    private Long bookId;

    @Column(name = "daily_new_count", nullable = false)
    private Integer dailyNewCount = 10;

    @Column(name = "daily_review_count", nullable = false)
    private Integer dailyReviewCount = 20;

    @Column(name = "current_position", nullable = false)
    private Integer currentPosition = 0;

    @Column(name = "review_algorithm", nullable = false, length = 32)
    private String reviewAlgorithm = "SM2_SIMPLE";

    @Column(name = "started_on", nullable = false)
    private LocalDate startedOn;

    @Column(name = "finished_on")
    private LocalDate finishedOn;

    @Column(nullable = false, length = 32)
    private String status = "ACTIVE";

    @PrePersist
    public void beforeInsert() {
        if (this.startedOn == null) {
            this.startedOn = LocalDate.now();
        }
    }

    public Long getUserId() {
        return userId;
    }

    public void setUserId(Long userId) {
        this.userId = userId;
    }

    public Long getBookId() {
        return bookId;
    }

    public void setBookId(Long bookId) {
        this.bookId = bookId;
    }

    public Integer getDailyNewCount() {
        return dailyNewCount;
    }

    public void setDailyNewCount(Integer dailyNewCount) {
        this.dailyNewCount = dailyNewCount;
    }

    public Integer getDailyReviewCount() {
        return dailyReviewCount;
    }

    public void setDailyReviewCount(Integer dailyReviewCount) {
        this.dailyReviewCount = dailyReviewCount;
    }

    public Integer getCurrentPosition() {
        return currentPosition;
    }

    public void setCurrentPosition(Integer currentPosition) {
        this.currentPosition = currentPosition;
    }

    public String getReviewAlgorithm() {
        return reviewAlgorithm;
    }

    public void setReviewAlgorithm(String reviewAlgorithm) {
        this.reviewAlgorithm = reviewAlgorithm;
    }

    public LocalDate getStartedOn() {
        return startedOn;
    }

    public void setStartedOn(LocalDate startedOn) {
        this.startedOn = startedOn;
    }

    public LocalDate getFinishedOn() {
        return finishedOn;
    }

    public void setFinishedOn(LocalDate finishedOn) {
        this.finishedOn = finishedOn;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }
}
