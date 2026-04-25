<template>
  <div>
    <h1 class="page-title">单词学习</h1>
    <p class="page-subtitle">选择词书创建计划，或继续当前计划的待复习单词。</p>

    <div class="grid-2">
      <el-card v-loading="loadingPlans" shadow="never">
        <template #header>
          <div class="card-header">
            <span>当前计划</span>
            <el-button text type="primary" @click="loadData">刷新</el-button>
          </div>
        </template>

        <el-descriptions v-if="currentPlan" :column="1" border>
          <el-descriptions-item label="词书">{{ currentPlan.bookTitle }}</el-descriptions-item>
          <el-descriptions-item label="进度">
            {{ currentPlan.learnedWords ?? 0 }} / {{ currentPlan.totalWords ?? 0 }}
          </el-descriptions-item>
          <el-descriptions-item label="待复习">{{ currentPlan.dueReviewWords ?? 0 }}</el-descriptions-item>
          <el-descriptions-item label="状态">{{ currentPlan.status }}</el-descriptions-item>
        </el-descriptions>
        <el-empty v-else description="暂无学习计划" />
      </el-card>

      <el-card v-loading="loadingBooks" shadow="never">
        <template #header>
          <span>创建学习计划</span>
        </template>
        <el-form label-position="top">
          <el-form-item label="词书">
            <el-select v-model="selectedBookId" placeholder="请选择词书" style="width: 100%;">
              <el-option
                v-for="book in books"
                :key="book.bookId"
                :label="`${book.title}（${book.itemCount ?? 0}词）`"
                :value="book.bookId"
              />
            </el-select>
          </el-form-item>
          <el-form-item label="每日新词">
            <el-input-number v-model="dailyNewCount" :min="1" :max="200" />
          </el-form-item>
          <el-form-item label="每日复习">
            <el-input-number v-model="dailyReviewCount" :min="1" :max="500" />
          </el-form-item>
          <el-button type="primary" :loading="creating" @click="createPlan">创建计划</el-button>
        </el-form>
      </el-card>
    </div>

    <el-card class="words-card" v-loading="loadingWords" shadow="never">
      <template #header>
        <div class="card-header">
          <span>待学习单词</span>
          <el-button type="primary" plain @click="loadDueWords">重新加载</el-button>
        </div>
      </template>

      <el-empty v-if="dueWords.length === 0" description="当前没有待学习单词" />
      <el-table v-else :data="dueWords" border stripe>
        <el-table-column prop="word" label="单词" min-width="140" />
        <el-table-column prop="translation" label="释义" min-width="180" />
        <el-table-column prop="partOfSpeech" label="词性" width="100" />
        <el-table-column prop="masteryLevel" label="掌握度" width="100" />
        <el-table-column label="操作" width="160">
          <template #default="{ row }">
            <el-button size="small" type="success" @click="markWord(row, true)">认识</el-button>
            <el-button size="small" @click="markWord(row, false)">不熟</el-button>
          </template>
        </el-table-column>
      </el-table>
    </el-card>
  </div>
</template>

<script setup>
import { computed, onMounted, ref } from "vue";
import { ElMessage } from "element-plus";
import { bookApi, wordLearningApi } from "../api/modules";

const books = ref([]);
const plans = ref([]);
const dueWords = ref([]);
const selectedBookId = ref(null);
const dailyNewCount = ref(20);
const dailyReviewCount = ref(50);
const loadingBooks = ref(false);
const loadingPlans = ref(false);
const loadingWords = ref(false);
const creating = ref(false);

const currentPlan = computed(() => plans.value.find((plan) => plan.status === "ACTIVE") || plans.value[0] || null);

async function loadBooks() {
  loadingBooks.value = true;
  try {
    books.value = await bookApi.list();
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
  } catch (error) {
    ElMessage.error(error.message || "加载学习计划失败");
  } finally {
    loadingPlans.value = false;
  }
}

async function loadDueWords() {
  loadingWords.value = true;
  try {
    dueWords.value = await wordLearningApi.dueWords(currentPlan.value?.planId);
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

async function markWord(word, correct) {
  try {
    await wordLearningApi.submitEvent({
      planId: currentPlan.value?.planId,
      wordId: word.wordId,
      activityType: "WORD_REVIEW",
      selectedAnswer: word.translation,
      correct,
      durationMs: 1000,
      clientEventId: `${Date.now()}-${word.wordId}`
    });
    ElMessage.success(correct ? "已记录：认识" : "已记录：继续复习");
    dueWords.value = dueWords.value.filter((item) => item.wordId !== word.wordId);
  } catch (error) {
    ElMessage.error(error.message || "记录学习结果失败");
  }
}

async function loadData() {
  await Promise.all([loadBooks(), loadPlans()]);
  await loadDueWords();
}

onMounted(loadData);
</script>

<style scoped>
.card-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
}

.words-card {
  margin-top: 18px;
}
</style>
