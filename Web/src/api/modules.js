import { http } from "./http";

export const authApi = {
  register(payload) {
    return http.post("/auth/register", payload);
  },
  login(payload) {
    return http.post("/auth/login", payload);
  }
};

export const userApi = {
  me() {
    return http.get("/users/me");
  }
};

export const quizApi = {
  next() {
    return http.get("/quiz/next");
  },
  submit(payload) {
    return http.post("/quiz/submit", payload);
  }
};

export const rankingApi = {
  top(limit = 10) {
    return http.get("/ranking/top", {
      params: { limit }
    });
  }
};

export const bookApi = {
  list(bookType) {
    return http.get("/books", {
      params: bookType ? { bookType } : {}
    });
  },
  create(payload) {
    return http.post("/books", payload);
  },
  detail(bookId) {
    return http.get(`/books/${bookId}`);
  },
  words(bookId) {
    return http.get(`/books/${bookId}/words`);
  }
};

export const wordLearningApi = {
  listPlans() {
    return http.get("/word-learning/plans");
  },
  currentPlan() {
    return http.get("/word-learning/plans/current");
  },
  createPlan(payload) {
    return http.post("/word-learning/plans", payload);
  },
  updatePlan(planId, payload) {
    return http.put(`/word-learning/plans/${planId}`, payload);
  },
  dueWords(planId) {
    return http.get("/word-learning/due-words", {
      params: planId ? { planId } : {}
    });
  },
  submitEvent(payload) {
    return http.post("/word-learning/events", payload);
  },
  todaySummary() {
    return http.get("/word-learning/summary/today");
  }
};

export const adminApi = {
  overview() {
    return http.get("/admin/overview");
  },
  users(params = {}) {
    return http.get("/admin/users", { params });
  },
  roles() {
    return http.get("/admin/roles");
  },
  updateUserStatus(userId, status) {
    return http.put(`/admin/users/${userId}/status`, { status });
  },
  updateUserRole(userId, roleCode) {
    return http.put(`/admin/users/${userId}/role`, { roleCode });
  }
};
