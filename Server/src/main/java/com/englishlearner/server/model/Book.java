package com.englishlearner.server.model;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.Table;

@Entity
@Table(name = "book")
public class Book extends BaseEntity {
    @Column(nullable = false, length = 200)
    private String title;

    @Column(name = "book_type", nullable = false, length = 32)
    private String bookType;

    @Column(name = "source_type", nullable = false, length = 32)
    private String sourceType;

    @Column(name = "owner_user_id")
    private Long ownerUserId;

    @Column(nullable = false, length = 32)
    private String language = "en";

    @Column(length = 32)
    private String level;

    @Column(name = "cover_url", length = 512)
    private String coverUrl;

    @Column(name = "item_count", nullable = false)
    private Integer itemCount = 0;

    @Column(name = "content_storage_mode", nullable = false, length = 32)
    private String contentStorageMode = "SERVER";

    @Column(name = "client_content_ref", length = 512)
    private String clientContentRef;

    @Column(name = "content_hash", length = 128)
    private String contentHash;

    @Column(nullable = false, length = 32)
    private String status = "ACTIVE";

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getBookType() {
        return bookType;
    }

    public void setBookType(String bookType) {
        this.bookType = bookType;
    }

    public String getSourceType() {
        return sourceType;
    }

    public void setSourceType(String sourceType) {
        this.sourceType = sourceType;
    }

    public Long getOwnerUserId() {
        return ownerUserId;
    }

    public void setOwnerUserId(Long ownerUserId) {
        this.ownerUserId = ownerUserId;
    }

    public String getLanguage() {
        return language;
    }

    public void setLanguage(String language) {
        this.language = language;
    }

    public String getLevel() {
        return level;
    }

    public void setLevel(String level) {
        this.level = level;
    }

    public String getCoverUrl() {
        return coverUrl;
    }

    public void setCoverUrl(String coverUrl) {
        this.coverUrl = coverUrl;
    }

    public Integer getItemCount() {
        return itemCount;
    }

    public void setItemCount(Integer itemCount) {
        this.itemCount = itemCount;
    }

    public String getContentStorageMode() {
        return contentStorageMode;
    }

    public void setContentStorageMode(String contentStorageMode) {
        this.contentStorageMode = contentStorageMode;
    }

    public String getClientContentRef() {
        return clientContentRef;
    }

    public void setClientContentRef(String clientContentRef) {
        this.clientContentRef = clientContentRef;
    }

    public String getContentHash() {
        return contentHash;
    }

    public void setContentHash(String contentHash) {
        this.contentHash = contentHash;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }
}
