package com.englishlearner.server.controller;

import com.englishlearner.server.common.ApiResponse;
import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.dto.quiz.QuizQuestionResponse;
import com.englishlearner.server.dto.quiz.SubmitAnswerRequest;
import com.englishlearner.server.dto.quiz.SubmitAnswerResponse;
import com.englishlearner.server.security.AuthenticatedUser;
import com.englishlearner.server.service.QuizService;
import jakarta.validation.Valid;
import org.springframework.security.core.Authentication;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/api/v1/quiz")
public class QuizController {
    private final QuizService quizService;

    public QuizController(QuizService quizService) {
        this.quizService = quizService;
    }

    @GetMapping("/next")
    public ApiResponse<QuizQuestionResponse> next() {
        return ApiResponse.ok(quizService.nextQuestion());
    }

    @PostMapping("/submit")
    public ApiResponse<SubmitAnswerResponse> submit(@Valid @RequestBody SubmitAnswerRequest request,
                                                    Authentication authentication) {
        Long userId = getUserId(authentication);
        return ApiResponse.ok(quizService.submitAnswer(userId, request));
    }

    private Long getUserId(Authentication authentication) {
        if (authentication == null || !(authentication.getPrincipal() instanceof AuthenticatedUser authUser)) {
            throw BusinessException.unauthorized("请先登录");
        }
        return authUser.userId();
    }
}
