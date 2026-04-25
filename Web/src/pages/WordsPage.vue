<template>
  <div class="words-page">
    <section class="page-head">
      <div>
        <h1 class="page-title">单词学习</h1>
        <p class="page-subtitle">
          今日 {{ summary?.newWords ?? 0 }} 新词 · {{ summary?.reviewWords ?? 0 }} 复习 ·
          正确率 {{ todayAccuracy }}%
        </p>
      </div>
      <el-button :icon="RefreshRight" :loading="refreshing" @click="loadData">刷新</el-button>
    </section>

    <section class="overview-grid">
      <div v-for="item in overview" :key="item.label" class="metric">
        <span>{{ item.label }}</span>
        <strong>{{ item.value }}</strong>
      </div>
    </section>

    <section class="learn-workspace">
      <el-card class="study-panel" shadow="never" v-loading="loadingWords">
        <template #header>
          <div class="card-header">
            <span>当前学习</span>
            <el-tag v-if="currentWord" :type="modeTagType(currentWord.studyMode)" effect="light">
              {{ modeLabel(currentWord.studyMode) }}
            </el-tag>
          </div>
        </template>

        <div v-if="currentWord" class="study-card">
          <div class="card-status">
            <span>{{ currentIndex + 1 }} / {{ dueWords.length }}</span>
            <span>{{ stateLabel(currentWord.state) }} · 掌握度 {{ currentWord.masteryLevel ?? 0 }}/5</span>
          </div>

          <div class="word-face">
            <h2>{{ currentWord.word }}</h2>
            <div class="phonetics">
              <span v-if="currentWord.phoneticUs">美 {{ currentWord.phoneticUs }}</span>
              <span v-if="currentWord.phoneticUk">英 {{ currentWord.phoneticUk }}</span>
            </div>
          </div>

          <div class="answer-panel" :class="{ concealed: !showAnswer }">
            <div class="translation">{{ currentWord.translation }}</div>
            <div class="word-meta">
              <el-tag v-if="currentWord.partOfSpeech" size="small">{{ currentWord.partOfSpeech }}</el-tag>
              <el-tag size="small" type="info">难度 {{ currentWord.difficulty ?? 1 }}</el-tag>
              <span v-if="currentWord.nextReviewAt">下次 {{ formatDateTime(currentWord.nextReviewAt) }}</span>
            </div>
            <p v-if="currentWord.exampleSentence" class="example">
              {{ currentWord.exampleSentence }}
            </p>
          </div>

          <div class="study-actions">
            <el-button
              v-if="!showAnswer"
              type="primary"
              size="large"
              :icon="View"
              @click="revealAnswer"
            >
              显示释义
            </el-button>
            <template v-else>
              <el-button type="danger" plain :icon="Close" :loading="submitting" @click="submitQuality(1)">
                忘记
              </el-button>
              <el-button type="warning" plain :loading="submitting" @click="submitQuality(2)">
                模糊
              </el-button>
              <el-button type="success" plain :icon="Check" :loading="submitting" @click="submitQuality(4)">
                认识
              </el-button>
              <el-button type="primary" :icon="ArrowRight" :loading="submitting" @click="submitQuality(5)">
                太简单
              </el-button>
            </template>
          </div>
        </div>

        <div v-else class="empty-study">
          <el-empty description="今天的单词已清空" />
          <el-button type="primary" plain :icon="RefreshRight" @click="loadDueWords">重新检查</el-button>
        </div>
      </el-card>

      <div class="side-stack">
        <el-card v-loading="loadingPlans" shadow="never">
          <template #header>
            <div class="card-header">
              <span>当前计划</span>
              <el-tag v-if="currentPlan" :type="planStatusType(currentPlan.status)" effect="light">
                {{ planStatusLabel(currentPlan.status) }}
              </el-tag>
            </div>
          </template>

          <template v-if="currentPlan">
            <div class="plan-title">{{ currentPlan.bookTitle }}</div>
            <el-progress :percentage="planProgress" :stroke-width="10" />
            <div class="plan-meta">
              <span>已学 {{ currentPlan.learnedWords ?? 0 }}/{{ currentPlan.totalWords ?? 0 }}</span>
              <span>掌握 {{ currentPlan.masteredWords ?? 0 }}</span>
              <span>到期 {{ currentPlan.dueReviewWords ?? 0 }}</span>
            </div>

            <el-form class="plan-form" label-position="top">
              <div class="quota-row">
                <el-form-item label="每日新词">
                  <el-input-number v-model="planDailyNewCount" :min="1" :max="200" />
                </el-form-item>
                <el-form-item label="每日复习">
                  <el-input-number v-model="planDailyReviewCount" :min="1" :max="500" />
                </el-form-item>
              </div>
              <el-button type="primary" plain :icon="Setting" :loading="savingPlan" @click="savePlan">
                保存计划
              </el-button>
            </el-form>
          </template>

          <el-empty v-else description="暂无学习计划" />
        </el-card>

        <el-card v-loading="loadingBooks" shadow="never">
          <template #header>
            <span>创建计划</span>
          </template>
          <el-form label-position="top">
            <el-form-item label="词书">
              <el-select v-model="selectedBookId" placeholder="请选择词书" class="full-width">
                <el-option
                  v-for="book in books"
                  :key="book.bookId"
                  :label="`${book.title}（${book.itemCount ?? 0}词）`"
                  :value="book.bookId"
                />
              </el-select>
            </el-form-item>
            <div class="quota-row">
              <el-form-item label="新词">
                <el-input-number v-model="dailyNewCount" :min="1" :max="200" />
              </el-form-item>
              <el-form-item label="复习">
                <el-input-number v-model="dailyReviewCount" :min="1" :max="500" />
              </el-form-item>
            </div>
            <el-button type="primary" :loading="creating" @click="createPlan">创建并启用</el-button>
          </el-form>
        </el-card>
      </div>
    </section>
  </div>
