<template>
  <div class="admin-page">
    <div class="admin-heading">
      <div>
        <h1 class="page-title">后台管理</h1>
        <p class="page-subtitle">用户状态、角色权限和后台入口配置</p>
      </div>
      <el-button :icon="Refresh" :loading="refreshing" @click="reloadAll">刷新</el-button>
    </div>

    <div class="metric-grid">
      <section v-for="item in metrics" :key="item.label" class="metric-card">
        <div class="metric-label">{{ item.label }}</div>
        <div class="metric-value">{{ item.value }}</div>
      </section>
    </div>

    <section class="admin-panel">
      <div class="panel-toolbar">
        <div class="filter-group">
          <el-input
            v-model.trim="filters.keyword"
            class="keyword-input"
            :prefix-icon="Search"
            placeholder="用户名 / 邮箱 / 昵称"
            clearable
            @keyup.enter="handleSearch"
          />
          <el-select v-model="filters.status" placeholder="状态" clearable class="filter-select">
            <el-option label="启用" value="ACTIVE" />
            <el-option label="禁用" value="DISABLED" />
          </el-select>
          <el-select v-model="filters.roleName" placeholder="角色" clearable class="filter-select">
            <el-option v-for="role in roles" :key="role.code" :label="role.name" :value="role.code" />
          </el-select>
        </div>
        <el-button type="primary" :icon="Search" @click="handleSearch">查询</el-button>
      </div>

      <el-table v-loading="usersLoading" :data="users" row-key="userId" stripe>
        <el-table-column label="用户" min-width="230">
          <template #default="{ row }">
            <div class="user-cell">
              <span class="user-name">{{ row.displayName || row.username }}</span>
              <span class="user-meta">@{{ row.username }} · {{ row.email || "未设置邮箱" }}</span>
            </div>
          </template>
        </el-table-column>
        <el-table-column label="状态" width="140">
          <template #default="{ row }">
            <el-switch
              :model-value="row.status === 'ACTIVE'"
              :disabled="row.userId === currentUserId"
              active-text="启用"
              inactive-text="禁用"
              inline-prompt
              @change="(value) => updateStatus(row, value)"
            />
          </template>
        </el-table-column>
        <el-table-column label="角色" min-width="170">
          <template #default="{ row }">
            <el-select
              :model-value="row.roleName"
              :disabled="row.userId === currentUserId"
              class="role-select"
              @change="(value) => updateRole(row, value)"
            >
              <el-option v-for="role in roles" :key="role.code" :label="role.name" :value="role.code" />
            </el-select>
          </template>
        </el-table-column>
        <el-table-column label="积分" prop="totalScore" width="100" />
        <el-table-column label="学习天数" prop="studyDays" width="110" />
        <el-table-column label="最近登录" min-width="180">
          <template #default="{ row }">{{ formatTime(row.lastLoginAt) }}</template>
        </el-table-column>
        <el-table-column label="创建时间" min-width="180">
          <template #default="{ row }">{{ formatTime(row.createdAt) }}</template>
        </el-table-column>
      </el-table>

      <div class="pager">
        <el-pagination
          v-model:current-page="page"
          v-model:page-size="size"
          :page-sizes="[10, 20, 50]"
          :total="total"
          layout="total, sizes, prev, pager, next"
          @size-change="loadUsers"
          @current-change="loadUsers"
        />
      </div>
    </section>

    <section class="role-panel">
      <div class="panel-title">角色权限</div>
      <el-table :data="roles" row-key="code" border>
        <el-table-column label="角色" width="150">
          <template #default="{ row }">
            <strong>{{ row.name }}</strong>
            <span class="role-code">{{ row.code }}</span>
          </template>
        </el-table-column>
        <el-table-column label="说明" prop="description" min-width="180" />
        <el-table-column label="权限点" min-width="320">
          <template #default="{ row }">
            <div class="permission-list">
              <el-tag v-for="permission in row.permissions" :key="permission" effect="plain">
                {{ permission }}
              </el-tag>
            </div>
          </template>
        </el-table-column>
      </el-table>
    </section>
  </div>
</template>

<script setup>
import { computed, onMounted, reactive, ref } from "vue";
import { ElMessage, ElMessageBox } from "element-plus";
import { Refresh, Search } from "@element-plus/icons-vue";
import { adminApi } from "../api/modules";
import { useAuthStore } from "../stores/auth";

const authStore = useAuthStore();

const overview = ref({
  totalUsers: 0,
  activeUsers: 0,
  disabledUsers: 0,
  adminUsers: 0
});
const roles = ref([]);
const users = ref([]);
const total = ref(0);
const page = ref(1);
const size = ref(10);
const usersLoading = ref(false);
const refreshing = ref(false);

const filters = reactive({
  keyword: "",
  status: "",
  roleName: ""
});

