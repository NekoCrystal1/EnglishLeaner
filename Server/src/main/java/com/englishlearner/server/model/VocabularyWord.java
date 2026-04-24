package com.englishlearner.server.model;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.Table;

@Entity
@Table(name = "vocabulary_word")
public class VocabularyWord extends BaseEntity {
    @Column(nullable = false, length = 128)
    private String word;

    @Column(name = "normalized_word", nullable = false, length = 128)
    private String normalizedWord;

    @Column(name = "phonetic_us", length = 128)
    private String phoneticUs;

    @Column(name = "phonetic_uk", length = 128)
    private String phoneticUk;

    @Column(nullable = false, length = 256)
    private String translation;

    @Column(name = "part_of_speech", length = 64)
    private String partOfSpeech;

    @Column(name = "example_sentence", length = 1000)
    private String exampleSentence;

    @Column(nullable = false)
    private Integer difficulty = 1;

    @Column(name = "audio_asset_id")
    private Long audioAssetId;

    public String getWord() {
        return word;
    }

    public void setWord(String word) {
        this.word = word;
    }

    public String getNormalizedWord() {
        return normalizedWord;
    }

    public void setNormalizedWord(String normalizedWord) {
        this.normalizedWord = normalizedWord;
    }

    public String getPhoneticUs() {
        return phoneticUs;
    }

    public void setPhoneticUs(String phoneticUs) {
        this.phoneticUs = phoneticUs;
    }

    public String getPhoneticUk() {
        return phoneticUk;
    }

    public void setPhoneticUk(String phoneticUk) {
        this.phoneticUk = phoneticUk;
    }

    public String getTranslation() {
        return translation;
    }

    public void setTranslation(String translation) {
        this.translation = translation;
    }

    public String getPartOfSpeech() {
        return partOfSpeech;
    }

    public void setPartOfSpeech(String partOfSpeech) {
        this.partOfSpeech = partOfSpeech;
    }

    public String getExampleSentence() {
        return exampleSentence;
    }

    public void setExampleSentence(String exampleSentence) {
        this.exampleSentence = exampleSentence;
    }

    public Integer getDifficulty() {
        return difficulty;
    }

    public void setDifficulty(Integer difficulty) {
        this.difficulty = difficulty;
    }

    public Long getAudioAssetId() {
        return audioAssetId;
    }

    public void setAudioAssetId(Long audioAssetId) {
        this.audioAssetId = audioAssetId;
    }
}