</template>

<script setup>
import { computed, onMounted, ref, watch } from "vue";
import { ElMessage } from "element-plus";
import { ArrowRight, Check, Close, RefreshRight, Setting, View } from "@element-plus/icons-vue";
import { bookApi, wordLearningApi } from "../api/modules";
import { useAuthStore } from "../stores/auth";

const authStore = useAuthStore();
const books = ref([]);
const plans = ref([]);
const dueWords = ref([]);
const summary = ref(null);
const selectedBookId = ref(null);
const dailyNewCount = ref(20);
const dailyReviewCount = ref(50);
const planDailyNewCount = ref(20);
const planDailyReviewCount = ref(50);
const currentIndex = ref(0);
const showAnswer = ref(false);
const cardStartedAt = ref(Date.now());
const completedInSession = ref(0);
const correctInSession = ref(0);

const loadingBooks = ref(false);
const loadingPlans = ref(false);
const loadingWords = ref(false);
const refreshing = ref(false);
const creating = ref(false);
const savingPlan = ref(false);
const submitting = ref(false);

const currentPlan = computed(() => plans.value.find((plan) => plan.status === "ACTIVE") || plans.value[0] || null);
const currentWord = computed(() => dueWords.value[currentIndex.value] || null);
const planProgress = computed(() => percent(currentPlan.value?.learnedWords, currentPlan.value?.totalWords));
const totalAnsweredToday = computed(() => (summary.value?.correctCount ?? 0) + (summary.value?.wrongCount ?? 0));
const todayAccuracy = computed(() => percent(summary.value?.correctCount, totalAnsweredToday.value));
const sessionAccuracy = computed(() => percent(correctInSession.value, completedInSession.value));

const overview = computed(() => [
  { label: "待复习", value: currentPlan.value?.dueReviewWords ?? 0 },
  { label: "队列", value: dueWords.value.length },
  { label: "本次完成", value: completedInSession.value },
  { label: "本次正确率", value: `${sessionAccuracy.value}%` }
]);

watch(
  currentPlan,
  (plan) => {
    if (!plan) {
      return;
    }
    planDailyNewCount.value = plan.dailyNewCount ?? 20;
    planDailyReviewCount.value = plan.dailyReviewCount ?? 50;
  },
  { immediate: true }
);

async function loadBooks() {
  loadingBooks.value = true;
  try {
    books.value = await bookApi.list("WORD");
    if (!selectedBookId.value && books.value.length > 0) {
      selectedBookId.value = books.value[0].bookId;
    }
  } catch (error) {
    ElMessage.error(error.message || "加载词书失败");
  } finally {
    loadingBooks.value = false;
  }
}

