package com.englishlearner.server.controller;

import com.englishlearner.server.common.ApiResponse;
import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.dto.word.CreateWordPlanRequest;
import com.englishlearner.server.dto.word.DailyStudySummaryResponse;
import com.englishlearner.server.dto.word.SubmitWordStudyEventRequest;
import com.englishlearner.server.dto.word.UpdateWordPlanRequest;
import com.englishlearner.server.dto.word.WordLearningItemResponse;
import com.englishlearner.server.dto.word.WordPlanResponse;
import com.englishlearner.server.dto.word.WordStudyEventResponse;
import com.englishlearner.server.security.AuthenticatedUser;
import com.englishlearner.server.service.WordLearningService;
import jakarta.validation.Valid;
import org.springframework.security.core.Authentication;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
@RequestMapping("/api/v1/word-learning")
public class WordLearningController {
    private final WordLearningService wordLearningService;

    public WordLearningController(WordLearningService wordLearningService) {
        this.wordLearningService = wordLearningService;
    }

    @GetMapping("/plans")
    public ApiResponse<List<WordPlanResponse>> listPlans(Authentication authentication) {
        return ApiResponse.ok(wordLearningService.listPlans(getUserId(authentication)));
    }

    @GetMapping("/plans/current")
    public ApiResponse<WordPlanResponse> currentPlan(Authentication authentication) {
        return ApiResponse.ok(wordLearningService.getOrCreateCurrentPlan(getUserId(authentication)));
    }

    @PostMapping("/plans")
    public ApiResponse<WordPlanResponse> createPlan(@Valid @RequestBody CreateWordPlanRequest request,
                                                    Authentication authentication) {
        return ApiResponse.ok("word plan created", wordLearningService.createPlan(getUserId(authentication), request));
    }

    @PutMapping("/plans/{planId}")
    public ApiResponse<WordPlanResponse> updatePlan(@PathVariable Long planId,
                                                    @Valid @RequestBody UpdateWordPlanRequest request,
                                                    Authentication authentication) {
        return ApiResponse.ok(wordLearningService.updatePlan(getUserId(authentication), planId, request));
    }

    @GetMapping("/due-words")
    public ApiResponse<List<WordLearningItemResponse>> dueWords(@RequestParam(required = false) Long planId,
                                                                Authentication authentication) {
        // 服务层会先返回到期复习词，再用词书中的新词补足数量。
        return ApiResponse.ok(wordLearningService.getDueWords(getUserId(authentication), planId));
    }

    @PostMapping("/events")
    public ApiResponse<WordStudyEventResponse> submitEvent(@Valid @RequestBody SubmitWordStudyEventRequest request,
                                                           Authentication authentication) {
        return ApiResponse.ok("study event recorded", wordLearningService.submitWordEvent(getUserId(authentication), request));
    }

    @GetMapping("/summary/today")
    public ApiResponse<DailyStudySummaryResponse> todaySummary(Authentication authentication) {
        return ApiResponse.ok(wordLearningService.getTodaySummary(getUserId(authentication)));
    }

    private Long getUserId(Authentication authentication) {
        if (authentication == null || !(authentication.getPrincipal() instanceof AuthenticatedUser authUser)) {
            throw BusinessException.unauthorized("请先登录");
        }
        return authUser.userId();
    }
}
