import axios from "axios";

const API_BASE_URL = import.meta.env.VITE_API_BASE_URL || "/api/v1";
const TOKEN_KEY = "el_token";
const USER_KEY = "el_user";

export const http = axios.create({
  baseURL: API_BASE_URL,
  timeout: 10000
});

http.interceptors.request.use((config) => {
  const token = localStorage.getItem(TOKEN_KEY);
  if (token) {
    config.headers.Authorization = `Bearer ${token}`;
  }
  return config;
});

http.interceptors.response.use(
  (response) => {
    const payload = response?.data;
    if (payload && typeof payload.success === "boolean") {
      if (payload.success) {
        return payload.data;
      }
      return Promise.reject(new Error(payload.message || "请求失败"));
    }
    return payload;
  },
  (error) => {
    const status = error?.response?.status;
    const serverMessage = error?.response?.data?.message;
    const message = serverMessage || error.message || "网络异常，请稍后重试";

    if (status === 401) {
      localStorage.removeItem(TOKEN_KEY);
      localStorage.removeItem(USER_KEY);

      if (window.location.pathname !== "/auth") {
        const redirect = encodeURIComponent(`${window.location.pathname}${window.location.search}`);
        window.location.replace(`/auth?redirect=${redirect}`);
      }
    }

    return Promise.reject(new Error(message));
  }
);

export { TOKEN_KEY, USER_KEY };