async function loadPlans() {
  loadingPlans.value = true;
  try {
    plans.value = await wordLearningApi.listPlans();
    if (plans.value.length === 0) {
      const plan = await wordLearningApi.currentPlan();
      plans.value = [plan];
    }
  } catch (error) {
    ElMessage.error(error.message || "加载学习计划失败");
  } finally {
    loadingPlans.value = false;
  }
}

async function loadSummary() {
  try {
    summary.value = await wordLearningApi.todaySummary();
  } catch (error) {
    ElMessage.error(error.message || "加载学习概览失败");
  }
}

async function loadDueWords() {
  loadingWords.value = true;
  try {
    dueWords.value = await wordLearningApi.dueWords(currentPlan.value?.planId);
    currentIndex.value = 0;
    showAnswer.value = false;
    cardStartedAt.value = Date.now();
  } catch (error) {
    ElMessage.error(error.message || "加载待学习单词失败");
  } finally {
    loadingWords.value = false;
  }
}

async function createPlan() {
  if (!selectedBookId.value) {
    ElMessage.warning("请先选择词书");
    return;
  }

  creating.value = true;
  try {
    await wordLearningApi.createPlan({
      bookId: selectedBookId.value,
      dailyNewCount: dailyNewCount.value,
      dailyReviewCount: dailyReviewCount.value
    });
    ElMessage.success("学习计划已创建");
    await loadPlans();
    await loadDueWords();
  } catch (error) {
    ElMessage.error(error.message || "创建学习计划失败");
  } finally {
    creating.value = false;
  }
}

async function savePlan() {
  if (!currentPlan.value) {
    return;
  }
  savingPlan.value = true;
  try {
    await wordLearningApi.updatePlan(currentPlan.value.planId, {
      dailyNewCount: planDailyNewCount.value,
      dailyReviewCount: planDailyReviewCount.value
    });
    ElMessage.success("计划已更新");
    await loadPlans();
    await loadDueWords();
  } catch (error) {
    ElMessage.error(error.message || "保存计划失败");
  } finally {
    savingPlan.value = false;
  }
}

function revealAnswer() {
  showAnswer.value = true;
}

async function submitQuality(quality) {
  const word = currentWord.value;
  const plan = currentPlan.value;
  if (!word || !plan) {
    return;
  }

  submitting.value = true;
  try {
    const durationMs = Math.max(500, Date.now() - cardStartedAt.value);
    const correct = quality >= 3;
    const response = await wordLearningApi.submitEvent({
      planId: plan.planId,
      wordId: word.wordId,
      activityType: word.studyMode === "NEW" ? "NEW" : "REVIEW",
      selectedAnswer: word.translation,
      correct,
      quality,
      durationMs,
      clientEventId: `${Date.now()}-${word.wordId}-${quality}`
    });

    completedInSession.value += 1;
    if (correct) {
      correctInSession.value += 1;
    }
    authStore.updateTotalScore(response?.totalScore);

    dueWords.value.splice(currentIndex.value, 1);
    if (currentIndex.value >= dueWords.value.length) {
      currentIndex.value = 0;
    }
    showAnswer.value = false;
    cardStartedAt.value = Date.now();
    ElMessage.success(nextReviewMessage(response));
    await Promise.all([loadPlans(), loadSummary()]);
  } catch (error) {
    ElMessage.error(error.message || "记录学习结果失败");
  } finally {
    submitting.value = false;
  }
}

async function loadData() {
  refreshing.value = true;
  try {
    await Promise.all([loadBooks(), loadPlans(), loadSummary()]);
    await loadDueWords();
  } finally {
    refreshing.value = false;
  }
}

function nextReviewMessage(response) {
  if (response?.nextReviewAt) {
    return `已记录，下次 ${formatDateTime(response.nextReviewAt)}`;
  }
  return "已记录";
}

function percent(value, total) {
  const safeTotal = Number(total ?? 0);
  if (!safeTotal) {
    return 0;
  }
  return Math.round((Number(value ?? 0) / safeTotal) * 100);
}

function modeLabel(mode) {
  return mode === "NEW" ? "新词" : "复习";
}

function modeTagType(mode) {
  return mode === "NEW" ? "success" : "warning";
}

function stateLabel(state) {
  const labels = {
    NEW: "新词",
    LEARNING: "学习中",
    REVIEWING: "复习中",
    MASTERED: "已掌握"
  };
  return labels[state] || state || "新词";
}

function planStatusLabel(status) {
  const labels = {
    ACTIVE: "进行中",
    PAUSED: "已暂停",
    FINISHED: "已完成"
  };
  return labels[status] || status || "未知";
}

