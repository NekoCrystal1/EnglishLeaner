import { createRouter, createWebHistory } from "vue-router";
import MainLayout from "../layouts/MainLayout.vue";
import { useAuthStore } from "../stores/auth";

const AuthPage = () => import("../pages/AuthPage.vue");
const HomePage = () => import("../pages/HomePage.vue");
const QuizPage = () => import("../pages/QuizPage.vue");
const RankingPage = () => import("../pages/RankingPage.vue");

export const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: "/auth",
      name: "auth",
      component: AuthPage
    },
    {
      path: "/",
      component: MainLayout,
      meta: { requiresAuth: true },
      children: [
        {
          path: "",
          name: "home",
          component: HomePage
        },
        {
          path: "quiz",
          name: "quiz",
          component: QuizPage
        },
        {
          path: "ranking",
          name: "ranking",
          component: RankingPage
        }
      ]
    },
    {
      path: "/:pathMatch(.*)*",
      redirect: "/"
    }
  ]
});

export function setupRouterGuard(targetRouter, pinia) {
  targetRouter.beforeEach(async (to) => {
    const authStore = useAuthStore(pinia);

    if (authStore.token && !authStore.user) {
      await authStore.fetchMe();
    }

    if (to.meta.requiresAuth && !authStore.isLoggedIn) {
      return {
        name: "auth",
        query: {
          redirect: to.fullPath
        }
      };
    }

    if (to.name === "auth" && authStore.isLoggedIn) {
      return { name: "home" };
    }

    return true;
  });
}
