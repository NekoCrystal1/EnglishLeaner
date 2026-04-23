package com.englishlearner.server.model;

import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.Table;

@Entity
@Table(name = "vocabulary")
public class Vocabulary extends BaseEntity {
    @Column(nullable = false, length = 128)
    private String word;

    @Column(nullable = false, length = 256)
    private String translation;

    @Column(length = 256)
    private String option1;

    @Column(length = 256)
    private String option2;

    @Column(length = 256)
    private String option3;

    public String getWord() {
        return word;
    }

    public void setWord(String word) {
        this.word = word;
    }

    public String getTranslation() {
        return translation;
    }

    public void setTranslation(String translation) {
        this.translation = translation;
    }

    public String getOption1() {
        return option1;
    }

    public void setOption1(String option1) {
        this.option1 = option1;
    }

    public String getOption2() {
        return option2;
    }

    public void setOption2(String option2) {
        this.option2 = option2;
    }

    public String getOption3() {
        return option3;
    }

    public void setOption3(String option3) {
        this.option3 = option3;
    }
}
