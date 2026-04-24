<template>
  <el-container class="app-shell">
    <el-header class="shell-header">
      <button class="brand" type="button" @click="goHome">
        <span class="brand-mark">EL</span>
        <span>English Learner</span>
      </button>

      <el-menu
        class="nav-menu"
        mode="horizontal"
        :default-active="activePath"
        :ellipsis="false"
        @select="onSelect"
      >
        <el-menu-item index="/">
          <el-icon><House /></el-icon>
          <span>概览</span>
        </el-menu-item>
        <el-menu-item index="/words">
          <el-icon><Collection /></el-icon>
          <span>单词计划</span>
        </el-menu-item>
        <el-menu-item index="/quiz">
          <el-icon><EditPen /></el-icon>
          <span>刷题</span>
        </el-menu-item>
        <el-menu-item index="/ranking">
          <el-icon><Trophy /></el-icon>
          <span>排行榜</span>
        </el-menu-item>
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
              <el-dropdown-item @click="goHome">学习概览</el-dropdown-item>
              <el-dropdown-item @click="goWords">单词计划</el-dropdown-item>
              <el-dropdown-item divided @click="handleLogout">退出登录</el-dropdown-item>
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
import { ArrowDown, Collection, EditPen, House, Trophy } from "@element-plus/icons-vue";
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
  if (route.path.startsWith("/words")) {
    return "/words";
  }
  return "/";
});

function onSelect(path) {
  router.push(path);
}

function goHome() {
  router.push("/");
}

function goWords() {
  router.push("/words");
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
  grid-template-columns: auto minmax(0, 1fr) auto;
  align-items: center;
  gap: 16px;
  height: auto;
  min-height: 64px;
  padding: 0 20px;
  background: rgba(255, 255, 255, 0.92);
  border-bottom: 1px solid #e5e7eb;
  backdrop-filter: blur(8px);
  position: sticky;
  top: 0;
  z-index: 20;
}

.brand {
  display: inline-flex;
  align-items: center;
  gap: 10px;
  border: none;
  background: transparent;
  padding: 0;
  font-size: 20px;
  font-weight: 800;
  color: #1f2937;
  cursor: pointer;
  white-space: nowrap;
}

.brand-mark {
  display: inline-grid;
  place-items: center;
  width: 34px;
  height: 34px;
  border-radius: 8px;
  color: #ffffff;
  background: #2563eb;
  font-size: 14px;
  letter-spacing: 0;
}

.nav-menu {
  min-width: 0;
  border-bottom: none;
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
  width: min(1180px, 100%);
  margin: 0 auto;
  padding: 24px;
}

@media (max-width: 960px) {
  .shell-header {
    grid-template-columns: 1fr;
    gap: 6px;
    padding: 10px 12px;
  }

  .nav-menu {
    order: 3;
    overflow-x: auto;
  }

  .header-right {
    justify-content: space-between;
  }

  .shell-main {
    padding: 14px;
  }
}
</style>
