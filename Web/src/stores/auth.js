import { defineStore } from "pinia";
import { authApi, userApi } from "../api/modules";
import { TOKEN_KEY, USER_KEY } from "../api/http";

function readLocalUser() {
  const raw = localStorage.getItem(USER_KEY);
  if (!raw) {
    return null;
  }
  try {
    return JSON.parse(raw);
  } catch {
    localStorage.removeItem(USER_KEY);
    return null;
  }
}

export const useAuthStore = defineStore("auth", {
  state: () => ({
    token: localStorage.getItem(TOKEN_KEY) || "",
    user: readLocalUser(),
    loading: false
  }),
  getters: {
    isLoggedIn: (state) => Boolean(state.token),
    displayName: (state) => state.user?.displayName || state.user?.username || "学习者",
    isAdmin: (state) => {
      const roles = state.user?.roles || [];
      return state.user?.roleName === "ADMIN" || roles.includes("ADMIN");
    },
    hasPermission: (state) => (permission) => {
      const permissions = state.user?.permissions || [];
      return permissions.includes(permission);
    }
  },
  actions: {
    setToken(token) {
      this.token = token || "";
      if (token) {
        localStorage.setItem(TOKEN_KEY, token);
      } else {
        localStorage.removeItem(TOKEN_KEY);
      }
    },
    setUser(user) {
      this.user = user || null;
      if (user) {
        localStorage.setItem(USER_KEY, JSON.stringify(user));
      } else {
        localStorage.removeItem(USER_KEY);
      }
    },
    async login(credentials) {
      this.loading = true;
      try {
        const data = await authApi.login(credentials);
        this.setToken(data.token);
        this.setUser(data.user);
        return data;
      } finally {
        this.loading = false;
      }
    },
    async register(credentials) {
      this.loading = true;
      try {
        await authApi.register(credentials);
      } finally {
        this.loading = false;
      }
    },
    async registerAndLogin(credentials) {
      await this.register(credentials);
      return this.login(credentials);
    },
    async fetchMe() {
      if (!this.token) {
        return null;
      }
      try {
        const user = await userApi.me();
        this.setUser(user);
        return user;
      } catch {
        this.logout();
        return null;
      }
    },
    updateTotalScore(totalScore) {
      if (!this.user || totalScore == null) {
        return;
      }
      this.setUser({
        ...this.user,
        totalScore
      });
    },
    logout() {
      this.setToken("");
      this.setUser(null);
    }
  }
});
