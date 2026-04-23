<template>
  <el-container class="app-shell">
    <el-header class="shell-header">
      <button class="brand" @click="goHome">English Learner</button>

      <el-menu
        class="nav-menu"
        mode="horizontal"
        :default-active="activePath"
        :ellipsis="false"
        @select="onSelect"
      >
        <el-menu-item index="/">学习概览</el-menu-item>
        <el-menu-item index="/quiz">刷题练习</el-menu-item>
        <el-menu-item index="/ranking">排行榜</el-menu-item>
      </el-menu>

      <div class="header-right">
        <el-tag type="success" effect="light">
          总积分 {{ authStore.user?.totalScore ?? 0 }}
        </el-tag>
        <el-dropdown>
          <span class="user-trigger">
            {{ authStore.displayName }}
            <el-icon><ArrowDown /></el-icon>
          </span>
          <template #dropdown>
            <el-dropdown-menu>
              <el-dropdown-item @click="goHome">个人概览</el-dropdown-item>
              <el-dropdown-item @click="handleLogout">退出登录</el-dropdown-item>
            </el-dropdown-menu>
          </template>
        </el-dropdown>
      </div>
    </el-header>

    <el-main class="shell-main">
      <router-view />
    </el-main>
  </el-container>
</template>

<script setup>
import { computed } from "vue";
import { useRoute, useRouter } from "vue-router";
import { useAuthStore } from "../stores/auth";

const route = useRoute();
const router = useRouter();
const authStore = useAuthStore();

const activePath = computed(() => {
  if (route.path.startsWith("/ranking")) {
    return "/ranking";
  }
  if (route.path.startsWith("/quiz")) {
    return "/quiz";
  }
  return "/";
});

function onSelect(path) {
  router.push(path);
}

function goHome() {
  router.push("/");
}

function handleLogout() {
  authStore.logout();
  router.replace("/auth");
}
</script>

<style scoped>
.app-shell {
  min-height: 100vh;
}

.shell-header {
  display: grid;
  grid-template-columns: auto 1fr auto;
  align-items: center;
  gap: 16px;
  padding: 0 20px;
  background: rgba(255, 255, 255, 0.85);
  border-bottom: 1px solid #e5e7eb;
  backdrop-filter: blur(6px);
}

.brand {
  border: none;
  background: transparent;
  padding: 0;
  font-size: 20px;
  font-weight: 800;
  color: #1d4ed8;
  cursor: pointer;
}

.nav-menu {
  min-width: 0;
}

.header-right {
  display: flex;
  align-items: center;
  gap: 12px;
  white-space: nowrap;
}

.user-trigger {
  display: inline-flex;
  align-items: center;
  gap: 6px;
  cursor: pointer;
  color: #111827;
}

.shell-main {
  padding: 24px;
}

@media (max-width: 960px) {
  .shell-header {
    grid-template-columns: 1fr;
    gap: 4px;
    height: auto;
    padding: 10px 12px;
  }

  .nav-menu {
    order: 3;
  }

  .header-right {
    justify-content: space-between;
  }

  .shell-main {
    padding: 14px;
  }
}
</style>
