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
