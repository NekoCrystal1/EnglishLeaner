package com.englishlearner.server.model;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.PrePersist;
import jakarta.persistence.Table;

import java.time.LocalDateTime;

@Entity
@Table(name = "word_study_event")
public class WordStudyEvent extends BaseEntity {
    @Column(name = "user_id", nullable = false)
    private Long userId;

    @Column(name = "plan_id")
    private Long planId;

    @Column(name = "book_id")
    private Long bookId;

    @Column(name = "word_id", nullable = false)
    private Long wordId;

    @Column(name = "activity_type", nullable = false, length = 32)
    private String activityType;

    @Column(name = "question_id")
    private Long questionId;

    @Column(name = "selected_answer", length = 256)
    private String selectedAnswer;

    @Column(name = "is_correct", nullable = false)
    private Boolean correct;

    @Column(name = "score_delta", nullable = false)
    private Integer scoreDelta = 0;

    @Column(name = "duration_ms")
    private Integer durationMs;

    @Column(name = "answered_at", nullable = false)
    private LocalDateTime answeredAt;

    @Column(name = "client_event_id", length = 128)
    private String clientEventId;

    @Column(name = "source_device_id")
    private Long sourceDeviceId;

    @Column(name = "client_updated_at")
    private LocalDateTime clientUpdatedAt;

    @Column(name = "sync_version", nullable = false)
    private Long syncVersion = 0L;

    @PrePersist
    public void beforeInsert() {
        if (this.answeredAt == null) {
            this.answeredAt = LocalDateTime.now();
        }
    }

    public Long getUserId() {
        return userId;
    }

    public void setUserId(Long userId) {
        this.userId = userId;
    }

    public Long getPlanId() {
        return planId;
    }

    public void setPlanId(Long planId) {
        this.planId = planId;
    }

    public Long getBookId() {
        return bookId;
    }

    public void setBookId(Long bookId) {
        this.bookId = bookId;
    }

    public Long getWordId() {
        return wordId;
    }

    public void setWordId(Long wordId) {
        this.wordId = wordId;
    }

    public String getActivityType() {
        return activityType;
    }

    public void setActivityType(String activityType) {
        this.activityType = activityType;
    }

    public Long getQuestionId() {
        return questionId;
    }

    public void setQuestionId(Long questionId) {
        this.questionId = questionId;
    }

    public String getSelectedAnswer() {
        return selectedAnswer;
    }

    public void setSelectedAnswer(String selectedAnswer) {
        this.selectedAnswer = selectedAnswer;
    }

    public Boolean getCorrect() {
        return correct;
    }

    public void setCorrect(Boolean correct) {
        this.correct = correct;
    }

    public Integer getScoreDelta() {
        return scoreDelta;
    }

    public void setScoreDelta(Integer scoreDelta) {
        this.scoreDelta = scoreDelta;
    }

    public Integer getDurationMs() {
        return durationMs;
    }

    public void setDurationMs(Integer durationMs) {
        this.durationMs = durationMs;
    }

    public LocalDateTime getAnsweredAt() {
        return answeredAt;
    }

    public void setAnsweredAt(LocalDateTime answeredAt) {
        this.answeredAt = answeredAt;
    }

    public String getClientEventId() {
        return clientEventId;
    }

    public void setClientEventId(String clientEventId) {
        this.clientEventId = clientEventId;
    }

    public Long getSourceDeviceId() {
        return sourceDeviceId;
    }

    public void setSourceDeviceId(Long sourceDeviceId) {
        this.sourceDeviceId = sourceDeviceId;
    }

    public LocalDateTime getClientUpdatedAt() {
        return clientUpdatedAt;
    }

    public void setClientUpdatedAt(LocalDateTime clientUpdatedAt) {
        this.clientUpdatedAt = clientUpdatedAt;
    }

    public Long getSyncVersion() {
        return syncVersion;
    }

    public void setSyncVersion(Long syncVersion) {
        this.syncVersion = syncVersion;
    }
}
