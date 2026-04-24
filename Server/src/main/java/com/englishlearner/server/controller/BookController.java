package com.englishlearner.server.controller;

import com.englishlearner.server.common.ApiResponse;
import com.englishlearner.server.common.BusinessException;
import com.englishlearner.server.dto.book.BookResponse;
import com.englishlearner.server.dto.book.BookWordResponse;
import com.englishlearner.server.dto.book.CreateBookRequest;
import com.englishlearner.server.security.AuthenticatedUser;
import com.englishlearner.server.service.BookService;
import jakarta.validation.Valid;
import org.springframework.security.core.Authentication;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
@RequestMapping("/api/v1/books")
public class BookController {
    private final BookService bookService;

    public BookController(BookService bookService) {
        this.bookService = bookService;
    }

    @GetMapping
    public ApiResponse<List<BookResponse>> list(@RequestParam(required = false) String bookType,
                                                Authentication authentication) {
        Long userId = getUserId(authentication);
        return ApiResponse.ok(bookService.listVisibleBooks(userId, bookType));
    }

    @PostMapping
    public ApiResponse<BookResponse> create(@Valid @RequestBody CreateBookRequest request,
                                            Authentication authentication) {
        Long userId = getUserId(authentication);
        return ApiResponse.ok("book created", bookService.createUserBook(userId, request));
    }

    @GetMapping("/{bookId}")
    public ApiResponse<BookResponse> get(@PathVariable Long bookId,
                                         Authentication authentication) {
        Long userId = getUserId(authentication);
        return ApiResponse.ok(bookService.getVisibleBook(userId, bookId));
    }

    @GetMapping("/{bookId}/words")
    public ApiResponse<List<BookWordResponse>> words(@PathVariable Long bookId,
                                                     Authentication authentication) {
        Long userId = getUserId(authentication);
        return ApiResponse.ok(bookService.listBookWords(userId, bookId));
    }

    private Long getUserId(Authentication authentication) {
        if (authentication == null || !(authentication.getPrincipal() instanceof AuthenticatedUser authUser)) {
            throw BusinessException.unauthorized("请先登录");
        }
        return authUser.userId();
    }
}
