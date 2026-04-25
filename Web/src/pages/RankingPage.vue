<template>
  <div>
    <h1 class="page-title">积分排行榜</h1>
    <p class="page-subtitle">查看当前学习积分排名，支持 Top 10 / 20 / 50。</p>

    <el-card>
      <template #header>
        <div class="toolbar">
          <el-space>
            <span>显示范围</span>
            <el-select v-model="limit" style="width: 140px;" @change="loadRanking">
              <el-option :value="10" label="Top 10" />
              <el-option :value="20" label="Top 20" />
              <el-option :value="50" label="Top 50" />
            </el-select>
          </el-space>
          <el-button type="primary" plain :loading="loading" @click="loadRanking">刷新</el-button>
        </div>
      </template>

      <el-table :data="rankings" border stripe v-loading="loading">
        <el-table-column prop="rank" label="排名" width="90" />
        <el-table-column prop="username" label="用户名" />
        <el-table-column prop="totalScore" label="总分" width="120" />
      </el-table>

      <div class="chart-wrap">
        <div ref="chartRef" class="chart" />
      </div>
    </el-card>
  </div>
</template>

<script setup>
import { nextTick, onBeforeUnmount, onMounted, ref, watch } from "vue";
import { ElMessage } from "element-plus";
import { init, use } from "echarts/core";
import { BarChart } from "echarts/charts";
import { GridComponent, TooltipComponent } from "echarts/components";
import { CanvasRenderer } from "echarts/renderers";
import { rankingApi } from "../api/modules";

use([BarChart, GridComponent, TooltipComponent, CanvasRenderer]);

const limit = ref(10);
const rankings = ref([]);
const loading = ref(false);
const chartRef = ref(null);
let chart = null;

async function loadRanking() {
  loading.value = true;
  try {
    rankings.value = await rankingApi.top(limit.value);
  } catch (error) {
    ElMessage.error(error.message || "加载排行榜失败");
  } finally {
    loading.value = false;
  }
}

function initChart() {
  if (!chartRef.value) {
    return;
  }
  chart = init(chartRef.value);
  renderChart();
}

function renderChart() {
  if (!chart) {
    return;
  }

  const usernames = rankings.value.map((item) => item.username);
  const scores = rankings.value.map((item) => item.totalScore);

  chart.setOption({
    tooltip: {
      trigger: "axis"
    },
    grid: {
      left: 20,
      right: 20,
      top: 25,
      bottom: 50,
      containLabel: true
    },
    xAxis: {
      type: "category",
      data: usernames,
      axisLabel: {
        interval: 0,
        rotate: usernames.length > 8 ? 30 : 0
      }
    },
    yAxis: {
      type: "value",
      name: "积分"
    },
    series: [
      {
        type: "bar",
        data: scores,
        barMaxWidth: 36,
        itemStyle: {
          borderRadius: [6, 6, 0, 0],
          color: "#2563eb"
        }
      }
    ]
  });
}

function resizeChart() {
  chart?.resize();
}

watch(rankings, renderChart, { deep: true });

onMounted(async () => {
  await loadRanking();
  await nextTick();
  initChart();
  window.addEventListener("resize", resizeChart);
});

onBeforeUnmount(() => {
  window.removeEventListener("resize", resizeChart);
  chart?.dispose();
  chart = null;
});
</script>

<style scoped>
.toolbar {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  flex-wrap: wrap;
}

.chart-wrap {
  margin-top: 20px;
  padding-top: 12px;
  border-top: 1px dashed #e5e7eb;
}

.chart {
  width: 100%;
  height: 360px;
}
</style>
