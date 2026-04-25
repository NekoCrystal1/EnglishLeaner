import { createRouter, createWebHistory } from "vue-router";
import MainLayout from "../layouts/MainLayout.vue";
import { useAuthStore } from "../stores/auth";

const AuthPage = () => import("../pages/AuthPage.vue");
const HomePage = () => import("../pages/HomePage.vue");
const WordsPage = () => import("../pages/WordsPage.vue");
const QuizPage = () => import("../pages/QuizPage.vue");
const RankingPage = () => import("../pages/RankingPage.vue");
const AdminPage = () => import("../pages/AdminPage.vue");
const LearningPlaceholderPage = () => import("../pages/LearningPlaceholderPage.vue");

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
          path: "words",
          name: "words",
          component: WordsPage
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
        },
        {
          path: "reading",
          name: "reading",
          component: LearningPlaceholderPage
        },
        {
          path: "listening",
          name: "listening",
          component: LearningPlaceholderPage
        },
        {
          path: "speaking",
          name: "speaking",
          component: LearningPlaceholderPage
        },
        {
          path: "wrong-answers",
          name: "wrong-answers",
          component: LearningPlaceholderPage
        },
        {
          path: "favorites",
          name: "favorites",
          component: LearningPlaceholderPage
        },
        {
          path: "checkin",
          name: "checkin",
          component: LearningPlaceholderPage
        },
        {
          path: "achievements",
          name: "achievements",
          component: LearningPlaceholderPage
        },
        {
          path: "groups",
          name: "groups",
          component: LearningPlaceholderPage
        },
        {
          path: "ai",
          name: "ai",
          component: LearningPlaceholderPage
        },
        {
          path: "admin",
          name: "admin",
          component: AdminPage,
          meta: { requiresAdmin: true, permission: "ADMIN_ACCESS" }
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

    const needsFreshUser = authStore.token
      && (!authStore.user || (to.meta.permission && !authStore.hasPermission(to.meta.permission)));

    if (needsFreshUser) {
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

    if (to.meta.requiresAdmin && (!authStore.isAdmin || !authStore.hasPermission(to.meta.permission))) {
      return { name: "home" };
    }

    return true;
  });
}
