<template>
  <div>
    <h1 class="page-title">单词测验</h1>
    <p class="page-subtitle">选择单词对应的释义，答题后会更新积分。</p>

    <el-card v-loading="loadingQuestion">
      <template #header>
        <div class="header-row">
          <span>当前题目</span>
          <el-button type="primary" plain @click="loadNextQuestion">换一题</el-button>
        </div>
      </template>

      <el-empty v-if="!question" description="暂无题目，请先确认后端已导入词库。" />

      <template v-else>
        <div class="question-word">{{ question.word }}</div>
        <el-radio-group v-model="selectedAnswer" :disabled="Boolean(result)" class="options">
          <el-radio
            v-for="option in question.options"
            :key="option"
            :value="option"
            border
            class="option-item"
          >
            {{ option }}
          </el-radio>
        </el-radio-group>

        <div class="action-row">
          <el-button type="success" :loading="submitting" :disabled="Boolean(result)" @click="submitAnswer">
            提交答案
          </el-button>
          <el-button v-if="result" type="primary" @click="prepareNext">
            下一题
          </el-button>
        </div>

        <el-alert
          v-if="result"
          :title="result.correct ? '回答正确' : '回答错误'"
          :type="result.correct ? 'success' : 'error'"
          :closable="false"
          show-icon
        >
          <template #default>
            <div class="feedback-line">正确答案：{{ result.correctAnswer }}</div>
            <div class="feedback-line">积分变化：{{ result.scoreDelta }}</div>
            <div class="feedback-line">当前总分：{{ result.totalScore }}</div>
          </template>
        </el-alert>
      </template>
    </el-card>
  </div>
</template>

<script setup>
import { onMounted, ref } from "vue";
import { ElMessage } from "element-plus";
import { quizApi } from "../api/modules";
import { useAuthStore } from "../stores/auth";

const authStore = useAuthStore();

const question = ref(null);
const selectedAnswer = ref("");
const result = ref(null);
const loadingQuestion = ref(false);
const submitting = ref(false);

async function loadNextQuestion() {
  loadingQuestion.value = true;
  try {
    const data = await quizApi.next();
    question.value = data;
    selectedAnswer.value = "";
    result.value = null;
  } catch (error) {
    ElMessage.error(error.message || "加载题目失败");
  } finally {
    loadingQuestion.value = false;
  }
}

async function submitAnswer() {
  if (!question.value) {
    return;
  }
  if (!selectedAnswer.value) {
    ElMessage.warning("请先选择一个答案");
    return;
  }

  submitting.value = true;
  try {
    const data = await quizApi.submit({
      wordId: question.value.wordId,
      selectedAnswer: selectedAnswer.value
    });
    result.value = data;
    authStore.updateTotalScore(data.totalScore);
  } catch (error) {
    ElMessage.error(error.message || "提交答案失败");
  } finally {
    submitting.value = false;
  }
}

function prepareNext() {
  loadNextQuestion();
}

onMounted(loadNextQuestion);
</script>

<style scoped>
.header-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
}

.question-word {
  font-size: 34px;
  font-weight: 700;
  letter-spacing: 0.03em;
  margin-bottom: 18px;
  color: #0f172a;
}

.options {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 12px;
}

.option-item {
  margin: 0;
}

.action-row {
  margin: 20px 0;
  display: flex;
  gap: 10px;
}

.feedback-line {
  margin-top: 4px;
}

@media (max-width: 900px) {
  .options {
    grid-template-columns: 1fr;
  }

  .question-word {
    font-size: 28px;
  }
}
</style>
