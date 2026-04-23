package com.englishlearner.server.controller;

import com.englishlearner.server.common.ApiResponse;
import com.englishlearner.server.dto.ranking.RankingItemResponse;
import com.englishlearner.server.service.QuizService;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
@RequestMapping("/api/v1/ranking")
public class RankingController {
    private final QuizService quizService;

    public RankingController(QuizService quizService) {
        this.quizService = quizService;
    }

    @GetMapping("/top")
    public ApiResponse<List<RankingItemResponse>> top(@RequestParam(defaultValue = "10") int limit) {
        return ApiResponse.ok(quizService.topRanking(limit));
    }
}
