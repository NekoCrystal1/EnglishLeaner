<template>
  <div class="home-page">
    <section class="inspiration-hero" :style="{ backgroundImage: heroBackground }">
      <div class="hero-content">
        <span class="hero-kicker">{{ todayPrompt.word }}</span>
        <h1>{{ todayPrompt.quote }}</h1>
        <p>{{ todayPrompt.translation }}</p>
      </div>
    </section>

    <section class="stats-grid">
      <div v-for="item in stats" :key="item.label" class="stat-card">
        <div class="stat-label">{{ item.label }}</div>
        <div class="stat-value">{{ item.value }}</div>
      </div>
    </section>

    <section class="quick-panel">
      <div class="section-head">
        <div>
          <h2>快速开始</h2>
          <p>选择一个入口继续今天的练习。</p>
        </div>
      </div>
      <div class="quick-actions">
        <el-button type="primary" :icon="Collection" @click="router.push('/words')">背单词</el-button>
        <el-button type="success" :icon="EditPen" @click="router.push('/quiz')">单词测验</el-button>
        <el-button plain :icon="Trophy" @click="router.push('/ranking')">排行榜</el-button>
      </div>
    </section>

    <section class="feature-panel">
      <div class="section-head">
        <div>
          <h2>学习模块</h2>
          <p>按听、说、读、练和协作路径扩展。</p>
        </div>
      </div>
      <div class="feature-grid">
        <button
          v-for="item in learningEntries"
          :key="item.path"
          class="feature-tile"
          type="button"
          @click="router.push(item.path)"
        >
          <el-icon><component :is="item.icon" /></el-icon>
          <span>{{ item.title }}</span>
          <small>{{ item.subtitle }}</small>
        </button>
      </div>
    </section>

    <section class="scenery-grid">
      <div
        v-for="scene in scenes"
        :key="scene.title"
        class="scene-card"
        :style="{ backgroundImage: `url(${scene.image})` }"
      >
        <span>{{ scene.title }}</span>
      </div>
    </section>
  </div>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";
import { ElMessage } from "element-plus";
import {
  ChatDotRound,
  Collection,
  EditPen,
  Headset,
  Medal,
  Notebook,
  Reading,
  Star,
  Tickets,
  Trophy,
  UserFilled
} from "@element-plus/icons-vue";
import { useRouter } from "vue-router";
import { wordLearningApi } from "../api/modules";

const router = useRouter();
const summary = ref(null);

const prompts = [
  {
    word: "Perseverance",
    quote: "Small steps every day become fluent miles.",
    translation: "每天一小步，终会走成流利表达的长路。"
  },
  {
    word: "Courage",
    quote: "Speak before you are ready; growth answers after.",
    translation: "先开口，再变好。成长总会在行动之后回应。"
  },
  {
    word: "Focus",
    quote: "One word mastered is one door opened.",
    translation: "掌握一个词，就是推开一扇新的门。"
  }
];

const scenes = [
  {
    title: "Mountain Morning",
    image: "https://images.unsplash.com/photo-1500530855697-b586d89ba3ee?auto=format&fit=crop&w=900&q=80"
  },
  {
    title: "Quiet Lake",
    image: "https://images.unsplash.com/photo-1501785888041-af3ef285b470?auto=format&fit=crop&w=900&q=80"
  },
  {
    title: "Open Road",
    image: "https://images.unsplash.com/photo-1470770841072-f978cf4d019e?auto=format&fit=crop&w=900&q=80"
  }
];

const learningEntries = [
  { title: "阅读", subtitle: "文章与阅读理解", path: "/reading", icon: Reading },
  { title: "听力", subtitle: "材料、文本稿与听力题", path: "/listening", icon: Headset },
  { title: "口语", subtitle: "任务、录音与评测", path: "/speaking", icon: ChatDotRound },
  { title: "错题集", subtitle: "按模块复盘薄弱点", path: "/wrong-answers", icon: Tickets },
  { title: "收藏夹", subtitle: "单词、题目和文章", path: "/favorites", icon: Star },
  { title: "打卡", subtitle: "连续学习与积分激励", path: "/checkin", icon: Medal },
  { title: "成就", subtitle: "阶段目标与奖励", path: "/achievements", icon: Trophy },
  { title: "学习小组", subtitle: "同桌、好友和小组榜", path: "/groups", icon: UserFilled },
  { title: "AI 学习", subtitle: "例句、对话和测评", path: "/ai", icon: Notebook }
];

const todayPrompt = computed(() => prompts[new Date().getDate() % prompts.length]);
const heroBackground = computed(() => `linear-gradient(rgba(15, 23, 42, 0.28), rgba(15, 23, 42, 0.48)), url(${scenes[0].image})`);

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
.home-page {
  display: grid;
  gap: 18px;
}

.inspiration-hero {
  min-height: 280px;
  display: flex;
  align-items: flex-end;
  padding: 28px;
  border-radius: 8px;
  background-position: center;
  background-size: cover;
  color: #ffffff;
  overflow: hidden;
}

.hero-content {
  max-width: 760px;
}

.hero-kicker {
  display: inline-block;
  margin-bottom: 10px;
  font-weight: 800;
  letter-spacing: 0;
}

.hero-content h1 {
  margin: 0 0 10px;
  font-size: 36px;
  line-height: 1.18;
  letter-spacing: 0;
}

.hero-content p {
  margin: 0;
  color: rgba(255, 255, 255, 0.9);
  font-size: 16px;
}

.stats-grid {
  display: grid;
  grid-template-columns: repeat(4, minmax(0, 1fr));
  gap: 14px;
}

.stat-card,
.quick-panel,
.feature-panel {
  padding: 18px;
  border: 1px solid #e5e7eb;
  border-radius: 8px;
  background: #ffffff;
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

.section-head {
  display: flex;
  justify-content: space-between;
  gap: 12px;
  margin-bottom: 14px;
}

.section-head h2 {
  margin: 0;
  color: #111827;
  font-size: 18px;
}

.section-head p {
  margin: 4px 0 0;
  color: #64748b;
}

.quick-actions {
  display: flex;
  gap: 12px;
  flex-wrap: wrap;
}

.feature-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 12px;
}

.feature-tile {
  min-height: 110px;
  display: grid;
  justify-items: start;
  align-content: center;
  gap: 8px;
  padding: 16px;
  border: 1px solid #dbe3ef;
  border-radius: 8px;
  background: #f8fafc;
  color: #111827;
  cursor: pointer;
  text-align: left;
}

.feature-tile:hover {
  border-color: #409eff;
  background: #eef6ff;
}

.feature-tile .el-icon {
  color: #2563eb;
  font-size: 22px;
}

.feature-tile span {
  font-weight: 800;
}

.feature-tile small {
  color: #64748b;
  line-height: 1.4;
}

.scenery-grid {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 12px;
}

.scene-card {
  min-height: 150px;
  display: flex;
  align-items: flex-end;
  padding: 14px;
  border-radius: 8px;
  background-position: center;
  background-size: cover;
  color: #ffffff;
  font-weight: 800;
  overflow: hidden;
}

@media (max-width: 900px) {
  .stats-grid,
  .feature-grid,
  .scenery-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }

  .hero-content h1 {
    font-size: 30px;
  }
}

@media (max-width: 560px) {
  .stats-grid,
  .feature-grid,
  .scenery-grid {
    grid-template-columns: 1fr;
  }

  .inspiration-hero {
    min-height: 240px;
    padding: 22px;
  }
}
</style>