const currentUserId = computed(() => authStore.user?.userId);

const metrics = computed(() => [
  { label: "用户总数", value: overview.value.totalUsers },
  { label: "启用用户", value: overview.value.activeUsers },
  { label: "禁用用户", value: overview.value.disabledUsers },
  { label: "管理员", value: overview.value.adminUsers }
]);

onMounted(() => {
  reloadAll();
});

async function reloadAll() {
  refreshing.value = true;
  try {
    await Promise.all([loadOverview(), loadRoles(), loadUsers()]);
  } finally {
    refreshing.value = false;
  }
}

async function loadOverview() {
  overview.value = await adminApi.overview();
}

async function loadRoles() {
  roles.value = await adminApi.roles();
}

async function loadUsers() {
  usersLoading.value = true;
  try {
    const data = await adminApi.users({
      keyword: filters.keyword || undefined,
      status: filters.status || undefined,
      roleName: filters.roleName || undefined,
      page: page.value - 1,
      size: size.value
    });
    users.value = data.items || [];
    total.value = data.total || 0;
  } catch (error) {
    ElMessage.error(error.message || "加载用户失败");
  } finally {
    usersLoading.value = false;
  }
}

function handleSearch() {
  page.value = 1;
  loadUsers();
}

async function updateStatus(row, enabled) {
  const nextStatus = enabled ? "ACTIVE" : "DISABLED";
  if (row.status === nextStatus) {
    return;
  }

  if (nextStatus === "DISABLED") {
    try {
      await ElMessageBox.confirm(`确认禁用用户 ${row.username}？`, "禁用用户", {
        confirmButtonText: "禁用",
        cancelButtonText: "取消",
        type: "warning"
      });
    } catch {
      return;
    }
  }

  try {
    const updated = await adminApi.updateUserStatus(row.userId, nextStatus);
    mergeUser(updated);
    await loadOverview();
    ElMessage.success("状态已更新");
  } catch (error) {
    ElMessage.error(error.message || "更新状态失败");
  }
}

async function updateRole(row, roleCode) {
  if (row.roleName === roleCode) {
    return;
  }

  try {
    const updated = await adminApi.updateUserRole(row.userId, roleCode);
    mergeUser(updated);
    await loadOverview();
    ElMessage.success("角色已更新");
  } catch (error) {
    ElMessage.error(error.message || "更新角色失败");
  }
}

function mergeUser(updated) {
  const index = users.value.findIndex((item) => item.userId === updated.userId);
  if (index >= 0) {
    users.value[index] = updated;
  }
}

function formatTime(value) {
  if (!value) {
    return "-";
  }
  return String(value).replace("T", " ").slice(0, 19);
}
</script>

<style scoped>
.admin-page {
  display: grid;
  gap: 18px;
}

.admin-heading {
  display: flex;
  align-items: flex-start;
  justify-content: space-between;
  gap: 12px;
}

.metric-grid {
  display: grid;
  grid-template-columns: repeat(4, minmax(0, 1fr));
  gap: 12px;
}

.metric-card {
  min-height: 96px;
  padding: 18px;
  border: 1px solid #e5e7eb;
  border-radius: 8px;
  background: #ffffff;
}

.metric-label {
  color: #64748b;
  font-size: 14px;
}

.metric-value {
  margin-top: 10px;
  color: #111827;
  font-size: 30px;
  font-weight: 800;
}

.admin-panel,
.role-panel {
  padding: 18px;
  border: 1px solid #e5e7eb;
  border-radius: 8px;
  background: #ffffff;
}

.panel-toolbar {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 12px;
  margin-bottom: 14px;
}

.filter-group {
  display: flex;
  align-items: center;
  gap: 10px;
  min-width: 0;
}

.keyword-input {
  width: 260px;
}

.filter-select {
  width: 130px;
}

.user-cell {
  display: grid;
  gap: 4px;
}

.user-name {
  color: #111827;
  font-weight: 700;
}

.user-meta,
.role-code {
  color: #64748b;
  font-size: 13px;
}

.role-select {
  width: 128px;
}

.pager {
  display: flex;
  justify-content: flex-end;
  margin-top: 14px;
}

.panel-title {
  margin-bottom: 12px;
  font-size: 16px;
  font-weight: 800;
  color: #111827;
}

.permission-list {
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
}

.role-code {
  display: block;
  margin-top: 4px;
}

@media (max-width: 960px) {
  .admin-heading,
  .panel-toolbar,
  .filter-group {
    align-items: stretch;
    flex-direction: column;
  }

  .metric-grid {
    grid-template-columns: repeat(2, minmax(0, 1fr));
  }

  .keyword-input,
  .filter-select {
    width: 100%;
  }
}

@media (max-width: 560px) {
  .metric-grid {
    grid-template-columns: 1fr;
  }
}
</style>
