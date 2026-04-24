package com.englishlearner.server.dto.word;

import java.time.LocalDate;

public record DailyStudySummaryResponse(
        LocalDate studyDate,
        Integer newWords,
        Integer reviewWords,
        Integer correctCount,
        Integer wrongCount,
        Integer studySeconds,
        Integer scoreDelta,
        Integer completedTasks
) {
}
