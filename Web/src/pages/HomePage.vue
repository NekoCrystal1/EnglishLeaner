<template>
  <div>
    <h1 class="page-title">学习概览</h1>
    <p class="page-subtitle">查看当前账号学习状态，并快速进入刷题或排行榜。</p>

    <div class="grid-2">
      <el-card v-loading="loading">
        <template #header>
          <div class="card-title">
            <span>个人信息</span>
            <el-button text type="primary" @click="loadData">刷新</el-button>
          </div>
        </template>
        <el-descriptions :column="1" border>
          <el-descriptions-item label="用户 ID">
            {{ authStore.user?.userId ?? "-" }}
          </el-descriptions-item>
          <el-descriptions-item label="用户名">
            {{ authStore.user?.username ?? "-" }}
          </el-descriptions-item>
          <el-descriptions-item label="总积分">
            {{ authStore.user?.totalScore ?? 0 }}
          </el-descriptions-item>
          <el-descriptions-item label="学习天数">
            {{ authStore.user?.studyDays ?? 0 }}
          </el-descriptions-item>
          <el-descriptions-item label="当前排名">
            <el-tag v-if="myRank" type="warning">第 {{ myRank }} 名</el-tag>
            <span v-else>未进入榜单</span>
          </el-descriptions-item>
        </el-descriptions>
      </el-card>

      <el-card>
        <template #header>
          <span>快捷入口</span>
        </template>
        <div class="action-list">
          <el-button type="primary" size="large" @click="goQuiz">开始刷题</el-button>
          <el-button type="success" size="large" @click="goRanking">查看排行榜</el-button>
        </div>
        <el-divider />
        <div class="tips">
          <p>1. 每答对一道题可获得 10 分。</p>
          <p>2. 排行榜可查看 Top 10 / 20 / 50。</p>
          <p>3. 登录态基于 JWT，失效后会自动跳转登录页。</p>
        </div>
      </el-card>
    </div>

    <el-card style="margin-top: 16px;">
      <template #header>
        <span>Top 3 预览</span>
      </template>
      <el-empty v-if="topThree.length === 0" description="暂无排行数据" />
      <div v-else class="rank-preview">
        <el-tag
          v-for="item in topThree"
          :key="item.userId"
          :type="item.userId === authStore.user?.userId ? 'danger' : 'info'"
          effect="light"
          class="rank-tag"
        >
          #{{ item.rank }} {{ item.username }} ({{ item.totalScore }} 分)
        </el-tag>
      </div>
    </el-card>
  </div>
</template>

<script setup>
import { onMounted, ref } from "vue";
import { ElMessage } from "element-plus";
import { useRouter } from "vue-router";
import { rankingApi } from "../api/modules";
import { useAuthStore } from "../stores/auth";

const router = useRouter();
const authStore = useAuthStore();

const loading = ref(false);
const myRank = ref(0);
const topThree = ref([]);

async function loadData() {
  loading.value = true;
  try {
    await authStore.fetchMe();
    const rankings = await rankingApi.top(50);
    topThree.value = rankings.slice(0, 3);
    const mine = rankings.find((item) => item.userId === authStore.user?.userId);
    myRank.value = mine?.rank || 0;
  } catch (error) {
    ElMessage.error(error.message || "加载数据失败");
  } finally {
    loading.value = false;
  }
}

function goQuiz() {
  router.push("/quiz");
}

function goRanking() {
  router.push("/ranking");
}

onMounted(loadData);
</script>

<style scoped>
.card-title {
  display: flex;
  align-items: center;
  justify-content: space-between;
}

.action-list {
  display: flex;
  gap: 12px;
  flex-wrap: wrap;
}

.tips p {
  margin: 0 0 8px;
  color: #4b5563;
}

.rank-preview {
  display: flex;
  gap: 10px;
  flex-wrap: wrap;
}

.rank-tag {
  margin: 0;
}
</style>
