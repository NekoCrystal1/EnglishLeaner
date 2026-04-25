<template>
  <div class="placeholder-page">
    <section class="placeholder-panel">
      <el-icon><component :is="page.icon" /></el-icon>
      <div>
        <h1 class="page-title">{{ page.title }}</h1>
        <p class="page-subtitle">{{ page.subtitle }}</p>
      </div>
      <el-button type="primary" plain @click="router.push('/')">返回首页</el-button>
    </section>
  </div>
</template>

<script setup>
import { computed } from "vue";
import {
  ChatDotRound,
  Headset,
  Medal,
  Notebook,
  Reading,
  Star,
  Tickets,
  Trophy,
  UserFilled
} from "@element-plus/icons-vue";
import { useRoute, useRouter } from "vue-router";

const route = useRoute();
const router = useRouter();

const pageMap = {
  reading: { title: "阅读", subtitle: "文章列表、阅读进度和阅读理解题。", icon: Reading },
  listening: { title: "听力", subtitle: "听力材料、播放器、文本稿和听力题。", icon: Headset },
  speaking: { title: "口语", subtitle: "口语任务、录音、回放和评测结果。", icon: ChatDotRound },
  "wrong-answers": { title: "错题集", subtitle: "按单词、阅读、听力和口语复盘薄弱点。", icon: Tickets },
  favorites: { title: "收藏夹", subtitle: "收藏单词、题目、阅读文章和听力材料。", icon: Star },
  checkin: { title: "打卡", subtitle: "记录连续学习、每日任务和积分奖励。", icon: Medal },
  achievements: { title: "成就", subtitle: "展示阶段目标、徽章和学习奖励。", icon: Trophy },
  groups: { title: "学习小组", subtitle: "同桌、好友、小组榜和共同学习。", icon: UserFilled },
  ai: { title: "AI 学习", subtitle: "例句生成、对话练习和能力测评。", icon: Notebook }
};

const page = computed(() => pageMap[route.name] || pageMap.reading);
</script>

<style scoped>
.placeholder-page {
  min-height: 420px;
  display: grid;
  place-items: center;
}

.placeholder-panel {
  width: min(680px, 100%);
  display: grid;
  justify-items: center;
  gap: 16px;
  padding: 36px;
  border: 1px solid #e5e7eb;
  border-radius: 8px;
  background: #ffffff;
  text-align: center;
}

.placeholder-panel > .el-icon {
  color: #2563eb;
  font-size: 42px;
}
</style>
