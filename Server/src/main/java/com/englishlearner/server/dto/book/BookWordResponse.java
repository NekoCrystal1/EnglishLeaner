package com.englishlearner.server.dto.book;

public record BookWordResponse(
        Long wordId,
        String word,
        String translation,
        String phoneticUs,
        String phoneticUk,
        String partOfSpeech,
        String exampleSentence,
        Integer difficulty,
        Integer unitNo,
        Integer sortOrder
) {
}