function planStatusType(status) {
  if (status === "ACTIVE") {
    return "success";
  }
  if (status === "FINISHED") {
    return "primary";
  }
  return "info";
}

function formatDateTime(value) {
  if (!value) {
    return "";
  }
  const date = new Date(value);
  if (Number.isNaN(date.getTime())) {
    return value;
  }
  const today = new Date();
  const sameDay = date.toDateString() === today.toDateString();
  return sameDay
    ? date.toLocaleTimeString("zh-CN", { hour: "2-digit", minute: "2-digit" })
    : date.toLocaleDateString("zh-CN", { month: "2-digit", day: "2-digit" });
}

onMounted(loadData);
</script>

<style scoped>
.words-page {
  display: flex;
  flex-direction: column;
  gap: 18px;
}

.page-head {
  display: flex;
  align-items: flex-start;
  justify-content: space-between;
  gap: 16px;
}

.overview-grid {
  display: grid;
  grid-template-columns: repeat(4, minmax(0, 1fr));
  gap: 12px;
}

.metric {
  display: flex;
  flex-direction: column;
  gap: 6px;
  min-height: 82px;
  padding: 16px;
  border: 1px solid #dbe3ef;
  border-radius: 8px;
  background: #ffffff;
}

.metric span {
  color: #64748b;
  font-size: 13px;
}

.metric strong {
  color: #0f172a;
  font-size: 28px;
  line-height: 1;
}

.learn-workspace {
  display: grid;
  grid-template-columns: minmax(0, 1.6fr) minmax(320px, 0.9fr);
  gap: 16px;
  align-items: start;
}

.study-panel {
  min-height: 540px;
}

.card-header,
.card-status,
.plan-meta {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
}

.study-card {
  min-height: 440px;
  display: flex;
  flex-direction: column;
  gap: 24px;
}

.card-status {
  color: #64748b;
  font-size: 13px;
}

.word-face {
  padding: 22px 0 6px;
  text-align: center;
}

.word-face h2 {
  margin: 0;
  color: #0f172a;
  font-size: 52px;
  line-height: 1.1;
  font-weight: 800;
  letter-spacing: 0;
  overflow-wrap: anywhere;
}

.phonetics {
  display: flex;
  justify-content: center;
  gap: 12px;
  min-height: 24px;
  margin-top: 12px;
  color: #475569;
}

.answer-panel {
  min-height: 150px;
  padding: 20px;
  border: 1px solid #dbe3ef;
  border-radius: 8px;
  background: #f8fafc;
  transition: opacity 0.16s ease, transform 0.16s ease;
}

.answer-panel.concealed {
  opacity: 0;
  transform: translateY(4px);
  pointer-events: none;
}

.translation {
  color: #111827;
  font-size: 26px;
  font-weight: 700;
  overflow-wrap: anywhere;
}

.word-meta {
  display: flex;
  align-items: center;
  flex-wrap: wrap;
  gap: 8px;
  margin-top: 12px;
  color: #64748b;
  font-size: 13px;
}

.example {
  margin: 16px 0 0;
  color: #334155;
  line-height: 1.7;
}

.study-actions {
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  gap: 10px;
  margin-top: auto;
}

.empty-study {
  min-height: 430px;
  display: grid;
  place-items: center;
}

.side-stack {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.plan-title {
  margin-bottom: 14px;
  color: #0f172a;
  font-size: 18px;
  font-weight: 700;
}

.plan-meta {
  margin-top: 12px;
  color: #64748b;
  font-size: 13px;
  flex-wrap: wrap;
}

.plan-form {
  margin-top: 18px;
}

.quota-row {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 12px;
}

.full-width {
  width: 100%;
}

@media (max-width: 1080px) {
  .learn-workspace {
    grid-template-columns: 1fr;
  }
}

@media (max-width: 720px) {
  .page-head {
    flex-direction: column;
  }

  .overview-grid,
  .quota-row {
    grid-template-columns: 1fr 1fr;
  }

  .word-face h2 {
    font-size: 40px;
  }

  .translation {
    font-size: 22px;
  }
}

@media (max-width: 520px) {
  .overview-grid,
  .quota-row {
    grid-template-columns: 1fr;
  }

  .study-actions :deep(.el-button) {
    width: 100%;
    margin-left: 0;
  }
}
</style>
