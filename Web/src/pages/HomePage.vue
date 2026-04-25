<template>
  <div>
    <h1 class="page-title">学习概览</h1>
    <p class="page-subtitle">查看今天的学习情况，并快速进入常用功能。</p>

    <div class="stats-grid">
      <el-card v-for="item in stats" :key="item.label" shadow="never">
        <div class="stat-label">{{ item.label }}</div>
        <div class="stat-value">{{ item.value }}</div>
      </el-card>
    </div>

    <el-card class="quick-card" shadow="never">
      <template #header>
        <span>快速开始</span>
      </template>
      <div class="quick-actions">
        <el-button type="primary" @click="router.push('/words')">开始背词</el-button>
        <el-button type="success" @click="router.push('/quiz')">进入测验</el-button>
        <el-button plain @click="router.push('/ranking')">查看排行榜</el-button>
      </div>
    </el-card>
  </div>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";
import { ElMessage } from "element-plus";
import { useRouter } from "vue-router";
import { wordLearningApi } from "../api/modules";

const router = useRouter();
const summary = ref(null);

const stats = computed(() => [
  { label: "新学单词", value: summary.value?.newWords ?? 0 },
  { label: "复习单词", value: summary.value?.reviewWords ?? 0 },
  { label: "答对次数", value: summary.value?.correctCount ?? 0 },
  { label: "今日积分", value: summary.value?.scoreDelta ?? 0 }
]);

onMounted(async () => {
  try {
    summary.value = await wordLearningApi.todaySummary();
  } catch (error) {
    ElMessage.error(error.message || "加载学习概览失败");
  }
});
</script>

<style scoped>
.stats-grid {
  display: grid;
  grid-template-columns: repeat(4, minmax(0, 1fr));
  gap: 14px;
  margin-bottom: 18px;
}

.stat-label {
  color: #6b7280;
  margin-bottom: 8px;
}

.stat-value {
  font-size: 30px;
  font-weight: 800;
  color: #111827;
}

.quick-card {
  margin-top: 18px;
}

.quick-actions {
  display: flex;
  gap: 12px;
  flex-wrap: wrap;
}

@media (max-width: 900px) {
  .stats-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }
}
</style>
